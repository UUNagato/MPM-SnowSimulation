#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D startTexture;
uniform sampler2D endTexture;

void main()
{
	vec3 startPos = texture(startTexture, TexCoords).rgb;
	vec3 endPos = texture(endTexture, TexCoords).rgb;
	if (startPos == vec3(0.0))
		discard;
	vec3 direction = endPos - startPos;
	float len = length(direction);
	if (len == 0)
		discard;
	FragColor = vec4(endPos, 1.0);
}