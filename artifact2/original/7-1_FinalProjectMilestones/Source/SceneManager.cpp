///////////////////////////////////////////////////////////////////////////////
// scenemanager.cpp
// ============
// manage the preparing and rendering of 3D scenes - textures, materials, lighting
//
//  AUTHOR: Brian Battersby - SNHU Instructor / Computer Science
//	Created for CS-330-Computational Graphics and Visualization, Nov. 1st, 2023
///////////////////////////////////////////////////////////////////////////////

#include "SceneManager.h"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif

#include <glm/gtx/transform.hpp>

// declaration of global variables
namespace
{
	const char* g_ModelName = "model";
	const char* g_ColorValueName = "objectColor";
	const char* g_TextureValueName = "objectTexture";
	const char* g_UseTextureName = "bUseTexture";
	const char* g_UseLightingName = "bUseLighting";
}

/***********************************************************
 *  SceneManager()
 *
 *  The constructor for the class
 ***********************************************************/
SceneManager::SceneManager(ShaderManager *pShaderManager)
{
	m_pShaderManager = pShaderManager;
	m_basicMeshes = new ShapeMeshes();
}

/***********************************************************
 *  ~SceneManager()
 *
 *  The destructor for the class
 ***********************************************************/
SceneManager::~SceneManager()
{
	m_pShaderManager = NULL;
	delete m_basicMeshes;
	m_basicMeshes = NULL;
}

/***********************************************************
 *  CreateGLTexture()
 *
 *  This method is used for loading textures from image files,
 *  configuring the texture mapping parameters in OpenGL,
 *  generating the mipmaps, and loading the read texture into
 *  the next available texture slot in memory.
 ***********************************************************/
bool SceneManager::CreateGLTexture(const char* filename, std::string tag)
{
	int width = 0;
	int height = 0;
	int colorChannels = 0;
	GLuint textureID = 0;

	// indicate to always flip images vertically when loaded
	stbi_set_flip_vertically_on_load(true);

	// try to parse the image data from the specified image file
	unsigned char* image = stbi_load(
		filename,
		&width,
		&height,
		&colorChannels,
		0);

	// if the image was successfully read from the image file
	if (image)
	{
		std::cout << "Successfully loaded image:" << filename << ", width:" << width << ", height:" << height << ", channels:" << colorChannels << std::endl;

		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);

		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// if the loaded image is in RGB format
		if (colorChannels == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		// if the loaded image is in RGBA format - it supports transparency
		else if (colorChannels == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		else
		{
			std::cout << "Not implemented to handle image with " << colorChannels << " channels" << std::endl;
			return false;
		}

		// generate the texture mipmaps for mapping textures to lower resolutions
		glGenerateMipmap(GL_TEXTURE_2D);

		// free the image data from local memory
		stbi_image_free(image);
		glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

		// register the loaded texture and associate it with the special tag string
		m_textureIDs[m_loadedTextures].ID = textureID;
		m_textureIDs[m_loadedTextures].tag = tag;
		m_loadedTextures++;

		return true;
	}

	std::cout << "Could not load image:" << filename << std::endl;

	// Error loading the image
	return false;
}

/***********************************************************
 *  BindGLTextures()
 *
 *  This method is used for binding the loaded textures to
 *  OpenGL texture memory slots.  There are up to 16 slots.
 ***********************************************************/
void SceneManager::BindGLTextures()
{
	for (int i = 0; i < m_loadedTextures; i++)
	{
		// bind textures on corresponding texture units
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, m_textureIDs[i].ID);
	}
}

/***********************************************************
 *  DestroyGLTextures()
 *
 *  This method is used for freeing the memory in all the
 *  used texture memory slots.
 ***********************************************************/
void SceneManager::DestroyGLTextures()
{
	for (int i = 0; i < m_loadedTextures; i++)
	{
		glGenTextures(1, &m_textureIDs[i].ID);
	}
}

