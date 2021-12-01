#version 450

layout(location = 0) in vec4 v_Color; // The varying we took in from the vertex sheydar
layout(location = 1) in vec2 v_Position;

layout(location = 0) out vec4 o_Color; // We send the final color to the attachment of ID 0

void main() {
	float d = 1.0 - distance(v_Position, vec2(0.0, 0.0));
	if(d <= 0.0) {
		discard;
	}
    o_Color = v_Color;
}