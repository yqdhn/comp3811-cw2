#include"material.hpp"
#include"program.hpp"
#include"texture.hpp"
#include<glad.h>

int Material::numMaterials = 0;
Texture* Material::defaultTexWhite = nullptr;
Texture* Material::defaultTexBlack = nullptr;
Texture* Material::defaultTexBump = nullptr;

void Material::tryInitDefTex() {
	if (defaultTexWhite == nullptr) {
		unsigned char white[4] = { 255,255,255,255 };
		defaultTexWhite = new Texture(1, 1, white);
	}
	if (defaultTexBlack == nullptr) {
		unsigned char black[4] = { 0,0,0,255 };
		defaultTexBlack = new Texture(1, 1, black);
	}
	if (defaultTexBump == nullptr) {
		unsigned char bump[4] = { 0,0,255,255 };
		defaultTexBump = new Texture(1, 1, bump);
	}
}

void Material::tryFreeDefTex() {
	if (numMaterials == 0) {
		if (defaultTexWhite) {
			delete defaultTexWhite;
			defaultTexWhite = nullptr;
		}
		if (defaultTexBlack) {
			delete defaultTexBlack;
			defaultTexBlack = nullptr;
		}
		if (defaultTexBump) {
			delete defaultTexBump;
			defaultTexBump = nullptr;
		}
	}
}

Material::Material() :
	ambientCoeff({ 1.0f,1.0f,1.0f }),
	diffuseCoeff({0.8f,0.8f,0.8f}),
	specularCoeff({0.5f,0.5f,0.5f}),
	emissiveCoeff({0.0f,0.0f,0.0f}),
	specularExp(0.0f),
	diffTex(nullptr),
	specTex(nullptr),
	metallicTex(nullptr),
	roughnessTex(nullptr),
	ambientTex(nullptr),
	normalMap(nullptr),
	emissiveTex(nullptr),
	maskTex(nullptr)
{
	tryInitDefTex();
	numMaterials++;
}

Material::Material(const Material& mat) : 
	ambientCoeff(mat.ambientCoeff),
	diffuseCoeff(mat.diffuseCoeff), 
	specularCoeff(mat.specularCoeff),
	emissiveCoeff(mat.emissiveCoeff),
	specularExp(mat.specularExp), 
	diffTex(mat.diffTex), 
	specTex(mat.specTex),
	metallicTex(mat.metallicTex),
	roughnessTex(mat.roughnessTex),
	ambientTex(mat.ambientTex),
	normalMap(mat.normalMap),
	emissiveTex(mat.emissiveTex),
	maskTex(mat.maskTex)
{
	//Update reference counter on copy
	numMaterials++;
}

Material::~Material() {
	//free default tex when the last material gets deleted
	numMaterials--;
	tryFreeDefTex();
}

void Material::setNonPbrParams(const Vec3f& ambient, const Vec3f& diffuse, const Vec3f& specular, 
	const Vec3f& emissive, float shininess, Texture* diffMap, Texture* specMap) {
	ambientCoeff = ambient;
	diffuseCoeff = diffuse;
	specularCoeff = specular;
	emissiveCoeff = emissive;
	specularExp = shininess;
	diffTex = diffMap;
	specTex = specMap;
	
}

void Material::bindUniforms(const ShaderProgram& program) {
	glProgramUniform3f(program.programId(), MATERIAL_STARTING_INDEX, ambientCoeff.x, ambientCoeff.y, ambientCoeff.z);
	glProgramUniform3f(program.programId(), MATERIAL_STARTING_INDEX + 1, diffuseCoeff.x, diffuseCoeff.y, diffuseCoeff.z);
	glProgramUniform3f(program.programId(), MATERIAL_STARTING_INDEX + 2, specularCoeff.x, specularCoeff.y, specularCoeff.z);
	glProgramUniform3f(program.programId(), MATERIAL_STARTING_INDEX + 3, emissiveCoeff.x, emissiveCoeff.y, emissiveCoeff.z);
	glProgramUniform1f(program.programId(), MATERIAL_STARTING_INDEX + 4, specularExp);
}

