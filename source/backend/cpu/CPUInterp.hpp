//
//  CPUInterp.hpp
//  MNN
//
//  Created by MNN on 2018/07/17.
//  Copyright © 2018, Alibaba Group Holding Limited
//

#ifndef CPUInterp_hpp
#define CPUInterp_hpp

#include "Execution.hpp"

namespace MNN {

class CPUInterp : public Execution {
public:
    CPUInterp(Backend *backend, float widthScale, float heightScale, int resizeType, bool AlignCorners);
    virtual ~CPUInterp() = default;
    virtual ErrorCode onExecute(const std::vector<Tensor *> &inputs, const std::vector<Tensor *> &outputs) override;

private:
    float mWidthScale;
    float mHeightScale;
    int mResizeType; // 1:near 2: bilinear 3: cubic
    bool mAlignCorners;
};

} // namespace MNN

#endif /* CPUInterp_hpp */
