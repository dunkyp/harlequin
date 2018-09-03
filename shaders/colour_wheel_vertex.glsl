#version 410 core

uniform highp mat4 qt_Matrix;
in vec4 qt_Vertex;
in vec2 qt_MultiTexCoord0;
out vec2 coord;

void main() {
    coord = qt_MultiTexCoord0;
    gl_Position = qt_Matrix * qt_Vertex;
}
