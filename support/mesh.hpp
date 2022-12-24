#pragma once
#include"buffer.hpp"
#include"vao.hpp"
#include"material.hpp"
#include"../vmlib/vec2.hpp"
#include"../vmlib/vec3.hpp"
#include"../vmlib/mat44.hpp"
#include<vector>
#include"texture.hpp"

class State;

constexpr const int ATTRIB_LOCATION_VERT_POS = 0;
constexpr const int ATTRIB_LOCATION_VERT_NORMAL = 1;
constexpr const int ATTRIB_LOCATION_VERT_UV = 2;
constexpr const int ATTRIB_LOCATION_VERT_TANGENT = 3;

//A vertex definition. We always store vertices as if all three components (pos, normals, uvs) are present.
//It may be possible that some of these are missing. 
//  - If normals are missing, an exception is thrown since normals
//are required for lighting (an alternative approach not implemented here would be to compute normals from the geometry).
//  - If tex coords are missing, a value of (0.0,0.0) is written. In these cases a default 1x1 white texture will be loaded and
//the mesh will read from that texture instead.
struct Vertex {
	Vec3f pos;
	Vec3f normal;
	Vec2f texCoords;
	Vec3f tangent;
	float handedness;
};

struct MeshUniforms {
	const Mat44f* modelMat;
	const Mat44f* modelMatN;
	const Mat44f* viewProjMat;
};

/*
* A mesh class storing vertex array and element buffers and associated materials.
* A mesh is composed of a list of vertices (position, normal, and uv) and a list of "face groups".
* We define a "face groups" as a list of triangles (indexing the vertex array) and an associated material for this group.
* This allows us to render multi-material meshes that have different materials for their faces.
* The number of face group is equal to the number of unique materials in the mesh.
*/
class Mesh {
public://TODO make private again
	struct MaterialFaceGroupInternal {
		Material mat;//material shared by the faces in the group
		Buffer veb;//vertex element buffer - an index buffer, indexing the vertex array
		size_t numIndices;
		//MaterialFaceGroupInternal(const std::vector<unsigned int>& indices, const Material& material);
		MaterialFaceGroupInternal(unsigned int* indices, size_t numIndices, const Material& material);
	};

	Buffer vbo;//list of vertices - contains positions, normals, and tex coords
	std::vector<MaterialFaceGroupInternal> faceGroups;
	bool hasUVs;

public:
	//Input:
	// - vertices: a list of vertices (unique position, normal, uv);
	// - numVerts: size of the vertex list;
	// - (optional) numFaceGroupsHint: expected number of face groups (i.e. number of materials), so early allocation can be made;
	// - (optional) uvFlag: specifies whether the mesh has tex coordiantes or not.
	Mesh(Vertex* vertices, size_t numVerts, size_t numFaceGroupsHint = 1, bool uvFlag = true);

	//Load a new face group, i.e. a list of triangles and an associated material.
	void addFaceGroup(unsigned int* indices, size_t numIndices, const Material& mat);

	//Draw the mesh.
	void draw(State& state, VertexArrayObject& vao, const MeshUniforms& uniforms);
};

/*
* A helper class used for loading meshes from files. The class also stores the loaded meshes, so they can be deleted when the object
* goes out of scope.
*/
struct MeshLoader {
private:
	//Provide no access to the underlying storage method. 
	//For now, we settle on a simple array that doubles its size when the capacity is reached (to avoid repeated allocations).

	std::vector<Mesh*> meshes;
	TexLoader& mTexList;

public:
	//Initialize the mesh loader. A texture loader must be associated so that textures can be loaded automatically.
	//Input:
	// - texLoader: the associated tex loader;
	// - numMeshesHint: expected number of meshes to be loaded, so an early allocation can be done.
	MeshLoader(TexLoader& texLoader, int numMeshesHint = 15);
	
	//Destroy all associated meshes.
	~MeshLoader();

	//Load a mesh from a file. Returns a pointer to the mesh on success and nullptr on failure.
	//NOTE: DO NOT call delete on the pointer, the mesh will be automatically deleted when the loader object goes out of scope.
	Mesh* loadMesh(const char* filename);
};