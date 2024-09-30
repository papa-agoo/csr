#version 450

// inputs
layout(location = 0) in vec3 color;

// outputs
layout(location = 0) out vec4 fragment_color;

void main()
{
   fragment_color = vec4(color, 1);
}
