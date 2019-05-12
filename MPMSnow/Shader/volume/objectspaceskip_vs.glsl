#version 330 core
layout (location = 0) in vec3 pos;

out vec3 LocalPos;
out vec3 WorldPos;
out vec4 ProjectedPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	// just transform it
	// LocalPos needs to be normalized. Thus map from [-1, 1] to [0, 1];
	LocalPos = (pos + vec3(1.0)) * 0.5;
	WorldPos = vec3(model * vec4(pos, 1.0));
	ProjectedPos = view * vec4(WorldPos, 1.0);
	gl_Position = projection * ProjectedPos;
}