#version 410 core

in vec2 coord;
out vec4 fragColour;

uniform float greyFactor;
uniform float chroma;
uniform float hue;


vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main() {
     float l = 1.0 - coord.y;
     float chroma = pow(chroma, greyFactor);
     vec3 rgb = hsv2rgb(vec3(hue, chroma, l));
     fragColour = vec4(rgb, 1.0);
}
