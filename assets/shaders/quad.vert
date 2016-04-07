#version 130

// in_Position was bound to attribute index 0 and in_TexCoord was bound to attribute index 1
in vec2 in_Position;
in vec2 in_TexCoord;
 
// We output the ex_Color variable to the next shader in the chain
out vec2 ex_TexCoord;

uniform mat4 transform;

void main(void)
{
    gl_Position = transform * vec4(in_Position, 0.0, 1.0);
    ex_TexCoord = in_TexCoord;
}