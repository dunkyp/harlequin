#version 410 core

layout(location = 0) in vec3 rgb_colour;

uniform float greyFactor;

const vec3 colorspace_LabWts_ = vec3(95.0456, 100.0, 108.8754);

vec3 rgb2xyz(in vec3 RGB) {
    bvec3 tmp = greaterThan(RGB, vec3(0.04045));
    RGB =   (vec3(tmp)      * pow((RGB + 0.055) / 1.055, vec3(2.4))) 
          + (vec3(not(tmp)) * (RGB / 12.92));

    RGB *= 100.0;

    const mat3 conversionMatrix = mat3(
        0.412453, 0.357580, 0.180423,
        0.212671, 0.715160, 0.072169,
        0.019334, 0.119193, 0.950227);
    return RGB * conversionMatrix;
}

vec3 xyz2lab(in vec3 XYZ) {
    XYZ /= colorspace_LabWts_;

    bvec3 mask = greaterThan(XYZ, vec3(0.008856));
    XYZ =   (vec3(mask)      * pow(XYZ, vec3(1.0/3.0)))
          + (vec3(not(mask)) * (7.787 * XYZ + 16.0/116.0));

    return vec3(116.0 * XYZ.y - 16.0,
                500.0 * (XYZ.x - XYZ.y),
                200.0 * (XYZ.y - XYZ.z));
}

vec3 lab2lch(in vec3 LAB) {
     vec3 LCH = LAB;
     LCH.z = atan(LAB.z * 2 - 1, LAB.y * 2 - 1);
     LCH.y = sqrt(pow(LAB.y, 2.0) + pow(LAB.z, 2.0));
     return LCH;
}


vec3 rgb2lch(in vec3 RGB) {
     return lab2lch(xyz2lab(rgb2xyz(RGB)));
}


void main() {
    vec3 lch = rgb2lch(rgb_colour);
    lch.y = pow(lch.y / 100.0, 1.0/greyFactor) * 100.0;
    vec2 coords = vec2(lch.y / 100.0 * cos(lch.z), lch.y / 100.0 * sin(lch.z));

    gl_Position = vec4(coords, 0.0, 1.0);
}
