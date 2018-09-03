#version 410 core

const int VertexCount = 6;
const vec2 Position[VertexCount] = vec2[](
	vec2(-1.0,-1.0),
	vec2( 1.0,-1.0),
	vec2( 1.0, 1.0),
	vec2(-1.0,-1.0),
	vec2( 1.0, 1.0),
	vec2(-1.0, 1.0));

const vec2 madd=vec2(0.5,0.5);

out vec2 texcoord;

void main()
{
    texcoord = Position[gl_VertexID] * madd + madd;
    gl_Position = vec4(Position[gl_VertexID], 0.0, 1.0);
}
