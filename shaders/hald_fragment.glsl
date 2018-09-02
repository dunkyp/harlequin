#version 410

out vec4 fragColour;
uniform sampler3D clut;
uniform sampler2D source;
// add cube sizes here

in vec2 texcoord;

void main()
{
    vec4 colour;
    colour = texture(source, texcoord);
    colour = texture(clut, colour.rgb);
    fragColour = vec4(colour.rgb, 1.0);
}
