#version 330
out vec4 FragColor;

uniform vec3 pointcolor;

void main()
{
	FragColor = vec4(pointcolor, 1.0);
}