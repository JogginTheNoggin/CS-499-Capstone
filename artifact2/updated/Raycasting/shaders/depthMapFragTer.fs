#version 330 core
out vec4 fragColor;

uniform sampler2D texture0;
uniform float textWidth;
uniform float textHeight;
uniform float screenWidth;
uniform float screenHeight;
uniform float iTime;
uniform vec3 camPos;
uniform vec3 camDir;
uniform vec3 camPlaneU;
uniform vec3 camPlaneV; 

const int MAX_RAY_STEPS = 2000;  
const float maxHeight = 255.0; // what 1.0 should be


bool hit = false;

vec2 rotate2d(vec2 v, float a) {
	float sinA = sin(a);
	float cosA = cos(a);
	return vec2(v.x * cosA - v.y * sinA, v.y * cosA + v.x * sinA);	
}

bool sample(ivec3 mapPos){
	if(mapPos.x < 0 || mapPos.z < 0 || mapPos.x >= textWidth || mapPos.z >= textHeight){
		return false; 
	}
	else if(mapPos.y < 0){ // have a bottom
		return false;
	}

	float height = (texelFetch(texture0, mapPos.xz, 0).r); // all of them share same value, rgb, grey scale heightmap
	float voxelHeight = height * maxHeight;
	return mapPos.y <= voxelHeight; 

}



bool getVoxel(ivec3 mapPos) {
	return (mapPos.x == 0 && mapPos.y == 0 && mapPos.z == -12) || sample(mapPos);
    //return mapPos.x == 0 && mapPos.y == 0 && mapPos.z == 0;
}



void main(){
	vec2 fragCoord = gl_FragCoord.xy;
	vec2 screenPos = vec2(fragCoord.x / screenWidth, fragCoord.y / screenHeight) * 2.0 - 1.0;
	// vec3 cameraDir = camDir;
	// vec3 cameraPlaneU = camPlaneU;
	vec3 cameraPlaneV = camPlaneV * (screenHeight / screenWidth);
	vec3 rayDir = camDir + screenPos.x * camPlaneU + screenPos.y * cameraPlaneV;
	vec3 rayPos = camPos; // could get rid of to minimize foot print

	ivec3 mapPos = ivec3(floor(rayPos + 0.));
    	vec3 deltaDist = abs(vec3(length(rayDir)) / rayDir);
    	ivec3 rayStep = ivec3(sign(rayDir));
    	vec3 sideDist = (sign(rayDir) * (vec3(mapPos) - rayPos) + (sign(rayDir) * 0.5) + 0.5) * deltaDist; 
    	
	vec3 totalDist; 
	vec3 currentRayPos;

    	bvec3 mask;
	
	for (int i = 0; i < MAX_RAY_STEPS; i++) {
		if (getVoxel(mapPos)){
			hit = true;
			break;
		}
		else if(( mapPos.y < 0 && rayDir.y < 0 )|| mapPos.y > maxHeight && rayDir.y > 0.0){ // voxels past maxHeight are empty by default, stop raycasting if ray pointing further upwards
			break; // ray looking into space
			// can add one for outofbounds too
		}
		else if(( mapPos.x < 0 && rayDir.x < 0 )|| ( mapPos.x > textWidth && rayDir.y > 0.0) ){ // voxels past maxHeight are empty by default, stop raycasting if ray pointing further upwards
			break; // ray looking into space
			// can add one for outofbounds too
		}
		else if(( mapPos.z < 0 && rayDir.z < 0 ) || mapPos.z > textHeight && rayDir.z > 0.0){ // voxels past maxHeight are empty by default, stop raycasting if ray pointing further upwards
			break; // ray looking into space
			// can add one for outofbounds too
		}

		totalDist = sideDist; // track before incrementing or will have to adjust when exiting loop
		if (sideDist.x < sideDist.y) {
			if (sideDist.x < sideDist.z) {			
				sideDist.x += deltaDist.x;
				mapPos.x += rayStep.x;
				mask = bvec3(true, false, false);
			}
			else {
				sideDist.z += deltaDist.z;
				mapPos.z += rayStep.z;
				mask = bvec3(false, false, true);
			}
		}
		else {
			if (sideDist.y < sideDist.z) {
				sideDist.y += deltaDist.y;
				mapPos.y += rayStep.y;
				mask = bvec3(false, true, false);
			}
			else {
				sideDist.z += deltaDist.z;
				mapPos.z += rayStep.z;
				mask = bvec3(false, false, true);
			}
		}
		
	}
	
	vec3 color;
	if(hit){
		color = vec3(length(totalDist)) / MAX_RAY_STEPS;
	}
	else{
		color = vec3(0.0); 
	}
		
	fragColor.rgb = color;

}