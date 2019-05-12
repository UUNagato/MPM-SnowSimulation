#version 330
out vec4 FragColor;

in vec3 LocalPos;

uniform sampler3D volumeData;

void main()
{
	FragColor = vec4(texture(volumeData, LocalPos).xxx,1);
}