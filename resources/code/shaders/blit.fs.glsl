#version 430 core

in vec2 v_pos;

//layout( binding = 1, r32ui ) uniform uimage2D current;

out vec4 fragment_output;

void main()
{
	if(int(gl_FragCoord.y) % 3 == 0)
		fragment_output = vec4(0.1*v_pos.x,0.6*v_pos.y,0,1);
	else
		fragment_output = vec4(0,0,0,1);
}
