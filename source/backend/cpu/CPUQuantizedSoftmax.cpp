//
//  CPUQuantizedSoftmax.cpp
//  MNN
//
//  Created by MNN on 2018/09/29.
//  Copyright © 2018, Alibaba Group Holding Limited
//

#include "CPUQuantizedSoftmax.hpp"
#include "CPUBackend.hpp"
#include "CPUFixedPoint.hpp"
#include "CPUQuantizationUtils.hpp"
#include "Macro.h"

namespace MNN {

template <typename T>
CPUQuantizedSoftmax<T>::CPUQuantizedSoftmax(Backend* backend, const Op* op) : Execution(backend) {
    auto quantizedSoftmax_param = op->main_as_QuantizedSoftmax();
    mBeta                       = quantizedSoftmax_param->beta();
    mInputScale                 = quantizedSoftmax_param->inputScale();
}

const int kScaledDiffIntegerBits   = 5;
const int kAccumulationIntegerBits = 12;

template <typename T>
ErrorCode CPUQuantizedSoftmax<T>::onResize(const std::vector<Tensor*>& inputs, const std::vector<Tensor*>& outputs) {
    float beta  = mBeta;
    float scale = mInputScale;
    PreprocessSoftmaxScaling(beta, scale, kScaledDiffIntegerBits, &mInputMultiplier, &mInputLeftShift);
    mDiffMin = -1.0 * CalculateInputRadius(kScaledDiffIntegerBits, mInputLeftShift);
    return NO_ERROR;
}

template <typename T>
void CPUQuantizedSoftmax<T>::QuantizedSoftmax(const uint8_t* inputData, const std::vector<int>& inputDims,
                                              int32_t inputBetaMultiplier, int32_t inputBetaLeftShift,
                                              uint8_t* outputData, const std::vector<int>& outputDims) {
    using FixedPointScaledDiff = FixedPoint<int, kScaledDiffIntegerBits>;
    using FixedPointAccum      = FixedPoint<int, kAccumulationIntegerBits>;
    using FixedPoint0          = FixedPoint<int, 0>;

    const int outerSize = inputDims.at(0) * inputDims.at(1) * inputDims.at(2);
    const int depth     = inputDims.at(3);

    for (int b = 0; b < outerSize; ++b) {
        const uint8_t* inputDataPtr = inputData + b * depth;
        uint8_t* outputDataPtr      = outputData + b * depth;

        // Determine the largest entry in the current row
        uint8_t maxInRow = 0;
        {
            int c = 0;
            for (; c < depth; ++c) {
                maxInRow = std::max(maxInRow, inputDataPtr[c]);
            }
        }

        FixedPointAccum sumOfExps = FixedPointAccum::Zero();
        {
            int c = 0;
            for (; c < depth; ++c) {
                int32_t inputDiff = static_cast<int32_t>(inputDataPtr[c]) - maxInRow;
                if (inputDiff >= mDiffMin) {
                    const int32_t inputDiffRescaled =
                        MultiplyByQuantizedMultiplierGreaterThanOne(inputDiff, inputBetaMultiplier, inputBetaLeftShift);
                    const FixedPointScaledDiff scaledDiffF8 = FixedPointScaledDiff::FromRaw(inputDiffRescaled);
                    sumOfExps = sumOfExps + Rescale<kAccumulationIntegerBits>(exp_on_negative_values(scaledDiffF8));
                }
            }
        }

        int fixedSumOfExps  = sumOfExps.raw();
        int headroomPlusOne = __builtin_clz(static_cast<uint32_t>(fixedSumOfExps));

        int numBitsOverUnit        = kAccumulationIntegerBits - headroomPlusOne;
        int32_t shiftedSumMinusOne = static_cast<int32_t>((static_cast<uint32_t>(fixedSumOfExps) << headroomPlusOne) -
                                                          (static_cast<uint32_t>(1) << 31));
        FixedPoint0 shiftedScale   = one_over_one_plus_x_for_x_in_0_1(FixedPoint0::FromRaw(shiftedSumMinusOne));

        {
            int c = 0;
            for (; c < depth; ++c) {
                int32_t inputDiff = static_cast<int32_t>(inputDataPtr[c]) - maxInRow;
                if (inputDiff >= mDiffMin) {
                    const int inputDiffRescaled =
                        MultiplyByQuantizedMultiplierGreaterThanOne(inputDiff, inputBetaMultiplier, inputBetaLeftShift);
                    const FixedPointScaledDiff scaledDiffF8 = FixedPointScaledDiff::FromRaw(inputDiffRescaled);
                    FixedPoint0 expIn0                      = exp_on_negative_values(scaledDiffF8);
                    int unsatOutput  = RoundingDivideByPOT((shiftedScale * expIn0).raw(), numBitsOverUnit + 31 - 8);
                    outputDataPtr[c] = std::max(std::min(unsatOutput, 255), 0);
                } else {
                    outputDataPtr[c] = 0;
                }
            }
        }
    }
}

template <typename T>
ErrorCode CPUQuantizedSoftmax<T>::onExecute(const std::vector<MNN::Tensor*>& inputs,
                                            const std::vector<MNN::Tensor*>& outputs) {
    Tensor* input       = inputs[0];
    Tensor* output      = outputs[0];
    uint8_t* inputData  = input->host<uint8_t>();
    uint8_t* outputData = output->host<uint8_t>();

    std::vector<int> inputDims;
    std::vector<int> outputDims;
    MNN_ASSERT(2 == input->buffer().dimensions || 4 == input->buffer().dimensions);

    if (4 == input->buffer().dimensions) {
        for (int i = 0; i < input->buffer().dimensions; i++) {
            inputDims.push_back(input->buffer().dim[i].extent);
        }
        for (int i = 0; i < output->buffer().dimensions; i++) {
            outputDims.push_back(output->buffer().dim[i].extent);
        }
    } else {
        inputDims.push_back(input->buffer().dim[0].extent);
        inputDims.push_back(1);
        inputDims.push_back(1);
        inputDims.push_back(input->buffer().dim[1].extent);

        outputDims.push_back(input->buffer().dim[0].extent);
        outputDims.push_back(1);
        outputDims.push_back(1);
        outputDims.push_back(input->buffer().dim[1].extent);
    }

    QuantizedSoftmax(inputData, inputDims, mInputMultiplier, mInputLeftShift, outputData, outputDims);

    return NO_ERROR;
}

class CPUQuantizedSoftmaxCreator : public CPUBackend::Creator {
public:
    virtual Execution* onCreate(const std::vector<Tensor*>& inputs, const std::vector<Tensor*>& outputs,
                                const MNN::Op* op, Backend* backend) const {
        return new CPUQuantizedSoftmax<uint8_t>(backend, op);
    }
};
REGISTER_CPU_OP_CREATOR(CPUQuantizedSoftmaxCreator, OpType_QuantizedSoftmax);
} // namespace MNN
