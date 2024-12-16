#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include "headers\shader.h"
#include "headers\Camera.h"
#include "headers\Texture.h"
#include "headers\HeightMap.h"
#include "headers\DirectionalLight.h"
#include "headers\skybox.h"
#include "headers\Voxel\ChunkManager.h"
#include "raycastDebug.cpp"
#include "headers\FastNoiseLite.h"



void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void updateShaderCamInfo();
void debugRaycaster();
void SetWindowTitleWithFPS(GLFWwindow* window, float deltaTime, glm::vec3 position);

// settings
const char* vertShPath = "shaders\\vert.vs";
const char* fragShPath = "shaders\\fragChunkRender2.fs";
float SCR_WIDTH = 800.0;
float SCR_HEIGHT = 450.0;
Shader* ourShader = NULL; // you can name your shader files however you like


// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;



int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    // build and compile our shader program
    // ------------------------------------
    ourShader = new Shader(vertShPath, fragShPath, false); // you can name your shader files however you like
    //Shader ourShader;
    //ourShader = Shader();
    // Vertex data for a rectangle covering the entire screen (no texture coordinates)
    float vertices[] = {
        // positions      
        -1.0f,  1.0f, 0.0f, // top-left
        -1.0f, -1.0f, 0.0f, // bottom-left
         1.0f, -1.0f, 0.0f, // bottom-right
         1.0f, -1.0f, 0.0f,  // top-right
         1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f
    };
    unsigned int indices[] = {
        0, 1, 2, // first triangle
        0, 2, 3  // second triangle
    };

    // Create VAO, VBO, and EBO
    unsigned int VAO, VBO; //, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // glGenBuffers(1, &EBO);

    // Bind VAO
    glBindVertexArray(VAO);

    // Bind and set VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Bind and set EBO
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Unbind VBO
    //glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Unbind VAO
    //glBindVertexArray(0);
    // Create VAO, VBO, and EBO
    unsigned int VAO2, VBO2; //, EBO;
    glGenVertexArrays(1, &VAO2);
    glGenBuffers(1, &VBO2);

    // Define the texture dimensions and format
    int width = 28;  // Example width
    int height = 28; // Example height
    int depth = 28;  // Example depth
    

    

    ourShader->use(); // need to set/use shader to be able to change the uniform
    ourShader->setFloat("screenWidth", SCR_WIDTH);
    ourShader->setFloat("screenHeight", SCR_HEIGHT);

    //create map for noise texture
    // Texture text = Texture(10000, 10000);
    
    
    // HeightMap map = HeightMap("textures\\mountainRangeHeightMap.png");
    /*
    Texture text = Texture(*ourShader, "textures\\terrainHeightMap.png", Texture::DATATYPE::UN_BYTE);
    if (!text.isReady()) {
        return 0; 

    }
    
    text.printMax();
    // text.printTextureValues();
    */
    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);

    ChunkManager chunkManager = ChunkManager(10, 1, 10,camera.Position, &noise);
    chunkManager.bind3DTexture();
    ourShader->setInt("blocks", 6); // set the 3d texture
    ourShader->setFloat("textWidth", chunkManager.getblockCountX());
    ourShader->setFloat("textHeight", chunkManager.getblockCountY());
    ourShader->setFloat("textDepth", chunkManager.getblockCountZ());
    ourShader->setFloat("startX", chunkManager.startX);
    ourShader->setFloat("startY", chunkManager.startY);
    ourShader->setFloat("startZ", chunkManager.startZ);

    // !!! Method call cause issue, with uniform text.printTextureValues();
    //ourShader.setInt("gridSize", width);
    //ourShader.set3DTexture(width, height, depth);

    Texture grassTop = Texture(*ourShader, "textures\\grass_top.png", Texture::DATATYPE::UN_BYTE);
    ourShader->setInt(grassTop.getDefaultUniformName(), grassTop.getTextureIndex());


    Texture grass = Texture(*ourShader, "textures\\grass.png", Texture::DATATYPE::UN_BYTE);
    ourShader->setInt(grass.getDefaultUniformName(), grass.getTextureIndex()); 
    //ourShader->setFloat("stoneTextWidth", grass.getWidth());
    //ourShader->setFloat("stoneTextHeight", grass.getHeight());


    
    //ourShader->setFloat("stoneTextWidth", grassTop.getWidth());
    //ourShader->setFloat("stoneTextHeight", grassTop.getHeight());

    DirectionalLight light = DirectionalLight();
    Skybox skybox = Skybox();
    // debugRaycaster(); c++ cpu dummy calls to fragment shader

    
    

    light.setLightDir(glm::vec3(0.0, 0.9, 0.1)); // center of sky, mid day, 
    // render loop
    // -----------
    chunkManager.printDebug();
    while (!glfwWindowShouldClose(window))
    {

        
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        SetWindowTitleWithFPS(window, deltaTime, camera.Position);
        
        // input
        // -----
        processInput(window);
       // std::cout << camera.Position.z << std::endl;
        

        // render
        // ------
        //glm::vec3 clearColor = light.getSkyColor(currentFrame);
        glm::vec3 clearColor = glm::vec3(0.0,0.0,0.0);
        glClearColor(clearColor.x, clearColor.y, clearColor.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // draw light source first, so terrain will draw on top of it
        light.rotate();
        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 20000.0f);
        glm::mat4 view = camera.GetViewMatrix();
        // draw in order, skybox behind light, light behind terrain
        skybox.setLightDir(light.getLightDir());
        skybox.render(&view, &projection);
        light.render(projection, view);
        
        


        

        // render the triangle
        ourShader->use();
        ourShader->setFloat("iTime", glfwGetTime());
        ourShader->setVec3("lightDir", light.getLightDir());
        ourShader->setVec3("lightColor", light.getLightColor(currentFrame)); // adjusts light
        ourShader->setVec3("lightColor", light.getLightColor());

        
        if (chunkManager.updatePosition(camera.Position)) { // texture needs to be updated, as player moved to next chunk

            chunkManager.bind3DTexture();
            ourShader->setInt("blocks", 6); // set the 3d texture
            ourShader->setFloat("startX", chunkManager.startX);
            ourShader->setFloat("startY", chunkManager.startY);
            ourShader->setFloat("startZ", chunkManager.startZ);

        }
        
        

        ourShader->setVec3("LightColor", light.getLightColor());
        glBindVertexArray(VAO);
        // Draw rectangle
        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Unbind VAO

        //std::cout << "Position: " << camera.Position.x << ", " << camera.Position.y << ", " << camera.Position.z << std::endl; 

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    
    glfwTerminate();
    return 0;
}

