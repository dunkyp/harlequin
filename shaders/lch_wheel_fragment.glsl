#version 410 core

const float PI = 3.1415926535897932384626433832795;

in vec2 coord;
out vec4 fragColour;
uniform float brightness;
uniform float greyFactor;

vec3 lch2lab(in vec3 LCH) {
    float c = LCH.y;
    LCH.y = cos(LCH.z) * c;
    LCH.z = sin(LCH.z) * c;
    return LCH;
}

vec3 lab2xyz(in vec3 c) {
    float fy = ( c.x + 16.0 ) / 116.0;
    float fx = c.y / 500.0 + fy;
    float fz = fy - c.z / 200.0;
    return vec3(
                95.047 * (( fx > 0.206897 ) ? fx * fx * fx : ( fx - 16.0 / 116.0 ) / 7.787),
                100.000 * (( fy > 0.206897 ) ? fy * fy * fy : ( fy - 16.0 / 116.0 ) / 7.787),
                108.883 * (( fz > 0.206897 ) ? fz * fz * fz : ( fz - 16.0 / 116.0 ) / 7.787)
                );
}

vec3 xyz2rgb(in vec3 c) {
    const mat3 mat = mat3(
                          3.2406, -1.5372, -0.4986,
                          -0.9689, 1.8758, 0.0415,
                          0.0557, -0.2040, 1.0570
                          );
    vec3 v = (c / 100.0) * mat;
    vec3 r;
    r.x = ( v.r > 0.0031308 ) ? (( 1.055 * pow( v.r, ( 1.0 / 2.4 ))) - 0.055 ) : 12.92 * v.r;
    r.y = ( v.g > 0.0031308 ) ? (( 1.055 * pow( v.g, ( 1.0 / 2.4 ))) - 0.055 ) : 12.92 * v.g;
    r.z = ( v.b > 0.0031308 ) ? (( 1.055 * pow( v.b, ( 1.0 / 2.4 ))) - 0.055 ) : 12.92 * v.b;
    return r;
}

float extend(float v) {
    float lv = log(v);
    float minv = log(1);
    float maxv = log(2);
    return (lv - minv) / (maxv-  minv);
}

void main() {
    float l = brightness * 100;
    vec2 centre = vec2(0.5, 0.5);
    vec2 pos = coord - centre;
    float chroma = pow(length(pos) * 2, greyFactor) * 100.0;
    float hue = atan(pos.y, pos.x);
    vec3 rgb = xyz2rgb(lab2xyz(lch2lab(vec3(l, chroma, hue))));
    float dist = sqrt(dot(pos, pos));
    float t = smoothstep(0.5, 0.497, dist);
    fragColour = mix(vec4(0.0), vec4(rgb, 1.0), t);
}