/***********************************************************
 *  FindTextureID()
 *
 *  This method is used for getting an ID for the previously
 *  loaded texture bitmap associated with the passed in tag.
 ***********************************************************/
int SceneManager::FindTextureID(std::string tag)
{
	int textureID = -1;
	int index = 0;
	bool bFound = false;

	while ((index < m_loadedTextures) && (bFound == false))
	{
		if (m_textureIDs[index].tag.compare(tag) == 0)
		{
			textureID = m_textureIDs[index].ID;
			bFound = true;
		}
		else
			index++;
	}

	return(textureID);
}

/***********************************************************
 *  FindTextureSlot()
 *
 *  This method is used for getting a slot index for the previously
 *  loaded texture bitmap associated with the passed in tag.
 ***********************************************************/
int SceneManager::FindTextureSlot(std::string tag)
{
	int textureSlot = -1;
	int index = 0;
	bool bFound = false;

	while ((index < m_loadedTextures) && (bFound == false))
	{
		if (m_textureIDs[index].tag.compare(tag) == 0)
		{
			textureSlot = index;
			bFound = true;
		}
		else
			index++;
	}

	return(textureSlot);
}

/***********************************************************
 *  FindMaterial()
 *
 *  This method is used for getting a material from the previously
 *  defined materials list that is associated with the passed in tag.
 ***********************************************************/
bool SceneManager::FindMaterial(std::string tag, OBJECT_MATERIAL& material)
{
	if (m_objectMaterials.size() == 0)
	{
		return(false);
	}

	int index = 0;
	bool bFound = false;
	while ((index < m_objectMaterials.size()) && (bFound == false))
	{
		if (m_objectMaterials[index].tag.compare(tag) == 0)
		{
			bFound = true;
			material.ambientColor = m_objectMaterials[index].ambientColor;
			material.ambientStrength = m_objectMaterials[index].ambientStrength;
			material.diffuseColor = m_objectMaterials[index].diffuseColor;
			material.specularColor = m_objectMaterials[index].specularColor;
			material.shininess = m_objectMaterials[index].shininess;
		}
		else
		{
			index++;
		}
	}

	return(true);
}



/***********************************************************
 *  SetTransformations()
 *
 *  This method is used for setting the transform buffer
 *  using the passed in transformation values.
 ***********************************************************/
void SceneManager::SetTransformations(
	glm::vec3 scaleXYZ,
	float XrotationDegrees,
	float YrotationDegrees,
	float ZrotationDegrees,
	glm::vec3 positionXYZ)
{
	// variables for this method
	glm::mat4 modelView;
	glm::mat4 scale;
	glm::mat4 rotationX;
	glm::mat4 rotationY;
	glm::mat4 rotationZ;
	glm::mat4 translation;

	// set the scale value in the transform buffer
	scale = glm::scale(scaleXYZ);
	// set the rotation values in the transform buffer
	rotationX = glm::rotate(glm::radians(XrotationDegrees), glm::vec3(1.0f, 0.0f, 0.0f));
	rotationY = glm::rotate(glm::radians(YrotationDegrees), glm::vec3(0.0f, 1.0f, 0.0f));
	rotationZ = glm::rotate(glm::radians(ZrotationDegrees), glm::vec3(0.0f, 0.0f, 1.0f));
	// set the translation value in the transform buffer
	translation = glm::translate(positionXYZ);

	modelView = translation * rotationZ * rotationY * rotationX * scale;

	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setMat4Value(g_ModelName, modelView);
	}
}

/***********************************************************
 *  SetShaderColor()
 *
 *  This method is used for setting the passed in color
 *  into the shader for the next draw command
 ***********************************************************/
void SceneManager::SetShaderColor(
	float redColorValue,
	float greenColorValue,
	float blueColorValue,
	float alphaValue)
{
	// variables for this method
	glm::vec4 currentColor;

	currentColor.r = redColorValue;
	currentColor.g = greenColorValue;
	currentColor.b = blueColorValue;
	currentColor.a = alphaValue;

	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setIntValue(g_UseTextureName, false);
		m_pShaderManager->setVec4Value(g_ColorValueName, currentColor);
	}
}

