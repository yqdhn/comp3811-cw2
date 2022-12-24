#include"lights.hpp"
#include"program.hpp"
#include"../vmlib/vec3.hpp"
#include<cmath>


LightManager::LightManager() :
	dirLightBuffer(MAX_DIR_LIGHTS * sizeof(DirLightInternal) + sizeof(int), nullptr),
	pointLightBuffer(MAX_POINT_LIGHTS * sizeof(PointLightInternal) + sizeof(int), nullptr),
	spotLightBuffer(MAX_SPOT_LIGHTS * sizeof(SpotLightInternal) + sizeof(int), nullptr),
	numDirLights(0), numPointLights(0), numSpotLights(0) {
	clearDirLights();
	clearPointLights();
	clearSpotLights();
	dirLightBuffer.bindToUniform(BINDING_UNIFORM_DIR_LIGHT, 0, dirLightBuffer.getSize());
	pointLightBuffer.bindToUniform(BINDING_UNIFORM_POINT_LIGHT, 0, pointLightBuffer.getSize());
	spotLightBuffer.bindToUniform(BINDING_UNIFORM_SPOT_LIGHT, 0, spotLightBuffer.getSize());
}

//x = -k2 / rmax2; 
//y = 1 / (1 - exp(-k2)); 
//z = exp(-k2) / (1 - exp(-k2));
Vec3f LightManager::calcAttenConsts(float rMax, float k) {
	float k2 = k * k;
	float e = exp(-k2);
	float denom = 1.0f / (1.0f - e);
	return Vec3f{
		-k2 / (rMax * rMax),
		denom,
		e * denom
	};
}

int LightManager::addDirLight(const DirLight& dirLight) {
	if (numDirLights >= MAX_DIR_LIGHTS) {
		throw Error("Cannot create a new dir light, because max number of dir lights (%i) has been reached.", MAX_DIR_LIGHTS);
	}
	DirLightInternal dirLightNew{ dirLight.dir,0.0f,dirLight.col,0.0f };
	
	//Update buffer
	dirLightBuffer.setData(numDirLights * sizeof(DirLightInternal), sizeof(DirLightInternal), &dirLightNew);
	numDirLights++;
	dirLightBuffer.setData(MAX_DIR_LIGHTS * sizeof(DirLightInternal), sizeof(int), &numDirLights);
	
	return numDirLights - 1;//return light index
}

int LightManager::addPointLight(const PointLight& pointLight, float attenFuncPow) {
	if (numPointLights >= MAX_POINT_LIGHTS) {
		throw Error("Cannot create a new point light, because max number of point lights (%i) has been reached.", MAX_POINT_LIGHTS);
	}
	PointLightInternal pointLightNew{ pointLight.pos,0.0f,pointLight.col,0.0f,calcAttenConsts(pointLight.rMax,attenFuncPow),0.0f };
	//Update buffer
	pointLightBuffer.setData(numPointLights * sizeof(PointLightInternal), sizeof(PointLightInternal), &pointLightNew);
	numPointLights++;
	pointLightBuffer.setData(MAX_POINT_LIGHTS * sizeof(PointLightInternal), sizeof(int), &numPointLights);
	
	return numPointLights - 1;//return light index
}

int LightManager::addSpotLight(const SpotLight& spotLight, float attenFuncPow) {
	if (numSpotLights >= MAX_SPOT_LIGHTS) {
		throw Error("Cannot create a new spot light, because max number of spot lights (%i) has been reached.", MAX_SPOT_LIGHTS);
	}
	SpotLightInternal spotLightNew{ spotLight.pos,std::cosf(spotLight.innerCone),spotLight.dir,std::cosf(spotLight.outerCone),
	spotLight.col,0.0f,calcAttenConsts(spotLight.rMax,attenFuncPow),0.0f };

	//Update buffer
	spotLightBuffer.setData(numSpotLights * sizeof(SpotLightInternal), sizeof(SpotLightInternal), &spotLightNew);
	numSpotLights++;
	spotLightBuffer.setData(MAX_SPOT_LIGHTS * sizeof(SpotLightInternal), sizeof(int), &numSpotLights);

	return numSpotLights-1;//return light index
}

void LightManager::editDirLight(int idx, const DirLight& dirLight) {
	if (idx >= MAX_DIR_LIGHTS || idx < 0) {
		throw Error("Invalid index. Must be in the range 0-%i", MAX_DIR_LIGHTS - 1);
	}
	DirLightInternal dirLightNew{ dirLight.dir,0.0f,dirLight.col,0.0f };
	dirLightBuffer.setData(idx * sizeof(DirLightInternal), sizeof(DirLightInternal), &dirLightNew);
}

void LightManager::editPointLight(int idx, const PointLight& pointLight, float attenFuncPow) {
	if (idx >= MAX_POINT_LIGHTS || idx < 0) {
		throw Error("Invalid index. Must be in the range 0-%i", MAX_POINT_LIGHTS - 1);
	}
	PointLightInternal pointLightNew{ pointLight.pos,0.0f,pointLight.col,0.0f,calcAttenConsts(pointLight.rMax,attenFuncPow),0.0f };

	pointLightBuffer.setData(idx * sizeof(PointLightInternal), sizeof(PointLightInternal), &pointLightNew);
}

void LightManager::editSpotLight(int idx, const SpotLight& spotLight, float attenFuncPow) {
	if (idx >= MAX_SPOT_LIGHTS || idx < 0) {
		throw Error("Invalid index. Must be in the range 0-%i", MAX_SPOT_LIGHTS - 1);
	}
	SpotLightInternal spotLightNew{ spotLight.pos,std::cosf(spotLight.innerCone),spotLight.dir,std::cosf(spotLight.outerCone),
		spotLight.col,0.0f,calcAttenConsts(spotLight.rMax,attenFuncPow),0.0f };
	spotLightBuffer.setData(idx * sizeof(SpotLightInternal), sizeof(SpotLightInternal), &spotLightNew);
}

void LightManager::clearDirLights() {
	numDirLights = 0;
	dirLightBuffer.setData(MAX_DIR_LIGHTS * sizeof(DirLightInternal), sizeof(int), &numDirLights);
}

void LightManager::clearPointLights() {
	numPointLights = 0;
	pointLightBuffer.setData(MAX_POINT_LIGHTS * sizeof(PointLightInternal), sizeof(int), &numPointLights);
}

void LightManager::clearSpotLights() {
	numSpotLights = 0;
	spotLightBuffer.setData(MAX_SPOT_LIGHTS * sizeof(SpotLightInternal), sizeof(int), &numSpotLights);
}

void LightManager::setAmbientLight(const Vec3f ambLight, const ShaderProgram& program) {
	glProgramUniform3f(program.programId(), LOCATION_UNIFORM_AMBIENT_LIGHT, ambLight.x, ambLight.y, ambLight.z);
}
