//
//  CPUMoments.hpp
//  MNN
//
//  Created by MNN on 2019/02/28.
//  Copyright © 2018, Alibaba Group Holding Limited
//

#ifndef CPUMoments_hpp
#define CPUMoments_hpp

#include "Execution.hpp"

namespace MNN {

class CPUMoments : public Execution {
public:
    CPUMoments(Backend* backend, const MNN::Op* op);
    virtual ~CPUMoments() = default;
    virtual ErrorCode onExecute(const std::vector<Tensor*>& inputs, const std::vector<Tensor*>& outputs) override;
    virtual ErrorCode onResize(const std::vector<Tensor*>& inputs, const std::vector<Tensor*>& outputs) override;

private:
    std::vector<int> mAxis;
    bool mKeepDims;
    std::shared_ptr<Tensor> mMidBuffer;
};

} // namespace MNN

#endif /* CPUMoments_hpp */
