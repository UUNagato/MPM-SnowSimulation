#version 330 core
// Snow shader, volume rendering
out vec4 FragColor;
in vec4 ProjectionPosition;

// Start and End map
uniform sampler2D startMap;
uniform sampler2D endMap;
uniform sampler3D volumeData;

const float PI = 3.14159265359;

float HenyeyGreenstein(float g, float theta)
{
	float g2 = g * g;
	float nom = 1 - g2;
	float base = 1 + g2 - 2 * g * cos(theta);
	float denom = pow(base, 1.5);

	return base / (denom * 4 * PI);
}

void main()
{
	// get corresponding map
	vec2 sample_pos = ProjectionPosition.xy / ProjectionPosition.w;
	sample_pos = sample_pos * 0.5 + 0.5;					// map from [-1,1] to [0,1] in order to get uv
	vec3 startPos = texture(startMap, sample_pos).rgb;		// since no alpha channel

	vec3 endPos = texture(endMap, sample_pos).rgb;			// end position
	// direction
	vec3 direction = endPos - startPos;
	float max_t = length(direction);						// max t value for ray tracing
	direction = direction / max_t;							// normalize

	// assuming property and evenly density.
	vec3 color = vec3(0.9, 0.95, 1.0);

	
	float foot_step = 0.001;
	float t = 0;
	vec3 c = vec3(0);
	float a = 0;
	vec3 cur_pos = startPos;
	for (t = 0; t < max_t; t = t + foot_step) {
		cur_pos += foot_step * direction;
		float density = texture(volumeData, cur_pos).x;
		density = min(1.0, density);
		c = (1 - a) * color + c;
		a = (1 - a) * density + a;
		if (a > 0.995)
			break;
	}

	FragColor = vec4(vec3(1.0), a);
}