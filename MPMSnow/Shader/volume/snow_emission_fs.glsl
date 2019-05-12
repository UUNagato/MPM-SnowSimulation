#version 330
out vec4 FragColor;

in vec4 ProjectionPosition;

// Start and End map
uniform sampler2D startMap;
uniform sampler2D endMap;
uniform sampler3D volumeData;

// World transform to calculate real length
uniform mat4 model;

void main()
{
	// get screen position
	vec2 spos = ProjectionPosition.xy / ProjectionPosition.w;
	spos = 0.5 * spos + 0.5;

	vec3 startPos = texture(startMap, spos).rgb;
	vec3 endPos = texture(endMap, spos).rgb;

	vec3 dir = endPos - startPos;
	float max_t = length(dir);
	dir = dir / max_t;

	vec4 worldDir = model * vec4(dir,0);
	float realLength = length(worldDir);

	// ray marching to integrate light transportation.
	const float step_size = 0.001;
	float t = 0;
	int nSample = 0;		// number of samples

	// parameter
	vec3 sigma_t = vec3(30);
	vec3 tau = vec3(0);			// since it's integrated
	vec3 transmittance = vec3(1);
	vec3 emission_color = vec3(95, 98, 87);		// emission color, snow is white.

	// integral value
	vec3 Li = vec3(0);

	for (t = 0; t < max_t; t += step_size) {
		vec3 pos = startPos + t * dir;
		float density = texture(volumeData, pos).r;

		// calculate transmittance
		density = min(10.0, density);
		tau += sigma_t * density;		// current cumulative tau.
		// current pos transmittance
		transmittance = exp(-tau * step_size * realLength);
		// integrate to radiance
		Li += emission_color * transmittance;
		nSample += 1;
		// early terminate
		if (transmittance.y < 0.001)
			break;
	}

	Li = Li * step_size * realLength;
	// HDR
	Li = Li / (Li + vec3(1));
	// Gamma tuning
	Li = pow(Li, vec3(1.0 / 2.2));
	// alpha is approx
	FragColor = vec4(Li, 1.0 - transmittance.y);
}