void Material::setPbrParams(Texture* albedo, Texture* metallic, Texture* roughness, Texture* ambient) {
	diffTex = albedo;
	metallicTex = metallic;
	roughnessTex = roughness;
	ambientTex = ambient;
	
}

void Material::setAdditionalParams(Texture* emissionMap, Texture* bump, Texture* mask) {
	normalMap = bump;
	emissiveTex = emissionMap;
	maskTex = mask;
}

void Material::bindNonPbrParams(const ShaderProgram& program) {
	bindUniforms(program);

	if (diffTex) diffTex->bindTex(BINDING_TEX_DIFF);
	else defaultTexWhite->bindTex(BINDING_TEX_DIFF);

	if (specTex) specTex->bindTex(BINDING_TEX_SPEC);
	else defaultTexWhite->bindTex(BINDING_TEX_SPEC);

	if (normalMap) normalMap->bindTex(BINDING_TEX_BUMP);
	else defaultTexBump->bindTex(BINDING_TEX_BUMP);

	if (emissiveTex) emissiveTex->bindTex(BINDING_TEX_EMISSIVE);
	else defaultTexWhite->bindTex(BINDING_TEX_EMISSIVE);

	if (maskTex) maskTex->bindTex(BINDING_TEX_MASK);
	else defaultTexWhite->bindTex(BINDING_TEX_MASK);
}

void Material::bindPbrParams(const ShaderProgram& program) {
	if (diffTex) diffTex->bindTex(BINDING_TEX_ALBEDO);
	else defaultTexWhite->bindTex(BINDING_TEX_ALBEDO);

	if (metallicTex) metallicTex->bindTex(BINDING_TEX_METALLIC);
	else defaultTexBlack->bindTex(BINDING_TEX_METALLIC);

	if (roughnessTex) roughnessTex->bindTex(BINDING_TEX_ROUGHNESS);
	else defaultTexBlack->bindTex(BINDING_TEX_ROUGHNESS);

	if (ambientTex) ambientTex->bindTex(BINDING_TEX_AMBIENT);
	else defaultTexWhite->bindTex(BINDING_TEX_AMBIENT);

	if (normalMap) normalMap->bindTex(BINDING_TEX_BUMP);
	else defaultTexBump->bindTex(BINDING_TEX_BUMP);

	if (emissiveTex) emissiveTex->bindTex(BINDING_TEX_EMISSIVE);
	else defaultTexWhite->bindTex(BINDING_TEX_EMISSIVE);

	if (maskTex) maskTex->bindTex(BINDING_TEX_MASK);
	else defaultTexWhite->bindTex(BINDING_TEX_MASK);

	glProgramUniform3f(program.programId(), MATERIAL_STARTING_INDEX + 3, emissiveCoeff.x, emissiveCoeff.y, emissiveCoeff.z);
}

void Material::bindMaterialNoTex(const ShaderProgram& program, const LightModel& lightModel) {	
	//Bind default texture for all texture types!
	tryInitDefTex();//allocates on first call only
	if (lightModel == LightModel::BlinnPhong) {
		bindUniforms(program);
		defaultTexWhite->bindTex(BINDING_TEX_DIFF);
		defaultTexWhite->bindTex(BINDING_TEX_SPEC);
		defaultTexBump->bindTex(BINDING_TEX_BUMP);
	}
	else {
		defaultTexWhite->bindTex(BINDING_TEX_ALBEDO);
		defaultTexBlack->bindTex(BINDING_TEX_METALLIC);
		defaultTexBlack->bindTex(BINDING_TEX_ROUGHNESS);
		defaultTexWhite->bindTex(BINDING_TEX_AMBIENT);
	}

}
