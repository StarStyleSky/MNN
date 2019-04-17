//
//  GLTexture.h
//  MNN
//
//  Created by MNN on 2019/01/31.
//  Copyright © 2018, Alibaba Group Holding Limited
//

#ifndef GLTEXTURE_H
#define GLTEXTURE_H
/*Basic GLTexture, has no mipmap and just support ARGB GLTexture*/
#include "GLHead.h"
namespace MNN {

class GLTexture {
public:
    GLTexture(int w, int h, int d, GLenum target = GL_TEXTURE_3D, bool HWC4 = true);
    virtual ~GLTexture();
    unsigned int id() const {
        return mId;
    }

    void read(GLuint unit);
    void write(GLuint unit);
    void sample(GLuint unit, GLuint texId);

private:
    unsigned int mId;
    GLenum mTarget;
};
} // namespace MNN

#endif
