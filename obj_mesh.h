#pragma once
#ifndef OBJ_MESH_H
#define OBJ_MESH_H

#include "stb_image.h"
#ifdef _WIN32
// don't include anything
#else
#include <sys/stat.h>
#endif
#include <iostream>
#include <vector>
#include <map>
#include <GL/glew.h>
#include <glm/glm.hpp>
//#define TINYOBJLOADER_IMPLEMENTATION
#ifndef TINY_OBJ_LOADER_H_
#include "tiny_obj_loader.h"
#endif

struct VertexData {
	glm::vec3 position;
	glm::vec2 uv;
	glm::vec3 normal;
};

// data class for obj data
struct ObjData {
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	GLulong numFaces;
	GLuint vaoId;

	std::string baseDir;
	std::vector<tinyobj::material_t> materials;
	std::map<std::string, GLuint> textures;
};

class obj_mesh{
	public:
		std::string GetBaseDir(const std::string& filepath);
		bool FileExists(const std::string& absFilename);
		void LoadTextureData(ObjData* objData);
		void LoadObjFile(ObjData* objData, std::string filename);
		void LoadObjToMemory(ObjData* objData, GLfloat scaleFactor, GLfloat tOffset[]);
};

#endif

