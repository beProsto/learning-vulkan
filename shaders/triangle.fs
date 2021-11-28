#version 450

layout(location = 0) in vec4 v_Color; // The varying we took in from the vertex sheydar

layout(location = 0) out vec4 o_Color; // We send the final color to the attachment of ID 0

void main() {
    o_Color = v_Color;
}