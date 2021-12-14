#version 410
out vec4 color;

in vec3 Color; 
in vec3 Posi;
in vec3 Normal;
in vec3 FragPos;

uniform vec3 lightPos;
uniform vec3 lightColour;


void main()
{
	

	 
	 //color=vec4(1.0,0.0,0.0,1.0);
	
	 //vec3 lightColor=vec3(1.0,1.0,1.0);
	float ambient = 0.2f;

	// diffuse lighting
	vec3 normal = normalize(Normal);
	//vec3 light=normalize(lightPos);
	vec3 Frag=normalize(FragPos);
	vec3 light=normalize(lightPos);
	vec3 lightDirection = normalize( lightPos-FragPos);
	float diffuse = max(dot(normal, lightDirection), 0.0f);
	vec3 diff=diffuse*lightColour;
	// specular lighting
	float specularLight = 0.8f;
	vec3 viewDirection = normalize(vec3(0.0f,0.0,-1.0f) - FragPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;
	vec3 sp=specular*lightColour;

	if(diffuse==0)
	sp=vec3(0.0,.0,.0);
		vec3 result= (diff+ambient+sp)*Color;
	color = vec4( result,1.0);
	
	
	}