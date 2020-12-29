#version 440

struct PointLight {
	vec3 position;
    vec3 color;
    float intensity;
    vec3 attenuation;
};

struct Material{
	sampler2D diffuseTex;
	sampler2D specularTex;
	bool hasSpecularTex;
	float shininess;
	vec3 Rf0;

};

float GetAttenuation(PointLight p,float d){
    float att=1.0/(p.attenuation.x+p.attenuation.y*d+p.attenuation.z*d*d);
    return att;
}

uniform PointLight pointLight;
uniform Material material;

uniform  vec3 ambientColor;

out vec4 outColor;
in vec3 ViewPos;

in vec3 Normal;
in vec3 FragPos;  
in vec2 TexCoords;
void main() {

//Ambient
vec3 ambient =  ambientColor * vec3(texture(material.diffuseTex,TexCoords));
    
//Diffuse
vec3 norm = normalize(Normal);

vec3 lightDir = normalize(	pointLight.position - FragPos);  

float diff = max(dot( lightDir,norm), 0.0);



vec3 diffuse =   pointLight.color * diff * vec3(texture(material.diffuseTex,TexCoords));
vec3 Rf0 = material.Rf0;
 if(material.hasSpecularTex){
	Rf0 *= vec3(texture(material.specularTex,TexCoords));
 }
 float PI = 3.14159;
 
 //Normalize diffuse
 diffuse = max(diffuse * ((1-Rf0)/PI),0);


vec3 Li = pointLight.intensity * pointLight.color.yyz;

vec3 Rfang = Rf0 + ((1-Rf0)*pow(1 - (dot(lightDir,norm)),5	));

if(material.hasSpecularTex){
Rfang = Rfang + vec3(texture(material.specularTex,TexCoords));
}

vec3 V = normalize(FragPos-ViewPos);
vec3 R = normalize(reflect(lightDir,norm));
vec3 normSpec = (((material.shininess+2)/(2*PI)) * Rfang * max(pow(dot(V,R),material.shininess),0));

//Diffuse + Ambient
//outColor = vec4(ambient,1.0) +vec4(diffuse * Li * dot(Normal, lightDir),1.0);

//Ambient only
//outColor = vec4(ambient,1.0);

//Phong + Shlick

float dist = abs(length(FragPos - pointLight.position));


float att = GetAttenuation(pointLight,dist);


//outColor = vec4((diffuse + normSpec) *att  * Li * dot(Normal, lightDir),1.0) + vec4(ambient,1) ;

//Diffuse only 
//outColor = vec4(diffuse * Li * dot(Normal, lightDir),1.0);
 
 
 //outColor = vec4(diffuse,1.0);
 
outColor = vec4(diffuse * Li * dot(Normal, lightDir) + ambient,1.0) ;

  
 
}