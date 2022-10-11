#pragma once
#ifndef OBJDATA_H
#define OBJDATA_H

#include <iostream>
#include <vector>
#include <map>

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

#endif /* OBJDATA_H */
