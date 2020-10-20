#version 430

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;    //specifies the workgroup size, placeholder

layout (rgba8) uniform image2D front_buffer;
layout (rgba8) uniform image2D back_buffer;

layout( std430, binding = 0 ) buffer data_input
{
	float data[];
};

void main()
{
	if(gl_GlobalInvocationID.x == 0)
	{
		imageStore(front_buffer, ivec2(gl_GlobalInvocationID.xy), vec4(vec3(data[gl_GlobalInvocationID.y]/256.0),1));
	}
	else
	{
		imageStore(front_buffer, ivec2(gl_GlobalInvocationID.xy), imageLoad(back_buffer, ivec2(gl_GlobalInvocationID.xy)-ivec2(1,0)));
	}

	//imageStore(front_buffer, ivec2(gl_GlobalInvocationID.xy), vec4(vec3(data[gl_GlobalInvocationID.y]/100.0), 1));
}
