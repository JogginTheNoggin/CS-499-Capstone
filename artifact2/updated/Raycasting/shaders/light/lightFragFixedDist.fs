#version 330 core
in vec2 texCoords;

out vec4 FragColor;
uniform vec3 lightColor; 
uniform sampler2D sunTexture; 
void main()
{
	//FragColor = texture(sunTexture, texCoords); // set all 4 vector values to 1.0
	FragColor = vec4(1.0, 1.0, 1.0, 1.0);
	
}

/*

Assuming the texture is a plane we use the passed textCoords to sample
Was suppose to be a sun but not working properly


*/