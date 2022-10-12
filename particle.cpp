#include "particle.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "obj_mesh.h"

particle::particle(glm::mat4* trans, GLuint* normalTransformLoc, GLuint* modelTransformLoc, ObjData object) {
	*trans = glm::mat4(1.0f);
	*trans = glm::translate(*trans, glm::vec3(0.0f,0.0f,0.0f));
	//*trans = glm::rotate(*trans, glm::radians(0.0f), glm::vec3(0.0f,0.0f,0.0f));
	*trans = glm::scale(*trans, glm::vec3(1.0f,1.0f,1.0f));

	glm::mat4 normalTrans = glm::transpose(glm::inverse(*trans));
	glUniformMatrix4fv(*normalTransformLoc, 1, GL_FALSE, glm::value_ptr(normalTrans));
	normalTrans = glm::transpose(glm::inverse(*trans));
	glUniformMatrix4fv(*normalTransformLoc, 1, GL_FALSE, glm::value_ptr(normalTrans));
	glUniformMatrix4fv(*modelTransformLoc, 1, GL_FALSE, glm::value_ptr(*trans));
	glActiveTexture(GL_TEXTURE0);
	GLuint objTexture = object.textures[object.materials[0].diffuse_texname];
	glBindTexture(GL_TEXTURE_2D, objTexture);

	glDrawElements(GL_TRIANGLES, object.numFaces, GL_UNSIGNED_INT, (void*)0);

	//unbindtexture after rendering
	glBindTexture(GL_TEXTURE_2D, 0);
}
