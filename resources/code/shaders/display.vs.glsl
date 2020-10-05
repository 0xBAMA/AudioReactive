#version 430 core

in vec3 vPosition;
out vec3 vPos;

uniform float theta;
uniform float phi;

// uniform vec3 offset;
// uniform float scale;

//thanks to Neil Mendoza via http://www.neilmendoza.com/glsl-rotation-about-an-arbitrary-axis/
mat3 rotationMatrix(vec3 axis, float angle)
{
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;

    return mat3(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c);
}


void main()
{

	vec3 offset = vec3(0, 0.2*sin(5*vPosition.x*vPosition.z), 0);

	mat3 rotphi = rotationMatrix(vec3(1,0,0), -1.0*phi);
	mat3 rottheta = rotationMatrix(vec3(0,1,0), -1.0*theta);

	vPos = (rotphi * rottheta * 0.6 * ((vPosition.y>-0.1) ? vPosition+offset : vPosition));
	
	gl_Position = vec4(vPos,1.0);
}
