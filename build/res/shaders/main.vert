#version 420 core

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec4 in_colors;

out vec4 pass_colors;

void main()
{
    pass_colors = in_colors;
    gl_Position = vec4(in_position, 1.0);
}

