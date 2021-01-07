#version 440

in vec3 Normal;
in vec3 FragPos;  
in vec2 TexCoords;

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
	//float shininess;
	vec3 Rf0;
};

float GetAttenuation(PointLight p,float d){
    float att=1.0/(p.attenuation.x+p.attenuation.y*d+p.attenuation.z*d*d);
    return att;
}

uniform Material material;
uniform PointLight pointLight;

uniform  vec3 ambient;
//uniform float shininessMax;
uniform vec3 viewPos;
//uniform vec3 objectColor;
uniform bool useToneMapping;
uniform bool useGammaCorrection;
uniform float PI;

out vec4 outColor;

void main() {

	//Ambient 
	vec3 ambient =  vec3(0.1,0.1,0.1) * vec3(texture(material.diffuseTex,TexCoords));
	   
	//Normalizing normal and lightDir vectors     
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(	pointLight.position - FragPos);  

	//Phong diffuse
	float diff = max(dot( lightDir,norm), 0.0);
	vec3 diffuse =   pointLight.color * diff * vec3(texture(material.diffuseTex,TexCoords));
	 
	//Specular texture value (grayScale)
	vec4 specularTexel = texture(material.specularTex,TexCoords);
	 
	 //Material specular color at ang 0
	 vec3 RF0 = material.Rf0 * specularTexel.rgb;
	 
	 //Diffuse normalization
	 diffuse=max(diffuse * ((1-RF0)/PI),0);

	//Incidental Light
	vec3 Li = pointLight.intensity * normalize(pointLight.color);


	//Shininess via alpha channel

	float shininess = exp2(specularTexel.a*7+1);

	//Specular color
	vec3 Rfang = RF0 + ((1-RF0)*max(pow(1 - (dot(lightDir,norm)),5	),0));
	Rfang = Rfang + specularTexel.rgb;

	//View and reflect vectors
	vec3 V = normalize(FragPos-viewPos);
	vec3 R = normalize(reflect(lightDir,norm));



	//Normalized specular calculation
	vec3 normSpec = (((shininess+2)/(2*PI)) * Rfang * max(pow(dot(V,R),shininess),0));
	


	//Distance attenuation
	float dist = abs(length(FragPos - pointLight.position));
	float att = GetAttenuation(pointLight,dist);

	//Phong + Shlick
	vec4 hdr = vec4((diffuse + normSpec) *att  * Li * dot(norm, lightDir),1.0) + vec4(ambient,1) ;
	
	vec3 ldr = hdr.rgb;
	
	if(useToneMapping){
		ldr = ldr / (hdr.rgb + vec3(1.0)); // reinhard tone mapping
	}
	
	if(useGammaCorrection){
		ldr = pow(ldr, vec3(1/2.2)); // gamma correction
	}
	
	outColor = vec4(ldr, 1.0);
	

	
}