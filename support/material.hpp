#pragma once
#include "../vmlib/vec3.hpp"
#include<vector>

constexpr const int MATERIAL_STARTING_INDEX = 5;
constexpr const int BINDING_TEX_DIFF = 0;
constexpr const int BINDING_TEX_SPEC = 1;
constexpr const int BINDING_TEX_ALBEDO = 2;
constexpr const int BINDING_TEX_METALLIC = 3;
constexpr const int BINDING_TEX_ROUGHNESS = 4;
constexpr const int BINDING_TEX_AMBIENT = 5;
constexpr const int BINDING_TEX_BUMP = 6;
constexpr const int BINDING_TEX_EMISSIVE = 7;
constexpr const int BINDING_TEX_MASK = 8;

class ShaderProgram;
class Texture;
enum class LightModel;

/*
* A rendering material.
*  - ambinet colour;
*  - diffuse colour;
*  - specular colour;
*  - emissive colour;
*  - specular exponent;
*  - diffuse texture (or default 1x1 white texture if no attached texture).
*  - specular texture (or default 1x1 white texture if no attached texture).
*/
struct Material {
public:
	//Default texture object to be used in case there is no actual texture available.
	//This is a 1x1 white square (leaves light calculations unaffected).
	static Texture* defaultTexWhite;
	static Texture* defaultTexBlack;
	static Texture* defaultTexBump;

	//Number of existing materials. Used as a reference counter to decide when to delete the default texture.
	static int numMaterials;

	void tryInitDefTex();
	void tryFreeDefTex();

	//Utility function to avoid duplicate code
	void bindUniforms(const ShaderProgram& program);

public:
	//Blinn-Phong parameters
	Vec3f ambientCoeff;
	Vec3f diffuseCoeff;
	Vec3f specularCoeff;
	Vec3f emissiveCoeff;
	float specularExp;
	Texture* diffTex;
	Texture* specTex;

	//PBR parameters
	Texture* metallicTex;
	Texture* roughnessTex;
	Texture* ambientTex;

	//Common parameters
	Texture* emissiveTex;
	Texture* normalMap;
	Texture* maskTex;

	Material();
	Material(const Material& mat);
	~Material();

	void setNonPbrParams(const Vec3f& ambient, const Vec3f& diffuse, const Vec3f& specular, const Vec3f& emissive, 
		float shininess, Texture* diffMap, Texture* specMap);

	void setPbrParams(Texture* albedo, Texture* metallic, Texture* roughness, Texture* ambient);

	void setAdditionalParams(Texture* emissionMap, Texture* normalMap, Texture* mask);

	//Bind material for non-pbr rendering
	void bindNonPbrParams(const ShaderProgram& program);

	//Bind material for pbr rendering
	void bindPbrParams(const ShaderProgram& program);

	//Bind material for rendering, but bind the default texture on all texture slots.
	//To be used by meshes that have no uv coordinates.
	void bindMaterialNoTex(const ShaderProgram& program, const LightModel& lightModel);
};