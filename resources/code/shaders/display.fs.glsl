#version 430 core

in vec3 vPos;
out vec4 fragment_output;

uniform sampler2D data_texture;

void main()
{
	// fragment_output = vec4(vec3(vPos.y),1);
	
	vec2 sampleloc = vPos.xz+vec2(0.5);
	sampleloc.x = sqrt(sampleloc.x);
	sampleloc.y = pow(sampleloc.y, 4);
	
	fragment_output = texture2D(data_texture, sampleloc);
	fragment_output.a = 1.0;
}
