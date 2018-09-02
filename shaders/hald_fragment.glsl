#version 410

out vec4 fragColour;
uniform sampler3D clut;
uniform sampler2D source;
uniform float y_cut;
uniform float x_cut;

in vec2 texcoord;

void main()
{
    vec4 colour;
    colour = texture(source, texcoord);
    colour = (texcoord.y > y_cut && texcoord.x > x_cut) ? colour = texture(clut, colour.rgb) : colour;
    fragColour = vec4(colour.rgb, 1.0);
}
