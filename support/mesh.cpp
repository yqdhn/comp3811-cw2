#include "mesh.hpp"
#include"program.hpp"
#include"texture.hpp"
#include"rapidobj/rapidobj.hpp"
#include<unordered_map>
#include"window.hpp"
#include"camera.hpp"

const char* ASSETS_TEX_DIR = "./assets/";


Mesh::Mesh(Vertex* vertices, size_t numVerts, size_t numFaceGroupsHint, bool uvFlag) : vbo(numVerts*sizeof(Vertex),vertices), hasUVs(uvFlag) {

	//Reserve space for face groups
	faceGroups.reserve(numFaceGroupsHint);
}

void Mesh::addFaceGroup(unsigned int* indices, size_t numIndices, const Material& mat) {
	faceGroups.emplace_back(indices, numIndices, mat);
}

void Mesh::draw(State& state, VertexArrayObject& vao, const MeshUniforms& uniforms) {

	vbo.bindToAttrib(vao, ATTRIB_LOCATION_VERT_POS, 0, sizeof(Vertex));//bind buffer to pos binding point, stride is the vertex size
	vbo.bindToAttrib(vao, ATTRIB_LOCATION_VERT_NORMAL, 0, sizeof(Vertex));//bind buffer to normal binding point
	vbo.bindToAttrib(vao, ATTRIB_LOCATION_VERT_UV, 0, sizeof(Vertex));//bind buffer to uv binding point
	vbo.bindToAttrib(vao, ATTRIB_LOCATION_VERT_TANGENT, 0, sizeof(Vertex));//bind buffer to tangent binding point

	ShaderProgram* last = nullptr;
	
	for (auto it = faceGroups.begin(); it != faceGroups.end(); it++) {
		it->veb.bindAsElementBuf(vao);
		ShaderProgram* program = nullptr;

		
		if (it->mat.normalMap)
			program = state.programs->getProgram(RenderSettings::BUMP_MAP);
		if(!program) program = state.programs->getProgram(RenderSettings::STANDARD);

		if (program) {
			glUseProgram(program->programId());
			//Set up uniforms
			if (uniforms.modelMat) glProgramUniformMatrix4fv(program->programId(), 0, 1, GL_TRUE, uniforms.modelMat->v);
			if (uniforms.modelMatN) glProgramUniformMatrix4fv(program->programId(), 1, 1, GL_TRUE, uniforms.modelMatN->v);
			if (uniforms.viewProjMat) glProgramUniformMatrix4fv(program->programId(), 2, 1, GL_TRUE, uniforms.viewProjMat->v);
			glProgramUniform3f(program->programId(), 3, state.cam->getPosition().x, state.cam->getPosition().y, state.cam->getPosition().z);

			//Bind appropriate material parameters
			if (hasUVs) {
				if (state.programs->lightModel == LightModel::PBR) {
					it->mat.bindPbrParams(*program);
				}
				else
					it->mat.bindNonPbrParams(*program);
			}
			else {
				it->mat.bindMaterialNoTex(*program, state.programs->lightModel);
			}
			
		}

		//Draw
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(it->numIndices), GL_UNSIGNED_INT, nullptr);
	}
}


Mesh::MaterialFaceGroupInternal::MaterialFaceGroupInternal(unsigned int* indices, size_t numIndices, const Material& material) :
	mat(material), veb(numIndices  * sizeof(int), indices), numIndices(numIndices) {
}


//#####################################//
// HELPER FUNCTIONS RELATED TO HASHING //
//#####################################//

namespace {

	//A simple convertex from rapidobj vector to our custom Vec3f.
	Vec3f toVec3f(const rapidobj::Float3& fl) {
		return Vec3f{ fl[0],fl[1],fl[2] };
	}