/***********************************************************
 *  SetShaderMaterial()
 *
 *  This method is used for passing the material values
 *  into the shader.
 ***********************************************************/
void SceneManager::SetShaderMaterial(
	std::string materialTag)
{
	if (m_objectMaterials.size() > 0)
	{
		OBJECT_MATERIAL material;
		bool bReturn = false;

		// find the defined material that matches the tag
		bReturn = FindMaterial(materialTag, material);
		if (bReturn == true)
		{
			// pass the material properties into the shader
			m_pShaderManager->setVec3Value("material.ambientColor", material.ambientColor);
			m_pShaderManager->setFloatValue("material.ambientStrength", material.ambientStrength);
			m_pShaderManager->setVec3Value("material.diffuseColor", material.diffuseColor);
			m_pShaderManager->setVec3Value("material.specularColor", material.specularColor);
			m_pShaderManager->setFloatValue("material.shininess", material.shininess);
		}
	}
}

/***********************************************************
 *  SetShaderTexture()
 *
 *  This method is used for setting the texture data
 *  associated with the passed in ID into the shader.
 ***********************************************************/
void SceneManager::SetShaderTexture(
	std::string textureTag)
{
	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setIntValue(g_UseTextureName, true);

		int textureID = -1;
		textureID = FindTextureSlot(textureTag);
		m_pShaderManager->setSampler2DValue(g_TextureValueName, textureID);
	}
}

/***********************************************************
 *  SetTextureUVScale()
 *
 *  This method is used for setting the texture UV scale
 *  values into the shader.
 ***********************************************************/
void SceneManager::SetTextureUVScale(float u, float v)
{
	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setVec2Value("UVscale", glm::vec2(u, v));
	}
}




/**************************************************************/
/*** STUDENTS CAN MODIFY the code in the methods BELOW for  ***/
/*** preparing and rendering their own 3D replicated scenes.***/
/*** Please refer to the code in the OpenGL sample project  ***/
/*** for assistance.                                        ***/
/**************************************************************/


void SceneManager::LoadSceneTextures()
{
	bool bReturn = false;


	bReturn = CreateGLTexture(
		"textures\\marble_texture.jpg",
		"marble");

	bReturn = CreateGLTexture(
		"textures\\stone_texture.jpg",
		"stone");

	bReturn = CreateGLTexture(
		"textures\\gold_texture.jpg",
		"gold");

	bReturn = CreateGLTexture(
		"textures\\grass.jpg",
		"grass");

	

	BindGLTextures();

}

/***********************************************************
 *  DefineObjectMaterials()
 *
 *  This method is used for configuring the various material
 *  settings for all of the objects within the 3D scene.
 ***********************************************************/
void SceneManager::DefineObjectMaterials()
{
	/*** STUDENTS - add the code BELOW for defining object materials. ***/
	/*** There is no limit to the number of object materials that can ***/
	/*** be defined. Refer to the code in the OpenGL Sample for help  ***/

	OBJECT_MATERIAL goldMaterial;
	goldMaterial.ambientColor = glm::vec3(0.2f, 0.2f, 0.1f);
	goldMaterial.ambientStrength = 0.4f;
	goldMaterial.diffuseColor = glm::vec3(1.0f, 1.0f, 1.0f);
	goldMaterial.specularColor = glm::vec3(0.6f, 0.5f, 0.4f);
	goldMaterial.shininess = 22.0;
	goldMaterial.tag = "gold";
	m_objectMaterials.push_back(goldMaterial);


	OBJECT_MATERIAL basicMaterial;
	basicMaterial.ambientColor = glm::vec3(0.2f, 0.2f, 0.2f);
	basicMaterial.ambientStrength = 0.3f;
	basicMaterial.diffuseColor = glm::vec3(0.7f, 0.7f, 0.7f);
	basicMaterial.specularColor = glm::vec3(0.1f, 0.1f, 0.1f);
	basicMaterial.shininess = 0.5;
	basicMaterial.tag = "basic";
	m_objectMaterials.push_back(basicMaterial);


}



