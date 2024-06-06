#ifndef JSTUDIO_MATH_H
#define JSTUDIO_MATH_H

#include "dolphin/mtx.h"
#include "math.h"

namespace JStudio {
namespace math {
    void getRotation_xyz(Mtx44Ptr, f32, f32, f32);
    void rotate_xyz(Mtx44Ptr, CMtx44Ptr, f32, f32, f32);
    void rotate_y(Mtx44Ptr, CMtx44Ptr, f32);
    void getTransformation_SRxyzT(Mtx44Ptr, const Vec&, const Vec&, const Vec&);
    void getTransformation_RyT(Mtx44Ptr, f32, const Vec&);

    inline void getRotation_y(Mtx param_0, f32 param_1) {
        MTXRotRad(param_0, 0x79, DEG_TO_RAD(param_1));
    }

    inline void rotate_xyz(Mtx44Ptr param_0, CMtx44Ptr param_1, const Vec& param_2) {
        rotate_xyz(param_0, param_1, param_2.x, param_2.y, param_2.z);
    }
};
};  // namespace JStudio

#endif /* JSTUDIO_MATH_H */
