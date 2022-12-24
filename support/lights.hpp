#pragma once
#include "../vmlib/vec3.hpp"
#include "../vmlib/vec4.hpp"
#include"error.hpp"
#include"buffer.hpp"
#include<glad.h>
#include<vector>

class ShaderProgram;

const unsigned int MAX_DIR_LIGHTS = 3;
const unsigned int MAX_POINT_LIGHTS = 10;
const unsigned int MAX_SPOT_LIGHTS = 10;

const int BINDING_UNIFORM_DIR_LIGHT = 0;
const int BINDING_UNIFORM_POINT_LIGHT = 1;
const int BINDING_UNIFORM_SPOT_LIGHT = 2;
const int LOCATION_UNIFORM_AMBIENT_LIGHT = 4;

struct DirLight {
	Vec3f dir;
	Vec3f col;
};

struct PointLight {
	Vec3f pos;
	Vec3f col;
	float rMax;//maximum radius of illumination
};

struct SpotLight {
	Vec3f pos;
	Vec3f dir;
	Vec3f col;
	float innerCone;//inner cone angle in radians
	float outerCone;//outer cone angle in radians
	float rMax;//maximum radius of illumination
};

class LightManager {
private:
	//For consistency, we prefer to add explicit padding and keep the 'pos' and 'dir' members Vec3f for all structs
	//instead of setting them Vec4f for some structs (to avoid explicit padding variables) and Vec3f in others.
	struct DirLightInternal {
		Vec3f dir;
		float padding; //unused variable
		Vec3f col;
		float padding2;
	};

	struct PointLightInternal {
		Vec3f pos;
		float padding;//unused variable
		Vec3f col;
		float padding2;
		Vec3f attenConst;
		float padding3;
	};

	struct SpotLightInternal {
		Vec3f pos;
		float innerCone;
		Vec3f dir;
		float outerCone;
		Vec3f col;
		float padding;
		Vec3f attenConst;
		float padding2;
	};

	Buffer dirLightBuffer;
	Buffer pointLightBuffer;
	Buffer spotLightBuffer;
	int numDirLights;
	int numPointLights;
	int numSpotLights;

	Vec3f calcAttenConsts(float rMax, float k = 2);

public:
	LightManager();

	int addDirLight(const DirLight& dirLight);

	int addPointLight(const PointLight& pointLight, float attenFuncPow = 2);

	int addSpotLight(const SpotLight& spotLight, float attenFuncPow = 2);

	void editDirLight(int idx, const DirLight& dirLight);

	void editPointLight(int idx, const PointLight& pointLight, float attenFuncPow = 2);

	void editSpotLight(int idx, const SpotLight& spotLight, float attenFuncPow = 2);

	void clearDirLights();

	void clearPointLights();

	void clearSpotLights();

	void setAmbientLight(const Vec3f ambLight, const ShaderProgram& program);
};