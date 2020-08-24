#version 420 core

layout (location = 0) out vec4 frag_color;

in vec4 pass_colors;

void main()
{
    frag_color  =   pass_colors;
}
