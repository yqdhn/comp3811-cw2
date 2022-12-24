//imgui
#include "../support/imgui/imgui.h"
#include "../support/imgui/imgui_impl_glfw.h"
#include "../support/imgui/imgui_impl_opengl3.h"

#include "rapidobj/rapidobj.hpp"
#include <glad.h>
#include <GLFW/glfw3.h>

#include <typeinfo>
#include <stdexcept>

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <stb_image.h>

#include "../support/error.hpp"
#include "../support/program.hpp"
#include "../support/checkpoint.hpp"
#include "../support/debug_output.hpp"
#include "../support/window.hpp"
#include "../support/camera.hpp"
#include "../support/lights.hpp"
#include "../support/material.hpp"
#include "../support/buffer.hpp"
#include "../support/texture.hpp"
#include "../support/mesh.hpp"

#include "../vmlib/vec4.hpp"
#include "../vmlib/mat44.hpp"
#include "defaults.hpp"
#include "element1.hpp"

#if defined _WIN32
// Trick to make the code run on the external GPU on windows.
// Taken from the slides of Lec.14: Meshes.
extern "C"
{
	__declspec(dllexport) unsigned long NvOptimusEnablement = 1;
	__declspec(dllexport) unsigned long AmdPowerXpressRequestHighPerformance = 1;
	// untested// See https://stackoverflow.com/questions/17458803/amd-equivalent-to-nvoptimusenablement
}
#endif

namespace
{
	constexpr char const *kWindowTitle = "COMP3811 - Coursework 2";
	constexpr char const* oldwoody = "./assets/background-top-view-old-vintage-aged-brushed-brown-wooden-table-rich-texture.jpg";
	constexpr char const* thefloor = "./assets/floor.obj"; 
	constexpr char const *arena = "./assets/wallsnew.obj";
	constexpr char const *roof = "./assets/roofPbr.obj";
	//constexpr char const *element1 = "./assets/element1.obj";
	constexpr char const* element3 = "./assets/elemnt3.obj";
	constexpr char const *element4 = "./assets/element4.obj";
	constexpr char const* oldbox = "./assets/openoldbox.obj";
	constexpr char const* sword = "./assets/sword.obj";
	constexpr char const *boxWood = "./assets/woodbox.obj";
	constexpr char const *table = "./assets/Table.obj";
	constexpr char const *chair = "./assets/Chair.obj";
	constexpr char const *target = "./assets/target.obj";
	constexpr char const *target2 = "./assets/target2.obj";
	constexpr char const *light = "./assets/light.obj";
	constexpr char const* plane = "./assets/plane2.obj";
	constexpr char const* creeperhead = "./assets/creeperhead.obj";
	constexpr char const* creeperbody = "./assets/creeperbody.obj";
	constexpr char const* creeperleg = "./assets/creeperleg.obj";
	constexpr char const* lightbulb = "./assets/lightbulb.obj";
	constexpr char const* crack = "./assets/crack.obj";

	float degToRad(float angleInDeg)
	{
		return angleInDeg * 0.01745329f; // angleInDeg * pi/180 (precomputed)
	}
	constexpr float kPi_ = 3.1415926f;
}

