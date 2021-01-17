#version 440
#define MAX_POINT_LIGHTS 8
in vec3 Normal;
in vec3 FragPos;  
in vec2 TexCoords;

out vec4 outColor;

struct PointLight {
	vec3 position;
    vec3 color;
    float intensity;
    vec3 attenuation;
    float range;
};

struct DirectionalLight{
	vec3 direction;
	vec3 color;
	float intensity;
};

struct Material{

 bool useDiffuseTexture;
 bool useSpecularTexture;
 sampler2D diffuseTex;
 sampler2D specularTex;

//Material values that will be used if no SpecularTexture is found
 vec3 materialRF0;
 float materialShininess;


//Default value if no diffuse texture is found
 vec3 objectColor;
};

float GetAttenuation(PointLight p,float d){
    return 1.0 / (p.attenuation.x + p.attenuation.y * d + p.attenuation.z * d * d);
}

uniform int nPointLights;
uniform PointLight[MAX_POINT_LIGHTS] pointLights;

uniform bool directionalLightFound;
uniform DirectionalLight dirLight;

uniform vec3 ambientColor;
uniform float ambientIntensity;

uniform vec3 viewPos;

uniform Material material;

uniform bool useToneMapping;
uniform bool useGammaCorrection;

vec4 CalcPointLight(PointLight light,vec3 norm, vec3 viewDir, vec3 diffColor,vec3 RF0,float shininess){
	vec3 lightDir = normalize(light.position - FragPos);  
	
	//Incidental Light
	vec3 Li = light.intensity * light.color;
	
	//Specular color
	vec3 Rfang = RF0.rgb + ((1-RF0.rgb)*max(pow(1 - (dot(lightDir,norm)),5	),0));
	
	//Reflection vector
	vec3 R = normalize(reflect(-lightDir,norm));
	
	//Normalized specular calculation (2*PI is ommited)
	vec3 normSpec = (((shininess+2)/(2)) * Rfang * max(pow(dot(viewDir,R),shininess),0));
	
	//Distance attenuation
	float dist = abs(length(FragPos - light.position));
	float att = GetAttenuation(light,dist);
	
	//Phong + Shlick	
	vec4 hdr = vec4((((diffColor.rgb  + normSpec.rgb)   * Li * max(dot(norm, lightDir),0))) * att,1) ;
	return hdr;
}

vec4 CalcDirLight(DirectionalLight light, vec3 norm, vec3 viewDir, vec3 diffColor, vec3 RF0, float shininess){
	vec3 lightDir = -light.direction;
	
	//Incidental Light
	vec3 Li = light.intensity * light.color;
	
	//Specular color
	vec3 Rfang = RF0.rgb + ((1-RF0.rgb)*max(pow(1 - (dot(lightDir,norm)),5	),0));
	
	//Reflection vector
	vec3 R = normalize(reflect(-lightDir,norm));
	
	//Normalized specular calculation (2*PI is ommited)
	vec3 normSpec = (((shininess+2)/(2)) * Rfang * max(pow(dot(viewDir,R),shininess),0));
	
	
	//Phong + Shlick	
	vec4 hdr = vec4((((diffColor.rgb  + normSpec.rgb)   * Li * max(dot(norm, lightDir),0))),1) ;
	return hdr;


}

	void main() {
	
	//Texel color, if diffuse texture found, use its color, if not, use plain color
	vec3 diffColor;
	
	if(material.useDiffuseTexture) {
		diffColor = vec3(texture(material.diffuseTex,TexCoords).rgb);
		//We set diffColor to linear space, so that later on gamma correction works properly
		
		if(useGammaCorrection){
			diffColor = pow(diffColor, vec3(2.2));
		}
	}else {
		diffColor = material.objectColor;
	}
	
	//Ambient light - generated color
	vec3 ambient =  normalize(ambientColor) * ambientIntensity *  diffColor;
	   
	//Normalizing normal and lightDir vectors     
	vec3 norm = normalize(Normal);
	
	//Specular texture value (grayScale). Material specular color at ang 0

	vec3 RF0;
	
	float shininess;
	
	//If specular texture found, get specular color from it, and shininess from
	//alpha channel, if not, use given values
	if(material.useSpecularTexture){
		vec4 specularTexel = texture(material.specularTex,TexCoords);;
	
		RF0 = specularTexel.rgb;
		//Shininess via alpha channel
		shininess = exp2(specularTexel.a*7+1);
	}else{
		RF0 = vec3(material.materialRF0);
		shininess= material.materialShininess;
	} 
	
	//We set RF0 (Specular color) to linear space, so 
	//that later on gamma correction works properly
	if(useGammaCorrection){
		RF0 = pow(RF0.rgb, vec3(2.2));
	}
	 
	//Diffuse normalization (((1-RF0)/PI) is omitted)
	diffColor = diffColor * (1-RF0.rgb);
	
	//View vector
	vec3 V = normalize(viewPos-FragPos);
	
	//Here we would iterate through all Point lights
	
	vec4 hdr = vec4(0,0,0,0);
	for(int i=0;i<nPointLights;i++){
		hdr += CalcPointLight(pointLights[i],norm,V,diffColor,RF0.rgb,shininess);
	}	
	
	if(directionalLightFound){
		hdr += CalcDirLight(dirLight,norm,V,diffColor,RF0.rgb,shininess);
	}	
	
	hdr += vec4(ambient,1);
	
	//Get LDR to use Tonemapping/Gamma Correction
	vec3 ldr = hdr.rgb;
	
	if(useToneMapping){
		ldr = ldr / (ldr + vec3(1.0));
	}
	 
	if(useGammaCorrection){
		ldr = pow(ldr.xyz, vec3(1/2.2));
	}

	outColor = vec4(ldr,1);		
	
}