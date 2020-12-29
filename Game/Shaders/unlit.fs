#version 330 core

out vec4 color;

in vec2 TexCoords;
in vec3 Normal;

uniform sampler2D diffuse;

void main() {
	//color = texture2D(diffuse, TexCoords);
	
	color = vec4(1.0,0.0,0.0,1.0);

//color =   vec4(Normal.xyz,1.0);

}