// Function to set the window title with FPS
void SetWindowTitleWithFPS(GLFWwindow* window, float deltaTime, glm::vec3 position) {
    // Calculate FPS
    float fps = 1.0f / deltaTime;

    // Convert FPS to string
    std::ostringstream oss;
    oss.precision(2); // Set precision if needed
    oss << std::fixed << "FPS: " << fps << "  ";
    oss << std::fixed << "( Coord: " << position.x << ", " << position.y << "," << position.z << " )";
    // Set the window title
    std::string fpsString = oss.str();
    glfwSetWindowTitle(window, fpsString.c_str());
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.ProcessKeyboard(FORWARD, deltaTime);
        updateShaderCamInfo();
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
        updateShaderCamInfo();
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.ProcessKeyboard(LEFT, deltaTime);
        updateShaderCamInfo();
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.ProcessKeyboard(RIGHT, deltaTime);
        updateShaderCamInfo();
    }

    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
        ourShader->use();
        ourShader->setInt("texture1", 0); // set texture1 to 0 location first texture
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
    if (ourShader != NULL) {
        ourShader->use();
        ourShader->setFloat("screenWidth", SCR_WIDTH);
        ourShader->setFloat("screenHeight", SCR_HEIGHT);
        
    }
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
    // std::cout << camera.Position.z << std::endl;
    updateShaderCamInfo();
}


// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}


void updateShaderCamInfo() {
    // update shader info
    if (ourShader != NULL) {
        ourShader->use();
        ourShader->setVec3("camPos", camera.Position);
        ourShader->setVec3("camDir", camera.Front);
        ourShader->setVec3("camPlaneU", camera.Right);
        ourShader->setVec3("camPlaneV", camera.Up);
    }

}



/**
* C++ dummy version of fragment shader raycaster
* use this to test the evaluation of vector operations, and
* hit values. 
**/
void debugRaycaster() {
    std::cout << " ------------------------------- Debugging ------------------------------ " << std::endl;

    // Example usage
    float screenWidth = 200.0f;
    float screenHeight = 100.0f;
    int maxRaySteps = 100;
    glm::vec3 camPos = glm::vec3(0.0f, 0.0f, 50.0f);
    glm::vec3 camDir = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 camPlaneU = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 camPlaneV = glm::vec3(0.0f, 1.0f, 0.0f);

    Raycaster raycaster(screenWidth, screenHeight, maxRaySteps, camPos, camDir, camPlaneU, camPlaneV);

    // Simulate a fragment at (400, 300)
    raycaster.render(); 

    std::cout << " ------------------------------ End Debugging --------------------------- " << std::endl;
}