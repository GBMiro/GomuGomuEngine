#version 330

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 texCoords;
layout(location = 2) in vec3 normal;
uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;  
out vec3 ViewPos;
void main() {
    FragPos = vec3(model * vec4(pos, 1.0));
	ViewPos = vec3(view * vec4(pos,1.0));
	Normal = normal;
	TexCoords = texCoords;
	gl_Position = proj*view*model*vec4(pos, 1.0);
}