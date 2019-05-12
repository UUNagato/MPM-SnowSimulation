#version 330 core
// for snow rendering
layout (location = 0) in vec3 aPos;

out vec4 ProjectionPosition;
out vec4 WorldPosition;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	WorldPosition = model * vec4(aPos, 1.0);
	ProjectionPosition = projection * view * WorldPosition;
	gl_Position = ProjectionPosition;
}