int main()
try
{
	Window window(1280, 720, kWindowTitle);
	ShaderProgram prog({{GL_VERTEX_SHADER, "./assets/blinnPhong.vert"},
						{GL_FRAGMENT_SHADER, "./assets/blinnPhong.frag"}});
	ShaderProgram progPbr({ {GL_VERTEX_SHADER, "./assets/cookTorrance.vert"},
						{GL_FRAGMENT_SHADER, "./assets/cookTorrance.frag"} });
	ShaderProgram progPbrBump({ {GL_VERTEX_SHADER, "./assets/cookTorranceBump.vert"},
						{GL_FRAGMENT_SHADER, "./assets/cookTorranceBump.frag"} });
	RenderSettings pbrPrograms(LightModel::PBR);
	pbrPrograms.setProgram(RenderSettings::STANDARD, &progPbr);
	pbrPrograms.setProgram(RenderSettings::BUMP_MAP, &progPbrBump);
	RenderSettings blinnPhong(LightModel::BlinnPhong);
	blinnPhong.setProgram(RenderSettings::STANDARD, &prog);

	Camera camera;
	State state(&pbrPrograms, &camera);
	window.setState(&state);

	// Ddebug output
#if !defined(NDEBUG)
	setup_gl_debug_output();
#endif // ~ !NDEBUG

	// Global GL state
	OGL_CHECKPOINT_ALWAYS();

	// TODO: global GL setup goes here
	//glEnable(GL_FRAMEBUFFER_SRGB);//perform explicit gamma correction in fragment shader instead!
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.2f, 0.2f, 0.2f, 0.0f);

	glEnable(GL_DEPTH_TEST);

	// Other initialization & loading
	OGL_CHECKPOINT_ALWAYS();

	// TODO refactor it away
	// Set up vertex attributes
	VertexArrayObject vao;
	vao.addAttribF(0, 3, 0);// positions, automatically enabled and bound to idx 0
	vao.addAttribF(1, 3, 3 * sizeof(GL_FLOAT)); // normals, automatically enabled and bound to idx 1
	vao.addAttribF(2, 2, 6 * sizeof(GL_FLOAT)); // tex coords, automatically enabled and bound to idx 2
	vao.addAttribF(3, 4, 8 * sizeof(GL_FLOAT), true, true); // vertex tangent, used for bump mapping

	// imgui inti
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window.getGLFWindow(), true);
	ImGui_ImplOpenGL3_Init("#version 450");

	// Set textures
	TexLoader texList;

	// Convert cube from triangle soup to indexed mesh
	std::vector<Vertex> vertices;
	vertices.resize(132);
	for (int i = 0; i < 132; i++)
	{
		vertices[i].pos = Vec3f{ element1Positions[3 * i], element1Positions[3 * i + 1], element1Positions[3 * i + 2] };
		vertices[i].normal = Vec3f{ element1Normals[3 * i], element1Normals[3 * i + 1], element1Normals[3 * i + 2] };
		vertices[i].texCoords = Vec2f{ element1UVs[2 * i], element1UVs[2 * i + 1] };
	}
	std::vector<unsigned int> indices;
	indices.resize(132);
	for (int i = 0; i < 132; i++)
	{
		indices[i] = i;
	}

	// Construct mesh
	Texture* albedoTex = texList.loadTexture("./assets/pbr/bamboo-wood-semigloss-albedo.png");
	Texture* metallicTex = texList.loadTexture("./assets/pbr/bamboo-wood-semigloss-metal.png", ColorSpace::LINEAR);
	Texture* roughnessTex = texList.loadTexture("./assets/pbr/bamboo-wood-semigloss-roughness.png", ColorSpace::LINEAR);
	Texture* ambientTex = texList.loadTexture("./assets/pbr/bamboo-wood-semigloss-roughness.png", ColorSpace::LINEAR);
	Texture* bumpTex = texList.loadTexture("./assets/pbr/bamboo-wood-semigloss-normal.png", ColorSpace::LINEAR);
	Material mat;
	mat.setPbrParams(
		albedoTex,
		metallicTex,
		roughnessTex,
		ambientTex
	);
	mat.normalMap = bumpTex;
	Mesh element1Mesh(vertices.data(), vertices.size());
	element1Mesh.addFaceGroup(indices.data(), indices.size(), mat);

	// Load mesh
	MeshLoader meshes(texList);
	Mesh *arenaMesh = meshes.loadMesh(arena);
	Mesh *roofMesh = meshes.loadMesh(roof);
	Mesh* floorMesh = meshes.loadMesh(thefloor);
	//Mesh *element1Mesh = meshes.loadMesh(element1);
	Mesh *element3Mesh = meshes.loadMesh(element3);
	Mesh *element4Mesh = meshes.loadMesh(element4);
	Mesh *oldboxMesh = meshes.loadMesh(oldbox);
	Mesh* swordMesh = meshes.loadMesh(sword);
	Mesh *boxWoodMesh = meshes.loadMesh(boxWood);
	Mesh *tableMesh = meshes.loadMesh(table);
	Mesh *chairMesh = meshes.loadMesh(chair);
	Mesh *targetMesh = meshes.loadMesh(target);
	Mesh *target2Mesh = meshes.loadMesh(target2);
	Mesh *lightMesh = meshes.loadMesh(light);
	Mesh* planeMesh = meshes.loadMesh(plane);
	Mesh* creeperheadMesh = meshes.loadMesh(creeperhead);
	Mesh* creeperbodyMesh = meshes.loadMesh(creeperbody);
	Mesh* creeperlegMesh = meshes.loadMesh(creeperleg);
	Mesh* lightbulbMesh = meshes.loadMesh(lightbulb);
	Mesh* crackMesh = meshes.loadMesh(crack);
	Texture* crackMaskTex = texList.loadTexture("./assets/mask.png");
	planeMesh->faceGroups[0].mat.maskTex = crackMaskTex;

	OGL_CHECKPOINT_ALWAYS();

	// Set lights
	DirLight dirLight{
		{0.0f,0.0f, -1.0f },//dir
		{1.0f, 1.0f, 0.8f}//colour
		};
	SpotLight spotLight1{
		{-20.0f, 12.8f, -8.f},
		{0.0f,-1.0f,0.0f},//must be normalized!
		{1.0f, 1.f, 1.f},
		degToRad(60.0f),
		degToRad(90.0f),
		50.0f };
	SpotLight spotLight2{
		{20.0f, 12.8f, -8.f},
		{0.0f,-1.0f,0.0f},//must be normalized!
		{1.0f, 1.f, 1.f},
		degToRad(60.0f),
		degToRad(90.0f),
		50.0f };
	SpotLight spotLight3{
		{-20.0f, 12.8f, -68.4f},
		{0.0f,-1.0f,0.0f},//must be normalized!
		{1.0f, 1.f, 1.f},
		degToRad(60.0f),
		degToRad(90.0f),
		50.0f };
	SpotLight spotLight4{
		{20.0f, 12.8f, -68.4f},
		{0.0f,-1.0f,0.0f},//must be normalized!
		{1.0f, 1.f, 1.f},
		degToRad(60.0f),
		degToRad(90.0f),
		50.0f };
	SpotLight spotLight5{
		{0.0f, 15.8f, -38.2f},
		{0.0f,-1.0f,0.0f},//must be normalized!
		{1.0f, 1.f, 1.f},
		degToRad(60.0f),
		degToRad(90.0f),
		50.0f };
	PointLight pointLight1{
		{-20.0f, 12.8f, -8.f},
		{1.0f, 1.f, 1.f},
		50.0f };
	PointLight pointLight2{
		{20.0f, 12.8f, -8.f},
		{0.8f, 0.8f, 0.8f},
		50.0f };
	PointLight pointLight3{
		{-20.0f, 12.8f, -68.4f},
		{0.8f, 0.8f, 0.8f},
		50.0f };
	PointLight pointLight4{
		{20.0f, 12.8f, -68.4f},
		{0.8f, 0.8f, 0.8f},
		50.0f };
	PointLight pointLight5{
		{0.0f, 15.8f, -38.2f},
		{0.8f, 0.8f, 0.8f},
		55.0f };
	PointLight pointLightSword{
		23.f, 4.f, -51.f,
		{1.0f, 1.f, 1.f},
		6.0f };
	SpotLight spotLightSword{
		22.5f, 4.f, -50.5f,
		normalize({1.0f,-0.3f,0.0f}),
		{0.9f, 0.9f, 0.9f},
		degToRad(40.0f),
		degToRad(60.0f),
		10.0f };
	LightManager lightManager;
	lightManager.setAmbientLight({0.2f, 0.2f, 0.2f}, prog);
	lightManager.setAmbientLight({ 0.09f, 0.09f, 0.09f }, progPbr);
	lightManager.setAmbientLight({ 0.09f, 0.09f, 0.09f }, progPbrBump);
	lightManager.addPointLight(pointLightSword,2);
	lightManager.addSpotLight(spotLight1,2);
	lightManager.addSpotLight(spotLight2,2);
	lightManager.addSpotLight(spotLight3,2);
	lightManager.addSpotLight(spotLight4,2);
	lightManager.addSpotLight(spotLight5,2);
	lightManager.addSpotLight(spotLightSword, 2);
	
	
	// mesterybox
	float boxR = 1.f; //rotation (cw or not cw)
	float angleM = 0.0f; //rotation angle
	float boxM = 1.f; //movement up or down
	float boxYpos = 0.f;
	// target 1
	float yPos1 = 0.0f;
	float xPos1 = 0.0f;
	float RightLeftT = 1.0f; //movement right or left
	// traget 1-2
	float yPos12 = 0.0f;
	// target 2
	float zPos2 = -69.0f;
	float xPos2 = 0.0f;
	float FrondBack = 0.0f; //movement front or back
	float RightLeft = -1.0f; //movement right or left
	// target 2 -2
	float zPos22 = -69.0f;
	float xPos22 = 0.0f;

	float targetSpeed = 10; // for all targets
	// creeper
	float angleC = 0.f;
	float partsC = 1.0f; // legs and head movements
	float zPosC = -10.0f;
	float xPosC = -5.0f;
	bool jump = false;
	float yPosC = 0.0f;
	float FrondBackC = 0.0f; //movement front or back
	float RightLeftC = -1.0f; //movement right or left
	float creeperdir = -0.5f; //where the creeper looks

	float time = 0;
	camera.setPosition({0.0f, 5.0f, -3.0f});
	state.updateClock();
	
	vao.bind(); // bind vertex array to make vertex data (pos, normals, uvs) available
	// Main loop
	while (!window.IsClosed())
	{
		if (!window.isMinimized())
		{
			//imgui 
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Game settings and Keybindings");
			ImGui::Text("Chose a level:");
			if (ImGui::Button("Easy"))
				targetSpeed = 10;
			if (ImGui::Button("Medium"))
				targetSpeed = 17.5;
			if (ImGui::Button("Hard"))
				targetSpeed = 25;
			ImGui::Text("This changes the speed of the targets");
			ImGui::Text("Default level is easy");

			ImGui::Text("\nMake the creeper jump:");
			if (ImGui::Button("Jump"))
				jump = true;

			ImGui::Text("\nKeybindings:");
			ImGui::Text("WASD + EQ - navigation");
			ImGui::Text("Shift - speed up");
			ImGui::Text("Ctrl - speed down");
			ImGui::Text("Space - mouse mode");
			ImGui::Text("F - fullscreen");
			ImGui::Text("P - screenshot");
			ImGui::Text("Enter - pause the targets");
			ImGui::End();


			state.updateClock();
			if (state.animationActive) {
				// Animatio

				//## target 1
				time += state.dt();
				yPos1 = sin(time) * 3;
				xPos1 += state.dt() * targetSpeed * RightLeftT;
				// RightLeft movement
				if (xPos1 > 22.2f) { RightLeftT = -1; }
				if (xPos1 < -27.2f) { RightLeftT = 1; }
				//## target 1-2
				yPos12 = cos(time) * 4;

				//## target 2
				zPos2 += state.dt() * targetSpeed * FrondBack;
				xPos2 += state.dt() * targetSpeed * RightLeft;
				zPos22 += state.dt() * targetSpeed * -FrondBack;
				xPos22 += state.dt() * targetSpeed * -RightLeft;
				// squared movement
				if (zPos2 < -73.0f) { RightLeft = 1; FrondBack = 0; }
				if (zPos2 > -69.0f) { RightLeft = -1; FrondBack = 0; }
				if (xPos2 < -18.0f && !(zPos2 < -73.0f)) { FrondBack = -1; RightLeft = 0; }
				if (xPos2 > 18.f && !(zPos2 > -69.0f)) { FrondBack = 1; RightLeft = 0; }
			}
			//## mesterybox (sword)
			angleM += state.dt() * kPi_ * 0.1f * boxR;
			if (angleM >= 0.3f) { boxR = -1; }
			else if (angleM <= -0.3f) { boxR = 1; }
			boxYpos += state.dt() * 0.7 * boxM;
			if (boxYpos > 1.2f) { boxM = -1; }
			else if (boxYpos < 0.3f) { boxM = 1.0f; }

			//## Creeper
			// creeper jupm
			if (jump == true)
			{
				yPosC += 0.1;
				if (yPosC > 1.5)
					jump = false;
			}
			else if (yPosC > 0.0)
				yPosC -= 0.1;
			// head and legs animation
			angleC += state.dt() * kPi_ * 0.3f * partsC;
			if (angleC >= 0.3f) { partsC = -1; }
			else if (angleC <= -0.3f) { partsC = 1; }
			// move around
			zPosC += state.dt() * 7 * FrondBackC;
			xPosC += state.dt() * 7 * RightLeftC;
			// creeper path
			if (zPosC < -40.0f) { RightLeftC = 1; FrondBackC = 0; creeperdir = 0.5; }
			if (zPosC > -28.0f) { RightLeftC = -1; FrondBackC = 0; creeperdir = -0.5; }
			if (xPosC < -12.0f && !(zPosC < -40.0f)) { FrondBackC = -1; RightLeftC = 0; creeperdir = 1; }
			if (xPosC > 2.f && xPosC < 3.f && !(zPosC < -40.0f)) { FrondBackC = 1; RightLeftC = 0; creeperdir = 0; }
			if (zPosC > -10.0f && xPosC > 2.f && xPosC < 3.f) { RightLeftC = -1; FrondBackC = 0; creeperdir = -0.5; }
			if (xPosC > 15.f && !(zPosC > -28.0f)) { FrondBackC = 1; RightLeftC = 0; creeperdir = 0; }

			// Update: compute matrices
			// arena
			Mat44f model2world3 = make_translation({ 0.f, 0.f, 0.f }) * make_scaling(0.9f, 0.9f, 0.9f);
			float denom3 = 1.0f / 0.9f;
			Mat44f model2world3N = make_scaling(denom3, denom3, denom3); // S^-1
			// roof
			Mat44f model2worldroof = make_translation({ 0.f, 13.f, 0.f }) * make_scaling(0.9f, 0.9f, 0.9f);
			Mat44f model2worldroofN = model2world3N;
			// floor
			Mat44f model2worldfloor = make_translation({ 0.f, 0.f, 0.f }) * make_scaling(0.9f, 0.9f, 0.9f);
			Mat44f model2worldfloorN = model2world3N;
			// element 1
			Mat44f model2world4 = make_translation({ 17.f, 2.1f, -17.f }) * make_scaling(2.3f, 2.0f, 2.3f);
			float denom4 = 1.0f / 2.3f;
			Mat44f model2world4N = make_scaling(denom3, 1.0 / 2.0f, denom3); // S^-1
			// element3
			Mat44f model2world5 = make_translation({ -17.f, 0.f, -20.f });
			// element4
			Mat44f model2world6 = make_translation({ 15.f, 0.f, -35.f }) * make_scaling(1.5f, 1.5f, 1.5f);
			float denom = 1.0f / 1.5f;
			Mat44f model2world6N = make_scaling(denom, denom, denom); // S^-1
			// oldbox
			Mat44f model2worldoldboxMesh = make_translation({ 25.5f, 0.0f, -50.f }) * make_rotation_y(-1.57f) * make_scaling(6.f, 6.f, 6.f);
			float denom5 = 1.0f / 6.f;
			Mat44f model2worldoldboxMeshN = make_scaling(denom5, denom5, denom5) * make_rotation_y(-1.57f); // S^-1
			// sword
			Mat44f model2worldsword = make_translation({ 25.5f, boxYpos + 2.3f, -50.f }) * make_rotation_y(angleM - 1.5) * make_rotation_x(kPi_ * 0.3f) * make_scaling(0.07f, 0.07f, 0.07f);
			float denom51 = 1.0f / 0.07f;
			Mat44f model2worldswordN = make_scaling(denom51, denom51, denom51) * make_rotation_y(angleM - 1.5f) * make_rotation_x(kPi_ * 0.45f); // S^-1
			// boxWood 1
			Mat44f model2worldboxWood1Mesh = make_translation({ -27.f, 0.0f, -50.f }) * make_scaling(3.0f, 3.0f, 3.0f) * make_rotation_y(1.57f);
			float denom6 = 1.0f / 3.0f;
			Mat44f model2worldboxWoodMeshN = make_scaling(denom6, denom6, denom6) * make_rotation_y(1.57f); // S^-1
			// boxWood 2
			Mat44f model2worldboxWood2Mesh = make_translation({ -27.f, 0.0f, -46.f }) * make_scaling(3.0f, 3.0f, 3.0f) * make_rotation_y(1.57f);
			// boxWood 3
			Mat44f model2worldboxWood3Mesh = make_translation({ -27.f, 3.9f, -46.f }) * make_scaling(3.0f, 3.0f, 3.0f) * make_rotation_y(1.57f);
			// table
			Mat44f model2worldtableMesh = make_translation({ -25.f, 0, -60.f }) * make_scaling(0.007f, 0.007f, 0.007f);
			float denom7 = 1.0f / 0.007f;
			Mat44f model2worldtableMeshN = make_scaling(denom7, denom7, denom7); // S^-1
			// chair
			Mat44f model2worldchairMesh = make_translation({ -23.f, 0, -67.f }) * make_scaling(0.06f, 0.06f, 0.06f) * make_rotation_y(0.8f * kPi_);
			float denom8 = 1.0f / 0.06f;
			Mat44f model2worldchairMeshN = make_scaling(denom8, denom8, denom8) * make_scaling(0.1f, 0.1f, 0.1f) * make_rotation_y(0.8f * kPi_); // S^-1
			// chair 2
			Mat44f model2worldchair2 = make_translation({ -21.f, 0, -61.f }) * make_scaling(0.06f, 0.06f, 0.06f) * make_rotation_y(0.5f * kPi_);;
			float denom81 = 1.0f / 0.06f;
			Mat44f model2worldchair2N = make_scaling(denom81, denom81, denom81) * make_scaling(0.1f, 0.1f, 0.1f) * make_rotation_y(2.5f); // S^-1
			// taget 11
			Mat44f model2worldtarget = make_translation({ xPos1, yPos1 + 7, -76.4f }) * make_scaling(0.9f, 0.9f, 0.9f) * make_rotation_y(-1.57);
			float denom9 = 1.0f / 0.9f;
			Mat44f model2worldtargetN = make_scaling(denom9, denom9, denom9) * make_rotation_y(-1.57); // S^-1
			// taget 12
			Mat44f model2worldtarget12 = make_translation({ xPos1 + 5, yPos12 + 7, -76.4f }) * make_scaling(0.9f, 0.9f, 0.9f) * make_rotation_y(-1.57f);
			// taget2
			Mat44f model2worldtarget2 = make_translation({ xPos2, 0.f, zPos2 }) * make_scaling(0.7f, 0.7f, 0.7f);
			float denom10 = 1.0f / 0.7f;
			Mat44f model2worldtarget2N = make_scaling(denom10, denom10, denom10); // S^-1
			// taget2-2
			Mat44f model2worldtarget22 = make_translation({ xPos22, 0.f, zPos22 - 3.5f }) * make_scaling(0.7f, 0.7f, 0.7f);
			// light1
			Mat44f model2worldlight = make_translation({ -20.0f, 13.f, -8.f });
			// light2
			Mat44f model2worldlight2 = make_translation({ +20.0f, 13.f, -8.f });
			// light3
			Mat44f model2worldlight3 = make_translation({ -20.0f, 13.f, -68.4f });
			// light4
			Mat44f model2worldlight4 = make_translation({ +20.0f, 13.f, -68.4f });
			// light5
			Mat44f model2worldlight5 = make_translation({ 0.0f, 16.f, -38.2f });
			// lightbulb1
			Mat44f flipUp = make_rotation_z(kPi_);
			Mat44f model2worldlightbulb1 = make_translation({ -20.0f, 13.2f, -8.f }) * flipUp * make_scaling(0.05f, 0.05f, 0.05f);
			float denom1light = 1.f / 0.05f;
			Mat44f model2worldlightbulbN = make_scaling(denom1light, denom1light, denom1light) * flipUp;
			// lightbulb2
			Mat44f model2worldlightbulb2 = make_translation({ +20.0f, 13.2f, -8.f }) * flipUp * make_scaling(0.05f, 0.05f, 0.05f);
			// lightbulb3
			Mat44f model2worldlightbulb3 = make_translation({ -20.0f, 13.2f, -68.4f }) * flipUp * make_scaling(0.05f, 0.05f, 0.05f);
			// lightbulb4
			Mat44f model2worldlightbulb4 = make_translation({ +20.0f, 13.2f, -68.4f }) * flipUp * make_scaling(0.05f, 0.05f, 0.05f);
			// lightbulb5
			Mat44f model2worldlightbulb5 = make_translation({ 0.0f, 16.2f, -38.2f }) * flipUp * make_scaling(0.05f, 0.05f, 0.05f);
			// ################################################################# Creeper
			// creeperbody
			Mat44f model2worldcreeperbody = make_translation({ xPosC, 1.6f+yPosC, zPosC }) * make_scaling(2.f, 2.f, 2.f) * make_rotation_y(creeperdir * kPi_);
			float denom11 = 1.0f / 2.f;
			Mat44f model2worldcreeperbodyN = make_scaling(denom11, denom11, denom11) * make_rotation_y(creeperdir * kPi_); // S^-1
			// creeperhead
			Mat44f model2worldcreeperhead = model2worldcreeperbody * make_translation({ 0.0f, 1.4f, 0.0f }) * make_rotation_y(kPi_ + angleC * 0.3f);
			Mat44f model2worldcreeperheadN = model2worldcreeperbodyN * make_rotation_y(kPi_ + angleC * 0.3f); // S^-1
			//// legs
			// creeperleg FrontLeft
			Mat44f model2worldcreeperlegFL = model2worldcreeperbody * make_translation({ 0.25f, 0.f, 0.2f }) * make_rotation_x(angleC);
			Mat44f model2worldcreeperlegFLN = model2worldcreeperbodyN * make_rotation_x(angleC); // S^-1
			// creeperleg FrontRight
			Mat44f model2worldcreeperlegFR = model2worldcreeperbody * make_translation({ -0.25f, 0.f, 0.2f }) * make_rotation_x(-angleC);
			Mat44f model2worldcreeperlegFRN = model2worldcreeperbodyN * make_rotation_x(-angleC); // S^-1
			// creeperleg backLeft
			Mat44f model2worldcreeperlegBL = model2worldcreeperbody * make_translation({ 0.25f, 0.f, -0.2f }) * make_rotation_x(-angleC) * make_rotation_y(kPi_);
			Mat44f model2worldcreeperlegBLN = model2worldcreeperbodyN * make_rotation_x(-angleC); // S^-1
			// creeperleg backLeft
			Mat44f model2worldcreeperlegBR = model2worldcreeperbody * make_translation({ -0.25f, 0.f, -0.2f }) * make_rotation_x(angleC) * make_rotation_y(kPi_);
			Mat44f model2worldcreeperlegBRN = model2worldcreeperbodyN * make_rotation_x(angleC); // S^-1
		
			//glass
			Mat44f model2worldglass = make_translation({ -7.7f,6.5f,-25.f });

			//crack on wall
			Mat44f model2worldcrack = make_translation({ 28.5f, 5.0f, -25.f })*make_scaling(3.0f,3.0f,3.0f);
			float denomCrack = 1.0 / 3.0f;
			Mat44f model2worldcraskN = make_scaling(denomCrack, denomCrack, denomCrack);

			Mat44f world2camera = camera.getViewMatrix();
			Mat44f projection = make_perspective_projection(
				camera.getVerticalFOV(),
				window.getAspectRatio(),
				0.1f, 100.0f);
			Mat44f viewProj = projection * world2camera;

			// Draw scene
			OGL_CHECKPOINT_DEBUG();

			// TODO: draw frame
			//  Clear color buffer to specified clear color
			glClear(GL_COLOR_BUFFER_BIT);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			
			// We want to draw with our program..
			{ // arena
				MeshUniforms uniforms{};
				uniforms.modelMat = &model2world3;
				uniforms.modelMatN = &model2world3N;
				uniforms.viewProjMat = &viewProj;
				arenaMesh->draw(state, vao, uniforms);
			}
			{ // roof
				MeshUniforms uniforms{};
				uniforms.modelMat = &model2worldroof;
				uniforms.modelMatN = &model2worldroofN;
				uniforms.viewProjMat = &viewProj;
				roofMesh->draw(state, vao, uniforms);
			}
			
			{ // floor
				MeshUniforms uniforms{};
				uniforms.modelMat = &model2worldfloor;
				uniforms.modelMatN = &model2worldfloorN;
				uniforms.viewProjMat = &viewProj;
				floorMesh->draw(state, vao, uniforms);
			}
			
			{ // elemnet 1
				MeshUniforms uniforms{};
				uniforms.modelMat = &model2world4;
				uniforms.modelMatN = &model2world4N;
				uniforms.viewProjMat = &viewProj;
				element1Mesh.draw(state, vao, uniforms);
			}
			{ // elemnet 3
				MeshUniforms uniforms{};
				uniforms.modelMat = &model2world5;
				uniforms.modelMatN = &model2world5;
				uniforms.viewProjMat = &viewProj;
				element3Mesh->draw(state, vao, uniforms);
			}
			{ // elemnet 4
				MeshUniforms uniforms{};
				uniforms.modelMat = &model2world6;
				uniforms.modelMatN = &model2world6N;
				uniforms.viewProjMat = &viewProj;
				element4Mesh->draw(state, vao, uniforms);
			}
			
			{ // old box
				MeshUniforms uniforms{};
				uniforms.modelMat = &model2worldoldboxMesh;
				uniforms.modelMatN = &model2worldoldboxMesh;
				uniforms.viewProjMat = &viewProj;
				oldboxMesh->draw(state, vao, uniforms);
			}
			{ // sword
				MeshUniforms uniforms{};
				uniforms.modelMat = &model2worldsword;
				uniforms.modelMatN = &model2worldswordN;
				uniforms.viewProjMat = &viewProj;
				swordMesh->draw(state, vao, uniforms);
			}
			{ // wooden box 1
				MeshUniforms uniforms{};
				uniforms.modelMat = &model2worldboxWood1Mesh;
				uniforms.modelMatN = &model2worldboxWoodMeshN;
				uniforms.viewProjMat = &viewProj;
				boxWoodMesh->draw(state, vao, uniforms);
			}
			{ // wooden box 2
				MeshUniforms uniforms{};
				uniforms.modelMat = &model2worldboxWood2Mesh;
				uniforms.modelMatN = &model2worldboxWoodMeshN;
				uniforms.viewProjMat = &viewProj;
				boxWoodMesh->draw(state, vao, uniforms);
			}
			{ // wooden box 3
				MeshUniforms uniforms{};
				uniforms.modelMat = &model2worldboxWood3Mesh;
				uniforms.modelMatN = &model2worldboxWoodMeshN;
				uniforms.viewProjMat = &viewProj;
				boxWoodMesh->draw(state, vao, uniforms);
			}
			{ // table
				MeshUniforms uniforms{};
				uniforms.modelMat = &model2worldtableMesh;
				uniforms.modelMatN = &model2worldtableMesh;
				uniforms.viewProjMat = &viewProj;
				tableMesh->draw(state, vao, uniforms);
			}
			
			{ // chair
				MeshUniforms uniforms{};
				uniforms.modelMat = &model2worldchairMesh;
				uniforms.modelMatN = &model2worldchairMeshN;
				uniforms.viewProjMat = &viewProj;
				chairMesh->draw(state, vao, uniforms);
			}
			{ // chair
				MeshUniforms uniforms{};
				uniforms.modelMat = &model2worldchair2;
				uniforms.modelMatN = &model2worldchair2N;
				uniforms.viewProjMat = &viewProj;
				chairMesh->draw(state, vao, uniforms);
			}
			
			{ // target 1
				MeshUniforms uniforms{};
				uniforms.modelMat = &model2worldtarget;
				uniforms.modelMatN = &model2worldtargetN;
				uniforms.viewProjMat = &viewProj;
				targetMesh->draw(state, vao, uniforms);
			}
			{ // target 1-2
				MeshUniforms uniforms{};
				uniforms.modelMat = &model2worldtarget12;
				uniforms.modelMatN = &model2worldtargetN;
				uniforms.viewProjMat = &viewProj;
				targetMesh->draw(state, vao, uniforms);
			}
			{ // target 2
				MeshUniforms uniforms{};
				uniforms.modelMat = &model2worldtarget2;
				uniforms.modelMatN = &model2worldtarget2N;
				uniforms.viewProjMat = &viewProj;
				target2Mesh->draw(state, vao, uniforms);
			}
			{ // target 2-2
				MeshUniforms uniforms{};
				uniforms.modelMat = &model2worldtarget22;
				uniforms.modelMatN = &model2worldtarget2N;
				uniforms.viewProjMat = &viewProj;
				target2Mesh->draw(state, vao, uniforms);
			}
			{ // light 1
				MeshUniforms uniforms{};
				uniforms.modelMat = &model2worldlight;
				uniforms.modelMatN = &model2worldlight;
				uniforms.viewProjMat = &viewProj;
				lightMesh->draw(state, vao, uniforms);
			}
			{ // light 2
				MeshUniforms uniforms{};
				uniforms.modelMat = &model2worldlight2;
				uniforms.modelMatN = &model2worldlight2;
				uniforms.viewProjMat = &viewProj;
				lightMesh->draw(state, vao, uniforms);
			}
			{ // light 3
				MeshUniforms uniforms{};
				uniforms.modelMat = &model2worldlight3;
				uniforms.modelMatN = &model2worldlight3;
				uniforms.viewProjMat = &viewProj;
				lightMesh->draw(state, vao, uniforms);
			}
			{ // light 4
				MeshUniforms uniforms{};
				uniforms.modelMat = &model2worldlight4;
				uniforms.modelMatN = &model2worldlight4;
				uniforms.viewProjMat = &viewProj;
				lightMesh->draw(state, vao, uniforms);
			}
			{ // light 5
				MeshUniforms uniforms{};
				uniforms.modelMat = &model2worldlight5;
				uniforms.modelMatN = &model2worldlight5;
				uniforms.viewProjMat = &viewProj;
				lightMesh->draw(state, vao, uniforms);
			}
			{ // creeperbody
				MeshUniforms uniforms{};
				uniforms.modelMat = &model2worldcreeperbody;
				uniforms.modelMatN = &model2worldcreeperbodyN;
				uniforms.viewProjMat = &viewProj;
				creeperbodyMesh->draw(state, vao, uniforms);
			}
			{ // creeperhead
				MeshUniforms uniforms{};
				uniforms.modelMat = &model2worldcreeperhead;
				uniforms.modelMatN = &model2worldcreeperheadN;
				uniforms.viewProjMat = &viewProj;
				creeperheadMesh->draw(state, vao, uniforms);
			}
			{ // creeperleg FL
				MeshUniforms uniforms{};
				uniforms.modelMat = &model2worldcreeperlegFL;
				uniforms.modelMatN = &model2worldcreeperlegFLN;
				uniforms.viewProjMat = &viewProj;
				creeperlegMesh->draw(state, vao, uniforms);
			}
			{ // creeperleg FR
				MeshUniforms uniforms{};
				uniforms.modelMat = &model2worldcreeperlegFR;
				uniforms.modelMatN = &model2worldcreeperlegFRN;
				uniforms.viewProjMat = &viewProj;
				creeperlegMesh->draw(state, vao, uniforms);
			}
			{ // creeperleg BL
				MeshUniforms uniforms{};
				uniforms.modelMat = &model2worldcreeperlegBL;
				uniforms.modelMatN = &model2worldcreeperlegBLN;
				uniforms.viewProjMat = &viewProj;
				creeperlegMesh->draw(state, vao, uniforms);
			}
			{ // creeperleg BR
				MeshUniforms uniforms{};
				uniforms.modelMat = &model2worldcreeperlegBR;
				uniforms.modelMatN = &model2worldcreeperlegBRN;
				uniforms.viewProjMat = &viewProj;
				creeperlegMesh->draw(state, vao, uniforms);
			}
			{// plane
				MeshUniforms uniforms{};
				uniforms.modelMat = &model2worldglass;
				uniforms.modelMatN = &model2worldglass;
				uniforms.viewProjMat = &viewProj;
				planeMesh->draw(state, vao, uniforms);
			}
			{// libhtbulb 1
				MeshUniforms uniforms{};
				uniforms.modelMat = &model2worldlightbulb1;
				uniforms.modelMatN = &model2worldlightbulbN;
				uniforms.viewProjMat = &viewProj;
				lightbulbMesh->draw(state, vao, uniforms);
			}
			{// libhtbulb 2
				MeshUniforms uniforms{};
				uniforms.modelMat = &model2worldlightbulb2;
				uniforms.modelMatN = &model2worldlightbulbN;
				uniforms.viewProjMat = &viewProj;
				lightbulbMesh->draw(state, vao, uniforms);
			}
			{// libhtbulb 3
				MeshUniforms uniforms{};
				uniforms.modelMat = &model2worldlightbulb3;
				uniforms.modelMatN = &model2worldlightbulbN;
				uniforms.viewProjMat = &viewProj;
				lightbulbMesh->draw(state, vao, uniforms);
			}
			{// libhtbulb 4
				MeshUniforms uniforms{};
				uniforms.modelMat = &model2worldlightbulb4;
				uniforms.modelMatN = &model2worldlightbulbN;
				uniforms.viewProjMat = &viewProj;
				lightbulbMesh->draw(state, vao, uniforms);
			}
			{// libhtbulb 5
				MeshUniforms uniforms{};
				uniforms.modelMat = &model2worldlightbulb5;
				uniforms.modelMatN = &model2worldlightbulbN;
				uniforms.viewProjMat = &viewProj;
				lightbulbMesh->draw(state, vao, uniforms);
			}

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			OGL_CHECKPOINT_DEBUG();
		}

		window.updateWindow();
	}
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	// Cleanup.
	// TODO ensure that all OpenGL objects are freed BEFORE the context is ternimated!
	return 0;
}
catch (std::exception const &eErr)
{
	std::fprintf(stderr, "Top-level Exception (%s):\n", typeid(eErr).name());
	std::fprintf(stderr, "%s\n", eErr.what());
	std::fprintf(stderr, "Bye.\n");
	return 1;
}
