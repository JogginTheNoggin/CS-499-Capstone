#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 TexCoords; 
out vec3 FragPos;
out vec2 texCoords;


uniform mat4 view;
uniform mat4 model; 
uniform mat4 projection;

void main()
{
    vec4 pos = projection * view  * model * vec4(aPos, 1.0); 
    gl_Position = pos.xyww; // no idea
}


/*
this shader assumes the vertices are of a plane, this will contain what is to be the sun
the pre processed transformations were designed to remove the translation components
this is done by converting to mat 3 back into mat 4
we also perform rotation on the model matrix to define its position in world space 
this allows the object plane to rotate arround the screen but preventing player from moving closer
the relative world space to camera remain the same (im not exactly sure on the finer details)
*/


// TEXTURE NOT WORKING
