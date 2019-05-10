#version 410 core

layout(location = 0) in vec3 rgb_colour;

uniform float greyFactor;

vec3 rgb2hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

void main() {
    vec3 hsv = rgb2hsv(rgb_colour.xyz);
    hsv.y = pow(hsv.y, 1.0/greyFactor);
    vec2 coords = vec2(hsv.y * cos(radians(hsv.x * 360.0)), hsv.y * sin(radians(hsv.x * 360.0)));

    gl_Position = vec4(coords, 0.0, 1.0);
}