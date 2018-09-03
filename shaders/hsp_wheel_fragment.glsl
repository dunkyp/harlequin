#version 410 core
#define M_PI 3.1415926535897932384626433832795

in vec2 coord;
uniform float qt_Opacity;
out vec4 fragColour;
uniform float brightness;
uniform sampler2D source;

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main() {
    float v = brightness / 100.0;
    vec2 centre = vec2(0.5, 0.5);
    vec2 pos = coord - centre;
    float dist = sqrt(dot(pos, pos));
    vec2 twelve = centre - vec2(1.0, 0.0);
    vec2 normalPos = normalize(pos);
    float angle = (atan(normalPos.y, normalPos.x) + M_PI) / (2.0 * M_PI) - 0.25;    
    vec3 rgb = hsv2rgb(vec3(angle, dist, v));
    float t = smoothstep(0.5, 0.497, dist);
    vec4 p = texture(source, coord);
    fragColour = mix(p, vec4(rgb, 1.0), t);;
}