/***********************************************************
 *  SetupSceneLights()
 *
 *  This method is called to add and configure the light
 *  sources for the 3D scene.  There are up to 4 light sources.
 ***********************************************************/
void SceneManager::SetupSceneLights()
{
	// this line of code is NEEDED for telling the shaders to render 
	// the 3D scene with custom lighting, if no light sources have
	// been added then the display window will be black - to use the 
	// default OpenGL lighting then comment out the following line
	//m_pShaderManager->setBoolValue(g_UseLightingName, true);

	/*** STUDENTS - add the code BELOW for setting up light sources ***/
	/*** Up to four light sources can be defined. Refer to the code ***/
	/*** in the OpenGL Sample for help                              ***/
	m_pShaderManager->setVec3Value("lightSources[0].position", -200.0f, 1000.0f, -200.0f);
	m_pShaderManager->setVec3Value("lightSources[0].ambientColor", 0.2f, 0.2f, 0.2f);
	m_pShaderManager->setVec3Value("lightSources[0].diffuseColor", 1.0f, 1.0f, 1.0f);
	m_pShaderManager->setVec3Value("lightSources[0].specularColor", 0.1f, 0.1f, 0.1f);
	m_pShaderManager->setFloatValue("lightSources[0].focalStrength", 4.0f);
	m_pShaderManager->setFloatValue("lightSources[0].specularIntensity", 0.5f);

	m_pShaderManager->setVec3Value("lightSources[1].position", 200.0f, 1000.0f, 200.0f);
	m_pShaderManager->setVec3Value("lightSources[1].ambientColor", 0.1f, 0.1f, 0.1f);
	m_pShaderManager->setVec3Value("lightSources[1].diffuseColor", 0.2f, 0.2f, 0.2f);
	m_pShaderManager->setVec3Value("lightSources[1].specularColor", 0.0f, 0.0f, 0.0f);
	m_pShaderManager->setFloatValue("lightSources[1].focalStrength", 32.0f);
	m_pShaderManager->setFloatValue("lightSources[1].specularIntensity", 0.0f);

	m_pShaderManager->setVec3Value("lightSources[2].position", 0.0f, 0.0f, 0.0f);
	m_pShaderManager->setVec3Value("lightSources[2].ambientColor", 0.1f, 0.1f, 0.1f);
	m_pShaderManager->setVec3Value("lightSources[2].diffuseColor", 0.1f, 0.1f, 0.1f);
	m_pShaderManager->setVec3Value("lightSources[2].specularColor", 0.0f, 0.0f, 0.0f);
	m_pShaderManager->setFloatValue("lightSources[2].focalStrength", 2.0f);
	m_pShaderManager->setFloatValue("lightSources[2].specularIntensity", 0.0f);


	m_pShaderManager->setVec3Value("lightSources[3].position", 0.0f, 0.0f, 0.0f);
	m_pShaderManager->setVec3Value("lightSources[3].ambientColor", 0.0f, 0.0f, 0.0f);
	m_pShaderManager->setVec3Value("lightSources[3].diffuseColor", 0.0f, 0.0f, 0.0f);
	m_pShaderManager->setVec3Value("lightSources[3].specularColor", 0.0f, 0.0f, 0.0f);
	m_pShaderManager->setFloatValue("lightSources[3].focalStrength", 12.0f);
	m_pShaderManager->setFloatValue("lightSources[3].specularIntensity", 0.0f);


	m_pShaderManager->setBoolValue("bUseLighting", true);
	m_pShaderManager->setBoolValue("bUseTexture", true);


}




/***********************************************************
 *  PrepareScene()
 *
 *  This method is used for preparing the 3D scene by loading
 *  the shapes, textures in memory to support the 3D scene 
 *  rendering
 ***********************************************************/
