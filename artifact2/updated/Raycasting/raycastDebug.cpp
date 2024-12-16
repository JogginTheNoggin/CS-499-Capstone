#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <array>




class Raycaster {
public:
    Raycaster(float screenWidth, float screenHeight, int maxRaySteps,
        const glm::vec3& camPos, const glm::vec3& camDir,
        const glm::vec3& camPlaneU, const glm::vec3& camPlaneV)
        : screenWidth(screenWidth), screenHeight(screenHeight), maxRaySteps(maxRaySteps),
        camPos(camPos), camDir(camDir), camPlaneU(camPlaneU), camPlaneV(camPlaneV) {}


    // Dummy getVoxel function for demonstration
    bool getVoxel(const glm::ivec3& mapPos) {
        // This function should check for voxel presence in a real application
        // Here, we'll just return true if all coordinates are non-negative
        return mapPos.x == 0 && mapPos.y == 0 && mapPos.z == 0;
    }

    void castRay(float fragCoordX, float fragCoordY) {
        glm::vec2 fragCoord = glm::vec2(fragCoordX, fragCoordY);
        glm::vec2 screenPos = (fragCoord / glm::vec2(screenWidth, screenHeight)) * 2.0f - 1.0f;

        glm::vec3 cameraPlaneVAdjusted = camPlaneV * (screenHeight / screenWidth);
        glm::vec3 rayDir = glm::normalize(camDir + screenPos.x * camPlaneU + screenPos.y * cameraPlaneVAdjusted);
        glm::vec3 rayPos = camPos;

        glm::ivec3 mapPos = glm::ivec3(glm::floor(rayPos));
        glm::vec3 deltaDist = glm::abs(glm::length(rayDir) / rayDir); // when only travel on one or the other axis, other values may be infinite
        glm::ivec3 rayStep = glm::ivec3(glm::sign(rayDir));
        glm::vec3 sideDist = (glm::sign(rayDir) * (glm::vec3(mapPos) - rayPos) + (glm::sign(rayDir) * 0.5f) + 0.5f) * deltaDist;
            //(glm::vec3(mapPos) + 0.5f + 0.5f * glm::vec3(rayStep) - rayPos) * deltaDist;

        float totalDist= 0.0;
        glm::vec3 currentRayPos;
        glm::bvec3 mask = glm::bvec3(false);
        bool hit = false;

        for (int i = 0; i < maxRaySteps; ++i) {
            if (getVoxel(mapPos)) {
                hit = true;
                std::cout << "Hit at: " << mapPos.x << ", " << mapPos.y << ", " << mapPos.z << std::endl; 
                break;
            }
      
            if (sideDist.x < sideDist.y) {
                if (sideDist.x < sideDist.z) {
                    totalDist = sideDist.x;
                    sideDist.x += deltaDist.x;
                    mapPos.x += rayStep.x;
                    mask = glm::bvec3(true, false, false);
                }
                else {
                    totalDist = sideDist.z;
                    sideDist.z += deltaDist.z;
                    mapPos.z += rayStep.z;
                    mask = glm::bvec3(false, false, true);
                }
            }
            else {
                if (sideDist.y < sideDist.z) {
                    totalDist = sideDist.y;
                    sideDist.y += deltaDist.y;
                    mapPos.y += rayStep.y;
                    mask = glm::bvec3(false, true, false);
                }
                else {
                    totalDist = sideDist.z;
                    sideDist.z += deltaDist.z;
                    mapPos.z += rayStep.z;
                    mask = glm::bvec3(false, false, true);
                }
            }
        }

        glm::vec3 color = glm::vec3(0.0f);  // Background color if no hit

        
        if (hit) {
            currentRayPos = rayPos + (rayDir * totalDist);  // Correct calculation of currentRayPos
            
            glm::vec3 fractCurrentRayPos = glm::fract(currentRayPos);
            // Print the required values for debugging
            std::cout << "----------New Hit------------" << std::endl;
            std::cout << "Fragment: (" << fragCoord.x << ", " << fragCoord.y << ", " << ")\n";
            std::cout << "Ray Direction: (" << rayDir.x << ", " << rayDir.y << ", " << rayDir.z << ")\n";
            std::cout << "Ray Position: (" << rayPos.x << ", " << rayPos.y << ", " << rayPos.z << ")\n";
            std::cout << "Side Distance: " << sideDist.x << ", " << sideDist.y << ", " << sideDist.z << "\n";
            std::cout << "Total Distance: " << totalDist << "\n";
            std::cout << "Current Ray Position: (" << currentRayPos.x << ", " << currentRayPos.y << ", " << currentRayPos.z << ")\n";
            std::cout << "Fractional Current Ray Position: (" << fractCurrentRayPos.x << ", " << fractCurrentRayPos.y << ", " << fractCurrentRayPos.z << ")\n";
            std::cout << "----------End Hit----------\n\n" << std::endl;
            /*
            if (mask.x) {
                // Sample the texture with the fractional part of the hit position
                color = textureSample(glm::fract(glm::vec2(currentRayPos.z, currentRayPos.y)));
            }
            else if (mask.y) {
                color = textureSample(glm::fract(glm::vec2(currentRayPos.x, currentRayPos.z)));
            }
            else if (mask.z) {
                color = textureSample(glm::fract(glm::vec2(currentRayPos.x, currentRayPos.y)));
            }
            */
        }

        //return glm::vec4(color, 1.0f);
        
    }

    // cast all rays
    void render() {
        for (int y = 0; y < screenHeight; ++y) {
            for (int x = 0; x < screenWidth; ++x) {
                castRay(x, y);
            }
        }
    }

private:
    float screenWidth;
    float screenHeight;
    int maxRaySteps;
    glm::vec3 camPos;
    glm::vec3 camDir;
    glm::vec3 camPlaneU;
    glm::vec3 camPlaneV;
};

