#version 330 core
out vec4 FragColor;

in vec3 TexCoords;
in vec3 skyColor;

uniform samplerCube skybox;
uniform vec3 lightDir;



vec3 getSkyColor(){
	// use texcoord Y to determine how high, great textCoor Y to 1, more blue
	// less more white
	float cutOff = 0.7;
	//vec3 skyColor = vec3(0.203921f, 0.329411f, 0.631372f); // darker blue
	vec3 skyColor = vec3(0.52941f, 0.80784f, 0.92156f); // lighter
	//vec3 lowerColor = vec3(0.67450f, 0.98039f, 1.0f);
	vec3 lowerColor = vec3(1.0f, 1.0f, 1.0f); 
	//float skyStrength = min(max(TexCoords.y, 0.0) + cutOff, 1.0);
	float skyStrength = min(max(TexCoords.y, 0.0) + cutOff, 1.0);
	float inverse = 1.0 - skyStrength;
	return inverse * lowerColor + skyStrength * skyColor; 


}


void main()
{    
	
	//vec3 skyColor = vec3(0.52941f, 0.80784f, 0.92156f);
	
	float dotProduct =  max(normalize(lightDir).y, 0.0) ; // note the greater y component is, further away from 0 in y axis, since normalized 
	float inverse = 1.0 - dotProduct; 
    FragColor = (texture(skybox, TexCoords) * inverse) + vec4(dotProduct * getSkyColor(), 1.0);
}