void SceneManager::PrepareScene()
{
	// only one instance of a particular mesh needs to be
	// loaded in memory no matter how many times it is drawn
	// in the rendered 3D scene
	LoadSceneTextures();
	DefineObjectMaterials();
	SetupSceneLights();

	m_basicMeshes->LoadPlaneMesh();
	m_basicMeshes->LoadBoxMesh();
	m_basicMeshes->LoadConeMesh();
	m_basicMeshes->LoadCylinderMesh();
	m_basicMeshes->LoadPyramid4Mesh();
	m_basicMeshes->LoadSphereMesh();
}

/***********************************************************
 *  RenderScene()
 *
 *  This method is used for rendering the 3D scene by 
 *  transforming and drawing the basic 3D shapes
 ***********************************************************/
void SceneManager::RenderScene()
{

	// Will be rendering 2ND scene
	// declare the variables for the transformations
	glm::vec3 scaleXYZ;
	glm::vec3 rotationXYZ;
	glm::vec3 positionXYZ;
	glm::vec4 colorRGBA;

	float size = 3000; // size of the plane
	scaleXYZ = glm::vec3(size, 1.0f, size); // set the XYZ scale for the mesh
	rotationXYZ = glm::vec3(0.0f, 0.0f, 0.0f); // set the XYZ rotation for the mesh
	positionXYZ = glm::vec3(0.0f, 0.0f, -size/2); // set the XYZ position for the mesh
	colorRGBA = glm::vec4(1.0f, 1.0f, 1.0f, 1); // set the color values into the shader
	SetShaderMaterial("basic"); // set the material for plane
	drawShape(shape::PLANE, scaleXYZ, rotationXYZ, positionXYZ, "grass");

	
	renderWall(glm::vec3(0.0),glm::vec3(0.0), 0.0);
	renderWall(glm::vec3(-12.0, 0.0, 0.0), glm::vec3(-2.0,0.0, 4.25), -45.0);
	renderWall(glm::vec3(-20.0, 0.0, -8.0), glm::vec3(-6.5, 0.0, 5.0), -90.0);
	renderWall(glm::vec3(-20.0, 0.0, -19.0), glm::vec3(-11.0, 0.0, 4.25), -135.0);

	renderWall(glm::vec3(-11.0, 0.0, -28.0), glm::vec3(-12.0, 0.0, 0.0), -180.0);
	renderWall(glm::vec3(0.0, 0.0, -28.0), glm::vec3(-11.0, 0.0, -4.25), -225.0);
	renderWall(glm::vec3(8.0, 0.0, -20.0), glm::vec3(-6.5, 0.0, -5.25), -270.0);
	renderWall(glm::vec3(8.0, 0.0, -9.0), glm::vec3(-2.5, 0.0, -4.25), -315.0);
}

