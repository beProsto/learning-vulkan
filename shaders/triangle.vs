#version 450

layout(location = 0) out vec4 v_Color; // A varying variable we'll send to the fragment shader
layout(location = 1) out vec2 v_Position;

vec3 m_Positions[6] = vec3[](
	vec3(-0.5,  0.5, 0.0),
	vec3(-0.5, -0.5, 0.0),
	vec3( 0.5, -0.5, 0.0),
	
	vec3(-0.5,  0.5, 0.0),
	vec3( 0.5, -0.5, 0.0),
	vec3( 0.5,  0.5, 0.0)
);

vec3 m_Colors[6] = vec3[](
	vec3(1.0,  0.0, 0.0),
	vec3(0.0, 1.0, 0.0),
	vec3(0.0, 0.0, 1.0),
	
	vec3(1.0, 0.0, 0.0),
	vec3(0.0, 0.0, 1.0),
	vec3(1.0, 0.0, 1.0)
);

void main() {
	gl_Position = vec4(m_Positions[gl_VertexIndex], 1.0);

	v_Position = gl_Position.xy * vec2(2.0);

	float aspect = 9.0/16.0;
	gl_Position.x *= aspect; // make it a fair square :p
	
	v_Color = vec4(m_Colors[gl_VertexIndex], 1.0);
}