	//A function to incrementally create a hash value from several hashed variables.
	//Implementation is a popular one and is taken from the Boost specification:
	//https://www.boost.org/doc/libs/1_55_0/doc/html/hash/reference.html#boost.hash_combine
	//Further discussion can be found here:
	//https://stackoverflow.com/questions/5889238/why-is-xor-the-default-way-to-combine-hashes
	//(p.s. title is misleading)
	template<class T>
	void hashCombine(size_t& seed, T const& v) {
		seed ^= std::hash<T>()(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}

	//A custom struct of 3 integer indices that includes a compare function (required for hashing).
	struct IndexCombined {
		int positionIdx;
		int normalIdx;
		int uvIdx;
		IndexCombined(const rapidobj::Index& idx) :positionIdx(idx.position_index), normalIdx(idx.normal_index), uvIdx(idx.texcoord_index) {}
		IndexCombined(int pos, int normal, int uv) :positionIdx(pos), normalIdx(normal), uvIdx(uv) {}
	};

	//Required for hashing.
	bool operator==(const IndexCombined& left, const IndexCombined& right) {
		return left.positionIdx == right.positionIdx && left.normalIdx == right.normalIdx && left.uvIdx == right.uvIdx;
	}

}

//Required for hashing.
bool operator==(const Vec3f& left, const Vec3f& right) {
	return left.x == right.x && left.y == right.y && left.z == right.z; //since we are looking for exact duplicates, testing for equality is fine
}

//Custom hash function used to hash a struct of 3 integers. We use hash combine to incrementally create the hash.
//Approach following Boost tutorial: https://www.boost.org/doc/libs/1_63_0/doc/html/hash/combine.html
namespace std {
	template <>
	struct hash<IndexCombined>
	{
		std::size_t operator()(const IndexCombined& idx) const {
			size_t seed = 0;
			hashCombine(seed, static_cast<size_t>(idx.positionIdx));
			hashCombine(seed, static_cast<size_t>(idx.normalIdx));
			hashCombine(seed, static_cast<size_t>(idx.uvIdx));
			return seed;
		}
	};

