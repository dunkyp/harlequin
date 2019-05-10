#version 430

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
layout (rgba32f, binding = 0) writeonly uniform image3D dest;

vec3 lab2xyz( vec3 c ) {
    float fy = ( c.x + 16.0 ) / 116.0;
    float fx = c.y / 500.0 + fy;
    float fz = fy - c.z / 200.0;
    return vec3(
                95.047 * (( fx > 0.206897 ) ? fx * fx * fx : ( fx - 16.0 / 116.0 ) / 7.787),
                100.000 * (( fy > 0.206897 ) ? fy * fy * fy : ( fy - 16.0 / 116.0 ) / 7.787),
                108.883 * (( fz > 0.206897 ) ? fz * fz * fz : ( fz - 16.0 / 116.0 ) / 7.787)
                );
}

vec3 xyz2rgb( vec3 c ) {
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


void main() {
    ivec3 coordinates = ivec3(gl_GlobalInvocationID);
    imageStore(dest, coordinates, vec4(0.0, 1.0, 0.0, 1.0));
}
