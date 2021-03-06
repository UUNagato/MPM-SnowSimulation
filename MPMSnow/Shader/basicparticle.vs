#version 330
layout(location=0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_PointSize = 2.0;
	gl_Position = projection * view * model * vec4(aPos, 1.0);
}