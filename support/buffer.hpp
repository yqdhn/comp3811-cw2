#pragma once
#include<glad.h>
#include<cstdint>
#include"error.hpp"
#include"vao.hpp"

/*
* A simple wrapper on top of an OpenGL buffer.
*/
class Buffer {
private:
	GLuint bufferID;

public:
	Buffer();//TODO should there be a default constructor - can we always use the other one which requires the size to be known at construction time?

	//Input:
	// - bufferSize: the size of the buffer to be allocated. Cannot be changed later!
	// - data: a pointer to the data to be copied to the buffer. nullptr if currently not available (call setData later).
	Buffer(uintptr_t bufferSize, const void* data);

	//Utility function. Mp need to call it explicitly unless default constructor was used.
	void init(uintptr_t bufferSize, const void* data);

	//Set data for (a part of) the buffer.
	//Input:
	// - offset: the offset in the buffer to the first element that should be written over;
	// - dataSize: the number of bytes to be written over;
	// - data: a pointer to the data to be copied over.
	void setData(intptr_t offset, uintptr_t dataSize, const void* data);

	//Bind the buffer to a uniform at a given binding point.
	//Input:
	// - index: the uniform binding point.
	// - offset: offset within the buffer where the memory to be bound begins (in case of binding only a subset of the buffer).
	// NOTE: offset must be multiple of GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, which on most platforms is 256;
	// - size: the number of bytes of the memory to be be bound.
	void bindToUniform(uint32_t index, intptr_t offset, uintptr_t size);

	//Bind the buffer to a vertex attribute at a given binding point.
	//Input:
	// - vao: the vertex array object holding attribute specification;
	// - bindingPoint: the binding point of the attribute. We often set it to simply be the attribute index.
	// - offset: offset within the buffer where the attribute memory begins.
	// - stride: number of bytes from the start, where the next attribute is stored. Allows interleaved attributes (simply use appropriate stride).
	void bindToAttrib(const VertexArrayObject& vao, uint32_t bindingPoint, intptr_t offset, uint32_t stride);

	//Bind the buffer as an element buffer holding faces (indexing into the vertex array).
	void bindAsElementBuf(const VertexArrayObject& vao);

	GLuint getBufferID() const;
	uintptr_t getSize() const;

	~Buffer();
};

inline Buffer::Buffer() :bufferID(0) {}

inline Buffer::Buffer(uintptr_t bufferSize, const void* data):bufferID(0) {
	init(bufferSize, data);
}

inline void Buffer::init(uintptr_t bufferSize, const void* data) {
	if (bufferID == 0) {
		if (bufferSize == 0) throw Error("Attempted creating buffer with size 0.\n");
		glCreateBuffers(1, &bufferID);
		glNamedBufferStorage(bufferID, static_cast<GLsizeiptr>(bufferSize), data, GL_DYNAMIC_STORAGE_BIT);
	}
}

inline void Buffer::setData(intptr_t offset, uintptr_t dataSize, const void* data) {
	if (offset + dataSize > getSize()) throw Error("Invalid attempt to set buffer data of buffer %u (offset + size would cause overflow)\n",bufferID);
	glNamedBufferSubData(bufferID, static_cast<GLintptr>(offset), static_cast<GLsizeiptr>(dataSize), data);
}

inline void Buffer::bindToUniform(uint32_t index, intptr_t offset, uintptr_t size) {
	int uboAlignment = 0;
	glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &uboAlignment);
	if (offset % static_cast<intptr_t>(uboAlignment) != 0) throw Error(
		"Invalid attempt to bind buffer %u to uniform bind point %u. Offset alignment must be %i.", bufferID, index, uboAlignment);
	glBindBufferRange(GL_UNIFORM_BUFFER, static_cast<GLuint>(index), bufferID, static_cast<GLintptr>(offset), static_cast<GLsizeiptr>(size));
}

inline void Buffer::bindToAttrib(const VertexArrayObject& vao, uint32_t bindingPoint, intptr_t offset, uint32_t stride) {
	glVertexArrayVertexBuffer(vao.getID(), static_cast<GLuint>(bindingPoint), bufferID, static_cast<GLintptr>(offset), static_cast<GLsizei>(stride));
}

inline void Buffer::bindAsElementBuf(const VertexArrayObject& vao) {
	glVertexArrayElementBuffer(vao.getID(), bufferID);
}

inline GLuint Buffer::getBufferID() const {
	return bufferID;
}

inline uintptr_t Buffer::getSize() const {
	if (bufferID == 0) return 0;
	GLint64 size;
	glGetNamedBufferParameteri64v(bufferID, GL_BUFFER_SIZE,&size);
	return static_cast<uintptr_t>(size);
}

inline Buffer::~Buffer() {
	if (bufferID != 0) {
		glDeleteBuffers(1, &bufferID);
	}
}


