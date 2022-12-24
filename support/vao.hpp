#pragma once
#include<glad.h>
#include<cstdint>

/*
* A simple wrapper on top of an OpenGL vao.
*/
class VertexArrayObject {
private:
	GLuint vao;

public:
	VertexArrayObject();
	//Bind the VAO for rendering.
	void bind();
	//Unbind the VAO.
	void unbind();

	//Define a new float-type vertex attribute.
	//Input: 
	// - attribIdx: index of the attribute, as specified in the vertex shader;
	// - size: number of components (each being a float);
	// - relativeOffset: offset from the start of the attribute block in case of interleaved attributes (e.g. normal offset relative to the start of the vertex block)
	// - (optional) enabled: should the attribute be enabled on creation;
	// - (optional) bindSameAsAttrIdx: should the attribute be bound to the same binding point as its index (for convenience)?
	void addAttribF(uint32_t attribIdx, int32_t size, uint32_t relativeOffset, bool enabled = true, bool bindSameAsAttrIdx = true);

	//Bind attribute to a given binding point. Vertex buffers will then be bound to this binding point.
	//For convenience, we aim to always set the bindng point to be the same as the attribute index.
	void bindAttrib(uint32_t attribIdx, uint32_t bindingPointIdx);

	void enableAttrib(uint32_t attribIdx);
	void disableAttrib(uint32_t attribIdx);
	GLuint getID() const;
	~VertexArrayObject();
};

inline VertexArrayObject::VertexArrayObject() {
	glCreateVertexArrays(1, &vao);
}

inline void VertexArrayObject::bind() {
	glBindVertexArray(vao);
}

inline void VertexArrayObject::unbind() {
	GLint boundVAO = 0;
	glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &boundVAO);
	if (static_cast<GLuint>(boundVAO) == vao) {
		glBindVertexArray(0);
	}
}

inline void VertexArrayObject::addAttribF(uint32_t attribIdx, int32_t size, uint32_t relativeOffset, bool enabled, bool bindSameAsAttrIdx) {
	//As regards vertex attributes, there are two logically distinct operations:
	// - describe where to find the vertex data;
	// - describe what the data format is.
	//Modern OpenGL gives us independent control of these two operations, so we prefer this over glVertexAttribPointer.
	//A more thourough explanation can ve found here: 
	//https://www.khronos.org/opengl/wiki/Vertex_Specification#Separate_attribute_format
	//https://stackoverflow.com/questions/37972229/glvertexattribpointer-and-glvertexattribformat-whats-the-difference

	glVertexArrayAttribFormat(vao, static_cast<GLuint>(attribIdx), static_cast<GLint>(size), GL_FLOAT, GL_FALSE, static_cast<GLuint>(relativeOffset));
	if (enabled) enableAttrib(attribIdx);
	if (bindSameAsAttrIdx) bindAttrib(attribIdx, attribIdx);
}

inline void VertexArrayObject::bindAttrib(uint32_t attribIdx, uint32_t bindingPointIdx) {
	glVertexArrayAttribBinding(vao, attribIdx, bindingPointIdx);
}

inline void VertexArrayObject::enableAttrib(uint32_t attribIdx) {
	glEnableVertexArrayAttrib(vao, attribIdx);
}

inline void VertexArrayObject::disableAttrib(uint32_t attribIdx) {
	glDisableVertexArrayAttrib(vao, attribIdx);
}

inline GLuint VertexArrayObject::getID() const {
	return vao;
}

inline VertexArrayObject::~VertexArrayObject() {
	glDeleteVertexArrays(1, &vao);
}
