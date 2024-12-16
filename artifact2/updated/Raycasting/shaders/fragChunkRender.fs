#version 330 core
out vec4 fragColor;

uniform sampler3D blocks;
uniform float textWidth;
uniform float textHeight;
uniform float textDepth; 
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float stoneTextWidth;
uniform float stoneTextHeight; 
uniform float screenWidth;
uniform float screenHeight;
uniform float iTime;
uniform vec3 camPos;
uniform vec3 camDir;
uniform vec3 camPlaneU;
uniform vec3 camPlaneV; 

uniform vec3 lightDir; // direction to the light source from origin
uniform vec3 lightColor; // gradually changes overtime 

const int MAX_DISTANCE = 3000;  
const float maxHeight = 255.0; // what 1.0 should be

bool hit = false;

vec2 rotate2d(vec2 v, float a) {
	float sinA = sin(a);
	float cosA = cos(a);
	return vec2(v.x * cosA - v.y * sinA, v.y * cosA + v.x * sinA);	
}

/* used with height map to determine if ray hit a voxel*/
bool sample(ivec3 mapPos){
	if(mapPos.x < 0 || mapPos.z < 0 || mapPos.x >= textWidth || mapPos.z >= textDepth){
		return false; 
	}
	else if(mapPos.y < 0 || mapPos.y >= textHeight){ // have a bottom
		return false;
	}

	float block = texelFetch(blocks, mapPos.xyz, 0).r; // only uses red channel
	return block != 0.0;  

}



bool getVoxel(ivec3 mapPos) {
	return (mapPos.x == 0 && mapPos.y == 0 && mapPos.z == -12) || sample(mapPos);
    //return mapPos.x == 0 && mapPos.y == 0 && mapPos.z == 0;
}

vec3 calcLight(vec3 norm, vec3 diffuse){
	vec3 ambient = 0.4 * lightColor;
	vec3 normLightDir = normalize(lightDir); // path to sun
	vec3 dir = -normLightDir; // sun to origin
	float diff = max(dot(norm, dir), 0.0);
	vec3 diffuseTotal = diff * lightColor ;
	// the higher the dir.y is the lower the light is on the horizon, therefore we use this to alter diffuse strength, to reflect time of day,ish
	// this will reduce diffuse effect at night, when sun is beneath terrain
	float sub = max(0.3 * dir.y, 0.0); // we subtract a fraction of it to reduce the strength, further under the terrain higher it is
	diffuseTotal.x = max(diffuseTotal.x - sub, 0.0) ;
	diffuseTotal.y = max(diffuseTotal.y - sub, 0.0) ;
	diffuseTotal.z = max(diffuseTotal.z - sub, 0.0) ;
	
	
	
	return (ambient + diffuseTotal) * diffuse; 
}


void main(){
	vec2 fragCoord = gl_FragCoord.xy;
	vec2 screenPos = vec2(fragCoord.x / screenWidth, fragCoord.y / screenHeight) * 2.0 - 1.0;
	// vec3 cameraDir = camDir;
	// vec3 cameraPlaneU = camPlaneU;
	vec3 cameraPlaneV = camPlaneV * (screenHeight / screenWidth);
	vec3 rayDir = normalize(camDir + screenPos.x * camPlaneU + screenPos.y * cameraPlaneV); // make sure normalized
	vec3 rayPos = camPos; // could get rid of to minimize foot print

	ivec3 mapPos = ivec3(floor(rayPos + 0.));
    	vec3 deltaDist = abs(vec3(length(rayDir)) / rayDir);
    	ivec3 rayStep = ivec3(sign(rayDir));
    	vec3 sideDist = (sign(rayDir) * (vec3(mapPos) - rayPos) + (sign(rayDir) * 0.5) + 0.5) * deltaDist; 
    	
	float totalDist = 0.0; 
	vec3 currentRayPos;

    	bvec3 mask;
	
	for (int i = 0; i < MAX_DISTANCE; i++) {
		if (getVoxel(mapPos)){
			hit = true;
			break;
		}
		else if(( mapPos.y < 0 && rayDir.y < 0 )|| (mapPos.y > textHeight && rayDir.y > 0.0)){ // voxels past maxHeight are empty by default, stop raycasting if ray pointing further upwards
			break; // ray looking into space
			// can add one for outofbounds too
		}
		else if(( mapPos.x < 0 && rayDir.x < 0 )|| ( mapPos.x > textWidth && rayDir.x > 0.0) ){ // voxels past maxHeight are empty by default, stop raycasting if ray pointing further upwards
			break; // ray looking into space
			// can add one for outofbounds too
		}
		else if(( mapPos.z < 0 && rayDir.z < 0 ) || (mapPos.z > textDepth && rayDir.z > 0.0)){ // voxels past maxHeight are empty by default, stop raycasting if ray pointing further upwards
			break; // ray looking into space
			// can add one for outofbounds too
		}


		if (sideDist.x < sideDist.y) {
			if (sideDist.x < sideDist.z) {
				totalDist = sideDist.x;			
				sideDist.x += deltaDist.x;
				mapPos.x += rayStep.x;
				mask = bvec3(true, false, false);
			}
			else {
				totalDist = sideDist.z;
				sideDist.z += deltaDist.z;
				mapPos.z += rayStep.z;
				mask = bvec3(false, false, true);
			}
		}
		else {
			if (sideDist.y < sideDist.z) {
				totalDist = sideDist.y;
				sideDist.y += deltaDist.y;
				mapPos.y += rayStep.y;
				mask = bvec3(false, true, false);
			}
			else {
				totalDist = sideDist.z;
				sideDist.z += deltaDist.z;
				mapPos.z += rayStep.z;
				mask = bvec3(false, false, true);
			}
		}
		
	}
	



	vec3 color;
	if(hit){
		currentRayPos =  rayPos + (rayDir * totalDist);
		vec3 normal; 
		vec3 diffuse; 
		if (mask.x) { 
			//zy
			normal = vec3(rayStep.x, 0.0, 0.0); // need opposite of ray direction to get normal
			diffuse = texture(texture1, vec2(fract(currentRayPos.zy))).rgb; 
			color.rgb = calcLight(normal, diffuse);
			// color = vec3(0.5);
			
		}
		if (mask.y) {
			//xz
			normal = vec3(0.0, rayStep.y, 0.0);
			diffuse = texture(texture2, vec2(fract(currentRayPos.xz))).rgb;
			color.rgb =  calcLight(normal, diffuse);
			//color = vec3(1.0);
		}
		if (mask.z) {
			//xy
			normal = vec3(0.0, 0.0, rayStep.z);
			diffuse = texture(texture1, vec2(fract(currentRayPos.xy))).rgb; 
			color.rgb =  calcLight(normal, diffuse); 
			//color = vec3(0.75);
		}
	}
	else{
		//color = vec3(0.52941, 0.80784, 0.92156); 
		discard; // do not draw
	}
		
	fragColor.rgb = color;
	// vec2 normalCoord = mapPos.xz/ (maxCoord-1);
	// fragColor.rgb = vec3(texture(texture0, vec2(0,0)).rgb);
	//fragColor.rgb = vec3(0.1 * noiseDeriv);

}