void SceneManager::renderWall(glm::vec3 posTranslate, glm::vec3 offset, float angle){
	glm::vec3 scaleXYZ;
	glm::vec3 rotationXYZ;
	glm::vec3 positionXYZ;
	glm::vec4 colorRGBA;


	/*** Set needed transformations before drawing the basic mesh.  ***/
	/*** This same ordering of code should be used for transforming ***/
	/*** and drawing all the basic 3D shapes.						***/
	/******************************************************************/
	
	/****************************************************************/
	// Middle Wall/Gate
	scaleXYZ = glm::vec3(10.0f, 1.0f, 6.0f); // set the XYZ scale for the mesh
	rotationXYZ = glm::vec3(0.0, angle, 0.0) + glm::vec3(90.0f, 0.0f, 0.0f); // set the XYZ rotation for the mesh
	positionXYZ = -offset + posTranslate + glm::vec3(0.0f, 3.0f, 0.0f); // set the XYZ position for the mesh
	// colorRGBA = glm::vec4(0.929f, 0.875f, 0.773f, 1); // set the color values into the shader
	SetShaderMaterial("basic");
	drawShape(shape::CUBE, scaleXYZ, rotationXYZ, positionXYZ, "stone");  // set the texture of the castle wall
	/*****************************----LEFT TOWER--********************************

	// tower lower cylinder
	scaleXYZ = glm::vec3(2.0f, 1.5f, 2.0f); // set the XYZ scale for the mesh
	rotationXYZ = rotation + glm::vec3(0.0f, 0.0f, 0.0f); // set the XYZ rotation for the mesh
	positionXYZ = posTranslate + glm::vec3(-6.5f, 0.0f, 0.0f); // set the XYZ position for the mesh
	// colorRGBA = glm::vec4(0.829f, 0.775f, 0.673f, 1); // set the color values into the shader
	drawShape(shape::CYLINDER, scaleXYZ, rotationXYZ, positionXYZ, "marble");
	//***********************************************************
	// tower long cylinder
	scaleXYZ = glm::vec3(1.5f, 5.0f, 1.5f); // set the XYZ scale for the mesh
	rotationXYZ = rotation + glm::vec3(0.0f, 0.0f, 0.0f); // set the XYZ rotation for the mesh
	positionXYZ = posTranslate + glm::vec3(-6.5f, 1.5f, 0.0f); // set the XYZ position for the mesh
	//colorRGBA = glm::vec4(0.729f, 0.675f, 0.573f, 1); // set the color values into the shader
	drawShape(shape::CYLINDER, scaleXYZ, rotationXYZ, positionXYZ, "stone");
	//***********************************************************
	// tower top cylinder
	scaleXYZ = glm::vec3(2.0f, 2.0f, 2.0f); // set the XYZ scale for the mesh
	rotationXYZ = rotation + glm::vec3(0.0f, 0.0f, 0.0f); // set the XYZ rotation for the mesh
	positionXYZ = posTranslate + glm::vec3(-6.5f, 6.5f, 0.0f); // set the XYZ position for the mesh
	//colorRGBA = glm::vec4(0.629f, 0.575f, 0.473f, 1); // set the color values into the shader
	drawShape(shape::CYLINDER, scaleXYZ, rotationXYZ, positionXYZ, "marble");
	//***********************************************************
	// tower top cone
	scaleXYZ = glm::vec3(2.0f, 3.0f, 2.0f); // set the XYZ scale for the mesh
	rotationXYZ = rotation + glm::vec3(0.0f, 0.0f, 0.0f); // set the XYZ rotation for the mesh
	positionXYZ = posTranslate + glm::vec3(-6.5f, 8.5f, 0.0f); // set the XYZ position for the mesh
	//colorRGBA = glm::vec4(0.976f, 0.753f, 0.0f, 1); // set the color values into the shader
	drawShape(shape::CONE, scaleXYZ, rotationXYZ, positionXYZ, "gold");
	*************************************************************/
	/*****************************----RIGHT TOWER--***********************************/
	
	// tower lower cylinder
	scaleXYZ = glm::vec3(2.0f, 1.5f, 2.0f); // set the XYZ scale for the mesh
	rotationXYZ = glm::vec3(0.0f, 0.0f, 0.0f); // set the XYZ rotation for the mesh
	positionXYZ = posTranslate + glm::vec3(6.5f, 0.0f, 0.0f); // set the XYZ position for the mesh
	SetShaderMaterial("basic");
	//colorRGBA = glm::vec4(0.829f, 0.775f, 0.673f, 1); // set the color values into the shader
	drawShape(shape::CYLINDER, scaleXYZ, rotationXYZ, positionXYZ, "marble");
	/****************************************************************/
	// tower long cylinder
	scaleXYZ = glm::vec3(1.5f, 5.0f, 1.5f); // set the XYZ scale for the mesh
	rotationXYZ = glm::vec3(0.0f, 0.0f, 0.0f); // set the XYZ rotation for the mesh
	positionXYZ = posTranslate + glm::vec3(6.5f, 1.5f, 0.0f); // set the XYZ position for the mesh
	//colorRGBA = glm::vec4(0.729f, 0.675f, 0.573f, 1); // set the color values into the shader
	SetShaderMaterial("basic");
	drawShape(shape::CYLINDER, scaleXYZ, rotationXYZ, positionXYZ, "stone");
	/****************************************************************/
	// tower top cylinder
	scaleXYZ = glm::vec3(2.0f, 2.0f, 2.0f); // set the XYZ scale for the mesh
	rotationXYZ = glm::vec3(0.0f, 0.0f, 0.0f); // set the XYZ rotation for the mesh
	positionXYZ = posTranslate + glm::vec3(6.5f, 6.5f, 0.0f); // set the XYZ position for the mesh
	//colorRGBA = glm::vec4(0.629f, 0.575f, 0.473f, 1); // set the color values into the shader
	SetShaderMaterial("basic");
	drawShape(shape::CYLINDER, scaleXYZ, rotationXYZ, positionXYZ, "marble");
	/****************************************************************/
	// tower top cone
	scaleXYZ = glm::vec3(2.0f, 3.0f, 2.0f); // set the XYZ scale for the mesh
	rotationXYZ = glm::vec3(0.0f, 0.0f, 0.0f); // set the XYZ rotation for the mesh
	positionXYZ = posTranslate + glm::vec3(6.5f, 8.5f, 0.0f); // set the XYZ position for the mesh
	//colorRGBA = glm::vec4(0.976f, 0.753f, 0.0f, 1); // set the color values into the shader
	SetShaderMaterial("gold");
	drawShape(shape::CONE, scaleXYZ, rotationXYZ, positionXYZ, "gold");




}


