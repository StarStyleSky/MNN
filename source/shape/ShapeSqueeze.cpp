//
//  ShapeSqueeze.cpp
//  MNN
//
//  Created by MNN on 2019/01/10.
//  Copyright © 2018, Alibaba Group Holding Limited
//

#include "Macro.h"
#include "SizeComputer.hpp"

namespace MNN {

class SqueezeSizeComputer : public SizeComputer {
    virtual bool onComputeSize(const MNN::Op* op, const std::vector<Tensor*>& inputs,
                               const std::vector<Tensor*>& outputs) const override {
        MNN_ASSERT(1 == inputs.size());
        MNN_ASSERT(1 == outputs.size());

        const int* squeezeDim    = op->main_as_SqueezeParam()->squeezeDims()->data();
        const int squeezeDimSize = op->main_as_SqueezeParam()->squeezeDims()->size();

        std::set<int> dimSet;
        for (int i = 0; i < squeezeDimSize; i++) {
            dimSet.insert(squeezeDim[i]);
        }

        auto& ob = outputs[0]->buffer();
        auto ib  = inputs[0]->buffer();

        MNN_ASSERT(squeezeDimSize < ib.dimensions);

        ob.dimensions = ib.dimensions - squeezeDimSize;
        int oDim      = 0;
        for (int i = 0; i < ib.dimensions; i++) {
            if (dimSet.find(i) == dimSet.end()) {
                ob.dim[oDim].extent = ib.dim[i].extent;
                oDim++;
            }
        }

        return true;
    }
};

REGISTER_SHAPE(SqueezeSizeComputer, OpType_Squeeze);
} // namespace MNN
