#version 330 core
out vec4 FragColor;

in vec3 LocalPos;
in vec3 WorldPos;
in vec4 ProjectedPos;

void main()
{
	// easy task, just render its world position
	FragColor = vec4(LocalPos, 1.0);
}