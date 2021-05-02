#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

uniform sampler2D textura1;
uniform sampler2D textura2;
uniform vec3 viewPos;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform float spotCutoff;

void main(){

	vec4 finalTextura = mix(texture(textura1, TexCoord), texture(textura2, TexCoord), 0.5);

	//ambient
	float ambientI = 0.3;
	vec3 ambient = ambientI * lightColor;

	vec3 ld = normalize(vec3(0.0,0.0,-lightPos.z ));
	vec3 fd = normalize(vec3(FragPos - lightPos));

	if(acos(dot(fd,ld)) <= radians(spotCutoff)){

		//diffuse
		vec3 norm = normalize(Normal);
		vec3 lightDir = normalize(lightPos - FragPos);
		float diff = max(dot(norm, lightDir), 0.0);
		vec3 diffuse = diff * lightColor;

		//specular
		float specularStrength = 0.3;
		vec3 viewDir = normalize (viewPos - FragPos);
		vec3 reflectDir = reflect(-lightDir, norm);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), 1.0);
		vec3 specular = specularStrength * spec * lightColor;

		vec3 result = (ambient + diffuse + specular) * objectColor;
		FragColor = vec4 (result,1.0);
        
		//FragColor= mix(FragColor,finalTextura,0.9);
		FragColor = FragColor * finalTextura;
        
	}else{

		vec3 result = (ambient) * objectColor;
		FragColor = vec4 (result,1.0);
        
		//FragColor= mix(FragColor,finalTextura,0.5);
		FragColor= FragColor * finalTextura;
	}
}