	template <>
	struct hash<Vec3f>
	{
		std::size_t operator()(const Vec3f& vec) const {
			size_t seed = 0;
			hashCombine(seed, vec.x);
			hashCombine(seed, vec.y);
			hashCombine(seed, vec.z);
			return seed;
		}
	};
}


//#################################################//
// HELPER FUNCTIONS RELATED TO TANGENT CALCULATION //
//#################################################//
namespace {

//Following the approach outlines by E.Lengyel in 
//"Mathematics for 3D Game Programming and Computer Graphics", Ch.7.8 Bump Mapping.
	void calculateTangents(Vertex* vertices, size_t numVerts, const std::vector<std::vector<unsigned int>>& indexArrays, std::vector<bool>& idxArrHasBumpMap) {
		std::vector<Vec3f> tangents(numVerts, { 0.0f,0.0f,0.0f });
		std::vector<Vec3f> bitangents(numVerts, { 0.0f,0.0f,0.0f });
		//std::unordered_map<Vec3f, int> vertexMap;

		for (size_t arrayIdx = 0; arrayIdx < indexArrays.size(); arrayIdx++) {
			if (!idxArrHasBumpMap[arrayIdx]) continue;
			auto& indices = indexArrays[arrayIdx];

			for (size_t triIdx = 0; triIdx < indices.size() / 3; triIdx++) {
				Vertex v0 = vertices[indices[triIdx * 3]];
				Vertex v1 = vertices[indices[triIdx * 3 + 1]];
				Vertex v2 = vertices[indices[triIdx * 3 + 2]];
				
				Vec2f uv0 = v0.texCoords;
				Vec2f uv1 = v1.texCoords;
				Vec2f uv2 = v2.texCoords;
				Vec2f diff1 = uv1 - uv0;
				Vec2f diff2 = uv2 - uv0;
				Vec2f diff3 = uv2 - uv1;
				if (abs(diff1.x) < 1.e-6f && abs(diff1.y) < 1.e-6f) uv1 += Vec2f{ 0.1,0.1 };
				if (abs(diff2.x) < 1.e-6f && abs(diff2.y) < 1.e-6f) uv2 += Vec2f{ 0.1,0.1 };
				if (abs(diff3.x) < 1.e-6f && abs(diff3.y) < 1.e-6f) uv2 += Vec2f{ 0.1,0.1 };


				//Fetch edges and uv differences of for the current tri. Valid indices assumed.
				Vec3f e0 = v1.pos - v0.pos;
				Vec3f e1 = v2.pos - v0.pos;
				double s0 = uv1.x*100 - uv0.x*100;
				double t0 = uv1.y*100 - uv0.y*100;
				double s1 = uv2.x*100 - uv0.x*100;
				double t1 = uv2.y*100 - uv0.y*100;

				double denom = s0 * t1 - s1 * t0;

				if (abs(denom) >= 1.e-6) {
					double r = 1.0 / denom;
					Vec3f T{
						(t1 * e0.x - t0 * e1.x) * r,
						(t1 * e0.y - t0 * e1.y) * r,
						(t1 * e0.z - t0 * e1.z) * r };
					Vec3f B{
						(s0 * e1.x - s1 * e0.x) * r,
						(s0 * e1.y - s1 * e0.y) * r,
						(s0 * e1.z - s1 * e0.z) * r };

					//Distribute to vertices so a per-vertex tangent can be calculated
					for (int i = 0; i < 3; i++) {
						vertices[indices[triIdx * 3 + i]].tangent += T;
						bitangents[(size_t)indices[triIdx * 3 + i]] += B;
					}
				}
			}
		}

		for (long vertIdx = 0; vertIdx < numVerts; vertIdx++)
		{
	
			const Vec3f& n = vertices[vertIdx].normal;
			Vec3f& t = vertices[vertIdx].tangent;

			float& w = vertices[vertIdx].handedness;

			if (length(t) < 1.e-6) {
				w = 0.0f;//value that signals no bump mapping to be applied
			}
			else {
				
				// Gram-Schmidt orthogonalize.
				t = normalize(t - dot(n, t) * n);

				// Calculate handedness.
				w = (dot(cross(n, t), bitangents[vertIdx]) < 0.0f) ? -1.0f : 1.0f;
				vertices[vertIdx].tangent = t;
			}
		}
	}
}




MeshLoader::MeshLoader(TexLoader& texLoader, int numMeshesHint) : mTexList(texLoader) {
	meshes.reserve(numMeshesHint);
}

MeshLoader::~MeshLoader() {
	for (int i = 0; i < meshes.size(); i++) {
		if (meshes[i]) {
			delete meshes[i];
		}
	}
}

Mesh* MeshLoader::loadMesh(const char* filename) {
	printf("\nStarted import on %s", filename);
	//Load the mesh and check for errors
	rapidobj::Result result = rapidobj::ParseFile(filename);

	if (result.error) {
		throw Error(result.error.code.message().append("\n").c_str());
	}
	bool success = rapidobj::Triangulate(result);
	if (!success) {
		throw Error(result.error.code.message().append("\n").c_str());
	}

	//Fetch mesh info
	size_t numTris = 0;
	for (const auto& shape : result.shapes) {
		numTris += shape.mesh.num_face_vertices.size();
	}
	size_t numVerts = result.attributes.positions.size()/3;
	size_t numNormals = result.attributes.positions.size() / 3;
	size_t numUVs = result.attributes.texcoords.size()/2;
	size_t numMaterials = result.materials.size();
	bool hasUVs = numUVs > 0;

	if (numVerts == 0) throw Error("Could not load %s. 0 vertices.\n", filename);
	if (numTris == 0) throw Error("Could not load %s. 0 faces.\n", filename);
	if (numNormals == 0) throw Error("Could not load %s. 0 normals.\n", filename);
	if (numMaterials == 0) throw Error("Could not load %s. 0 materials.\n", filename);

	//Prepare vertex array and index arrays (one index array per material).
	std::vector<Vertex> vertices;
	vertices.reserve(numVerts);

	std::vector<std::vector<unsigned int>> indexArrays;
	indexArrays.resize(numMaterials);
	size_t idxArraySizeHint = numTris * 3 / numMaterials;
	for (auto& arr : indexArrays) {
		arr.reserve(idxArraySizeHint);
	}

	int vert[3] = {};

	//Used to map combined indices to a single vertex index that will be added to the element array.
	std::unordered_map<IndexCombined, int> vertIdxMap;

	//Traverse all shapes in the mesh (will get unified)
	for (const auto& shape : result.shapes) {
		//Traverse all faces in the shape
		for (size_t faceIdx = 0; faceIdx < shape.mesh.num_face_vertices.size(); faceIdx++) {
			int matIdx = shape.mesh.material_ids[faceIdx];
			if (matIdx == -1) throw Error("Face %i of %s has no assigned materials\n", faceIdx, filename);

			for (size_t vertIdx = 0; vertIdx < 3; vertIdx++) {
				//To see if this is a new or repeated vertex, we search the index (combines pos, normal, and uv idx) in a hash map.
				//If not found, then this is the first occurence of the vertex, so we hash its idx.
				IndexCombined combinedIdx(shape.mesh.indices[faceIdx * 3 + vertIdx]);
				auto it = vertIdxMap.find(combinedIdx);

				if (it == vertIdxMap.end()) {
					auto insResult = vertIdxMap.insert({ combinedIdx, static_cast<int>(vertices.size()) });
					if (!insResult.second) throw Error("Error inserting f%i, v%i to the hash map, while loading %s",
						static_cast<int>(faceIdx), static_cast<int>(vertIdx), filename);

					it = insResult.first;

					//Compute vertex data
					Vec3f pos{
						result.attributes.positions[combinedIdx.positionIdx * 3],
						result.attributes.positions[combinedIdx.positionIdx * 3 + 1],
						result.attributes.positions[combinedIdx.positionIdx * 3 + 2],
					};
					Vec3f normal{
						result.attributes.normals[combinedIdx.normalIdx * 3],
						result.attributes.normals[combinedIdx.normalIdx * 3 + 1],
						result.attributes.normals[combinedIdx.normalIdx * 3 + 2],
					};
					Vec2f uv{ 
						hasUVs ? result.attributes.texcoords[combinedIdx.uvIdx * 2] : 0.0f,
						hasUVs ? result.attributes.texcoords[combinedIdx.uvIdx * 2 + 1] : 0.0f,
					};

					//Insert new vertex
					vertices.push_back({
						pos,
						normal,
						uv,
						Vec3f{0.0f,0.0f,0.0f},//some tangent default
						0.0f,//this default tells the vertex shader to perform no bump mapping
					});
					if (vertices.size() == vertices.capacity()) vertices.reserve(vertices.size() * 2);//expand vector if required
				}
				if (faceIdx == 78) {
					vert[vertIdx] = it->second;
				}
				//Insert vertex index to the respective array
				indexArrays[matIdx].push_back(it->second);
				size_t arrSize = indexArrays[matIdx].size();

				if (arrSize == indexArrays[matIdx].capacity()) indexArrays[matIdx].reserve(arrSize * 2);//expand vector if required
			}
		}

	}
	
	//Handle bump mapping for the face groups that require it
	std::vector<bool> idxArrHasBumpMap;
	idxArrHasBumpMap.resize(indexArrays.size());
	for (int i = 0; i < indexArrays.size(); i++) {
		idxArrHasBumpMap[i] = !result.materials[i].bump_texname.empty();
	}
	calculateTangents(vertices.data(), vertices.size(), indexArrays, idxArrHasBumpMap);

	Mesh* newMesh = new Mesh(vertices.data(), vertices.size(), indexArrays.size(), hasUVs);
	meshes.push_back(newMesh);//vertex list
	
	for (size_t i = 0; i < indexArrays.size();i++) {//face groups

		std::string texPath(ASSETS_TEX_DIR);
		std::string pathDiff = std::string(ASSETS_TEX_DIR) + result.materials[i].diffuse_texname;
		std::string pathSpec = std::string(ASSETS_TEX_DIR) + result.materials[i].specular_texname;
		std::string pathMetallic = std::string(ASSETS_TEX_DIR) + result.materials[i].metallic_texname;
		std::string pathRoughness = std::string(ASSETS_TEX_DIR) + result.materials[i].roughness_texname;
		std::string pathAmbient = std::string(ASSETS_TEX_DIR) + result.materials[i].ambient_texname;
		std::string pathBump = std::string(ASSETS_TEX_DIR) + result.materials[i].bump_texname;
		std::string pathEmissive = std::string(ASSETS_TEX_DIR) + result.materials[i].emissive_texname;

		//Set to nullptr if no associated texture. The material will automatically bind the default texture.
		Texture* diffTex = result.materials[i].diffuse_texname.empty() ? nullptr : mTexList.loadTexture(pathDiff.c_str(), ColorSpace::SRGB);
		Texture* specTex = result.materials[i].specular_texname.empty() ? nullptr : mTexList.loadTexture(pathSpec.c_str(), ColorSpace::LINEAR);
		Texture* metallicTex = result.materials[i].metallic_texname.empty() ? nullptr : mTexList.loadTexture(pathMetallic.c_str(),ColorSpace::LINEAR);
		Texture* roughnessTex = result.materials[i].roughness_texname.empty() ? nullptr : mTexList.loadTexture(pathRoughness.c_str(), ColorSpace::LINEAR);
		Texture* ambientTex = result.materials[i].ambient_texname.empty() ? nullptr : mTexList.loadTexture(pathAmbient.c_str(), ColorSpace::LINEAR);
		Texture* bumpTex = result.materials[i].bump_texname.empty() ? nullptr : mTexList.loadTexture(pathBump.c_str(), ColorSpace::LINEAR);
		Texture* emissiveTex = result.materials[i].emissive_texname.empty() ? nullptr : mTexList.loadTexture(pathEmissive.c_str(), ColorSpace::LINEAR);		

		Material mat;
		mat.setNonPbrParams(
			toVec3f(result.materials[i].ambient),
			toVec3f(result.materials[i].diffuse),
			toVec3f(result.materials[i].specular),
			toVec3f(result.materials[i].emission),
			result.materials[i].shininess,
			diffTex,
			specTex
		);
		mat.setPbrParams(diffTex, metallicTex, roughnessTex, ambientTex);
		mat.setAdditionalParams(emissiveTex, bumpTex, nullptr);

		newMesh->addFaceGroup(indexArrays[i].data(), indexArrays[i].size(), mat);
	}

	if (meshes.size() == meshes.capacity()) meshes.reserve(meshes.size() * 2);

	printf("Mesh %s successfully imported\n", filename);
	printf("Number of triangles: %i\n", (int)numTris);
	printf("Number of verts: %i\n", (int)numVerts);
	printf("Number of normals: %i\n", (int)numNormals);
	printf("Number of UVs: %i\n", (int)numUVs);
	printf("Number of materials: %i\n", static_cast<int>(result.materials.size()));
	return newMesh;
}
