#version 410

out vec4 fragColour;
uniform sampler3D clut;
uniform sampler2D source;
uniform float yCut;
uniform float xCut;

in vec2 texcoord;

const vec3 colorspace_LabWts_ = vec3(95.0456, 100.0, 108.8754);

vec3 xyz2lab(in vec3 XYZ) {
    XYZ /= colorspace_LabWts_;

    bvec3 mask = greaterThan(XYZ, vec3(0.008856));
    XYZ =   (vec3(mask)      * pow(XYZ, vec3(1.0/3.0)))
          + (vec3(not(mask)) * (7.787 * XYZ + 16.0/116.0));

    return vec3(116.0 * XYZ.y - 16.0,
                500.0 * (XYZ.x - XYZ.y),
                200.0 * (XYZ.y - XYZ.z));
}

vec3 lab2xyz(in vec3 LAB) {
    vec3 XYZ = vec3(0.0);
    XYZ.y = (LAB.x + 16.0) / 116.0;
    XYZ.x = LAB.y / 500.0 + XYZ.y;
    XYZ.z = XYZ.y - LAB.z / 200.0;

    bvec3 mask = greaterThan(pow(XYZ, vec3(3.0)), vec3(0.008856));
    XYZ =   (vec3(mask)      * pow(XYZ, vec3(3.0)))
          + (vec3(not(mask)) * ((XYZ - 16.0 / 116.0) / 7.787));

    return XYZ * colorspace_LabWts_;
}


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

vec3 xyz2rgb(in vec3 XYZ) {
    const mat3 conversionMatrix = mat3(
        3.240479, -1.537150, -0.498535,
        -0.969256, 1.875992, 0.041556,
        0.055648, -0.204043, 1.057311);

    XYZ = XYZ * conversionMatrix / 100.0;

    bvec3 tmp = greaterThan(XYZ, vec3(0.0031308));
    XYZ =   (vec3(tmp)      * (1.055 * pow(XYZ, vec3(1.0/2.4)) - 0.055))
          + (vec3(not(tmp)) * (XYZ * 12.92));
    
    return XYZ;
}


void main()
{
    vec4 colour;
    colour = texture(source, texcoord);
    colour = (texcoord.y > yCut && texcoord.x > xCut) ? colour = texture(clut, colour.rgb) : colour;
    vec3 lab_colour = xyz2lab(rgb2xyz(colour.rgb));
    fragColour = vec4(xyz2rgb(lab2xyz(lab_colour)), 1.0);
}
