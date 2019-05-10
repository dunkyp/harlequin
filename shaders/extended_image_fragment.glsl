#version 410

out vec4 fragColour;
uniform sampler2D source;

in vec2 texcoord;

void main()
{
    vec4 colour;
    colour = texture(source, texcoord);
    fragColour = colour;
}
