#version 430 core

in vec3 vPos;
out vec4 fragment_output;

void main()
{
	fragment_output = vec4(vPos,1);
}
