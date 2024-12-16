#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "shader.h"
#include "Camera.h"
#include "FileManager.h"


/*
* Used to define a directional light source, 
* Represented as a plane that rotates around camera at a fixed distance
* Cannot get closer to it
*/
class DirectionalLight {


public: 

    //NOTE!: You declare a type of this class before glfw and opengl context is made
    DirectionalLight() {
        glGenVertexArrays(1, &lightVAO);
        glBindVertexArray(lightVAO);

        // Generate and bind the VBO
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindVertexArray(lightVAO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))); // texture coordinates of plane
        glEnableVertexAttribArray(1);

        shader = new Shader(vertShPath, fragShPath, false);
        shader->use();
        shader->setVec3("lightColor", lightColor);

        setTexture(); // TODO Directional light texture not Working
        glActiveTexture(GL_TEXTURE4);
        shader->setInt("sunTexture", 4); // set to tenth texel unit of shader
    }

    ~DirectionalLight() {
        // Clean up resources
        glDeleteVertexArrays(1, &lightVAO);
        glDeleteBuffers(1, &VBO);
        delete shader;
    }

    /**
    *
    * Draw the directional light as a cube currently
    **/
    void render(glm::mat4 projection, glm::mat4 view) {
        shader->use();

        shader->setMat4("projection", projection);
        glm::mat4 viewAdj = glm::mat4(glm::mat3(view)); // convert the matrix into a mat3 to remove the translation property
        shader->setMat4("view", viewAdj);
        // get the perpendicualr vectors to define a rotation that keeps plane facing camera
        // mainly to rotate plane to lightDir will also remaing perpendicular to each othe
        // not clear on exaclty how it works
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 right = glm::normalize(glm::cross(up, lightDir));
        up = glm::normalize(glm::cross(lightDir, right));
        glm::mat4 rotation = glm::mat4(1.0f);
        rotation[0] = glm::vec4(right, 0.0f);
        rotation[1] = glm::vec4(up, 0.0f);
        rotation[2] = glm::vec4(lightDir, 0.0f);

        
        glm::mat4 model = glm::mat4(1.0f);
        model = model * rotation; // rotate model matrix of plane
        shader->setMat4("model", model);


        glBindVertexArray(lightVAO);
        glActiveTexture(GL_TEXTURE4); // TODO Directional Light Texture not working
        glBindTexture(GL_TEXTURE_2D, textureID);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        
    }

    // TODO, issue occurs if direction has z direction equal to zero
    void setLightDir(glm::vec3 direction) {
        // normalize, as distance should not contribute to vector, 
        // also shader may use direction for lighting, if magnitude is un-normalized issues will occur
        direction = glm::normalize(direction); 
        lightDir = direction; 
    }

    void move() {

    }

    glm::vec3 getLightDir() {
        return lightDir; 
    }

    void rotate() {

        float time = glfwGetTime();  // Get the current time

        // Update the y and z coordinates to create a circular motion
        lightDir.y = sin(speed * (time + timeOffset));
        lightDir.z = cos(speed * (time + timeOffset));

        // Normalize lightDir to ensure it is a unit vector
        lightDir = glm::normalize(lightDir);
        // std::cout << "Light Dir: < " << lightDir.x << ", " << lightDir.y << ", " << lightDir.z << " >";
    }


    /*TODO set based on light direction */
    glm::vec3 getLightColor(float time) {
        float state = sin(speed * (time + timeOffset));
        return lightColor * std::max(state, 0.3f); // not zero to keep some ambient
    }

    // ?? Doesn't work, retarded
    glm::vec3 getLightColor() {
        return lightColor * 0.5f;// *0.3f;
    }

    



private:
    float speed = 0.2; 
    float timeOffset = 0.0; 
    float static inline distance = 50.0f; // how far the plane will appear
    glm::vec3 lightColor = glm::vec3(1.0, 1.0, 1.0); // bright white light 
    // settings
    const char* vertShPath = "shaders\\light\\lightVertFixedDist.vs";
    const char* fragShPath = "shaders\\light\\lightFragFixedDist.fs";
    const char* texturePath = "textures\\sun.png";
    Shader* shader = nullptr;
    unsigned int textureID;
    // light's VAO 
    unsigned int VBO, lightVAO;
    // lighting
    static inline glm::vec3 lightDir = glm::vec3(0.0, 0.0f, 1.0f); // forward plane
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float static inline vertices[] = {
        // vertices             //TexCoords
        -1.0f, -1.0f, distance,   0.0f,  0.0f, 
         1.0f, -1.0f, distance,   1.0f,  0.0f, 
         1.0f,  1.0f, distance,   1.0f,  1.0f,
         1.0f,  1.0f, distance,   1.0f,  1.0f,
        -1.0f,  1.0f, distance,   0.0f,  1.0f,
        -1.0f, -1.0f, distance,   0.0f,  0.0f
    };


    /* Note working */
    void setTexture() {
        
        glGenTextures(1, &textureID);
        glActiveTexture(GL_TEXTURE4); 
        glBindTexture(GL_TEXTURE_2D, textureID);

        int width, height, nrChannels;
        unsigned char* data = FileManager::loadData(texturePath, &width, &height, &nrChannels, false);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            //stbi_image_free(data);
        }
        else
        {
            std::cout << "Texture failed to load at path: " << texturePath << std::endl;
            //stbi_image_free(data);
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    }




};