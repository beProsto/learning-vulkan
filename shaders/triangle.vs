#version 450

layout(location = 0) out vec4 v_Color; // A varying variable we'll send to the fragment shader

vec3 m_Positions[6] = vec3[](
	vec3(-0.5,  0.5, 0.0),
	vec3(-0.5, -0.5, 0.0),
	vec3( 0.5, -0.5, 0.0),
	
	vec3(-0.5,  0.5, 0.0),
	vec3( 0.5, -0.5, 0.0),
	vec3( 0.5,  0.5, 0.0)
);

void main() {
	gl_Position = vec4(m_Positions[gl_VertexIndex], 1.0);
	v_Color = vec4(m_Positions[gl_VertexIndex] * vec3(2.0), 1.0);
}