/**
* Uses enum in header class to decide which shape needs to be drawn
* one locations for drawing the meshd
* takes in transformations needed to translate local to world space as well as other two
* transformations
**/
void SceneManager::drawShape(shape shape, glm::vec3 scale, glm::vec3 rotation, glm::vec3 posTrans, glm::vec4 color) {
	SetTransformations(scale, rotation.x, rotation.y, rotation.z, posTrans); // set the transformations into memory to be used on the drawn meshes
	SetShaderColor(color.r, color.g, color.b, color.a); // change the color 

	// Draw the respective shape; 
	switch (shape) {
	case CUBE:
		m_basicMeshes->DrawBoxMesh();
		break;
	case CONE:
		m_basicMeshes->DrawConeMesh();
		break;
	case CYLINDER:
		m_basicMeshes->DrawCylinderMesh();
		break;
	case PLANE:
		m_basicMeshes->DrawPlaneMesh();
		break;
	case PYRAMID4:
		m_basicMeshes->DrawPyramid4Mesh();
		break;
	case SPHERE:
		m_basicMeshes->DrawSphereMesh();
		break;
	}
}



/**
* Uses enum in header class to decide which shape needs to be drawn
* one locations for drawing the meshd
* takes in transformations needed to translate local to world space as well as other two
* transformations
**/
void SceneManager::drawShape(shape shape, glm::vec3 scale, glm::vec3 rotation, glm::vec3 posTrans, std::string textureName) {
	SetTransformations(scale, rotation.x, rotation.y, rotation.z, posTrans); // set the transformations into memory to be used on the drawn meshes
	SetShaderTexture(textureName); // change the color 

	// Draw the respective shape; 
	switch (shape) {
	case CUBE:
		m_basicMeshes->DrawBoxMesh();
		break;
	case CONE:
		m_basicMeshes->DrawConeMesh();
		break;
	case CYLINDER:
		m_basicMeshes->DrawCylinderMesh();
		break;
	case PLANE:
		m_basicMeshes->DrawPlaneMesh();
		break;
	case PYRAMID4:
		m_basicMeshes->DrawPyramid4Mesh();
		break;
	case SPHERE:
		m_basicMeshes->DrawSphereMesh();
		break;
	}
}
