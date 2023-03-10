#ifndef CUBE_HPP_6874B39C_112D_4D34_BD85_AB81A730955B
#define CUBE_HPP_6874B39C_112D_4D34_BD85_AB81A730955B

constexpr float element1UVs[] = {
	// right
	0.0f, 0.0f,
	1.0f, 0.0f,
	0.0f, 1.0f,

	0.0f, 1.0f,
	1.0f, 0.0f,
	1.0f, 1.0f,

	0.0f, 1.0f,
	1.0f, 1.0f,
	0.0f, 0.0f,

	0.0f, 0.0f,
	1.0f, 1.0f,
	1.0f, 0.0f,

	0.0f, 0.0f,
	1.0f, 0.0f,
	0.0f, 1.0f,

	0.0f, 1.0f,
	1.0f, 0.0f,
	1.0f, 1.0f,

	0.0f, 0.0f,
	1.0f, 0.0f,
	0.0f, 1.0f,

	0.0f, 1.0f,
	1.0f, 0.0f,
	1.0f, 1.0f,

	1.0f, 0.0f,
	1.0f, 1.0f,
	0.0f, 0.0f,

	0.0f, 0.0f,
	1.0f, 1.0f,
	0.0f, 1.0f,

	// front
	0.0f, 0.0f,
	1.0f, 0.0f,
	0.0f, 1.0f,

	0.0f, 1.0f,
	1.0f, 0.0f,
	1.0f, 1.0f,

	0.0f, 1.0f,
	1.0f, 1.0f,
	0.0f, 0.0f,

	0.0f, 0.0f,
	1.0f, 1.0f,
	1.0f, 0.0f,

	0.0f, 0.0f,
	1.0f, 0.0f,
	0.0f, 1.0f,

	0.0f, 1.0f,
	1.0f, 0.0f,
	1.0f, 1.0f,

	1.0f, 0.0f,
	1.0f, 1.0f,
	0.0f, 0.0f,

	0.0f, 0.0f,
	1.0f, 1.0f,
	0.0f, 1.0f,

	0.0f, 0.0f,
	0.0f, 1.0f,
	1.0f, 0.0f,

	1.0f, 0.0f,
	0.0f, 1.0f,
	1.0f, 1.0f,

	// left
	0.0f, 0.0f,
	1.0f, 0.0f,
	0.0f, 1.0f,

	0.0f, 1.0f,
	1.0f, 0.0f,
	1.0f, 1.0f,

	0.0f, 1.0f,
	1.0f, 1.0f,
	0.0f, 0.0f,

	0.0f, 0.0f,
	1.0f, 1.0f,
	1.0f, 0.0f,

	0.0f, 0.0f,
	1.0f, 0.0f,
	0.0f, 1.0f,

	0.0f, 1.0f,
	1.0f, 0.0f,
	1.0f, 1.0f,

	0.0f, 0.0f,
	1.0f, 0.0f,
	0.0f, 1.0f,

	0.0f, 1.0f,
	1.0f, 0.0f,
	1.0f, 1.0f,

	0.0f, 0.0f,
	0.0f, 1.0f,
	1.0f, 0.0f,

	1.0f, 0.0f,
	0.0f, 1.0f,
	1.0f, 1.0f,

	// back
	0.0f, 1.0f,
	1.0f, 1.0f,
	0.0f, 0.0f,

	0.0f, 0.0f,
	1.0f, 1.0f,
	1.0f, 0.0f,

	0.0f, 0.0f,
	1.0f, 0.0f,
	0.0f, 1.0f,

	0.0f, 1.0f,
	1.0f, 0.0f,
	1.0f, 1.0f,

	0.0f, 0.0f,
	1.0f, 0.0f,
	0.0f, 1.0f,

	0.0f, 1.0f,
	1.0f, 0.0f,
	1.0f, 1.0f,

	1.0f, 0.0f,
	1.0f, 1.0f,
	0.0f, 0.0f,

	0.0f, 0.0f,
	1.0f, 1.0f,
	0.0f, 1.0f,

	0.0f, 0.0f,
	0.0f, 1.0f,
	1.0f, 0.0f,

	1.0f, 0.0f,
	0.0f, 1.0f,
	1.0f, 1.0f,

	//top and bottom
	0.0f, 0.0f,
	1.0f, 0.0f,
	0.0f, 1.0f,

	0.0f, 1.0f,
	1.0f, 0.0f,
	1.0f, 1.0f,

	0.0f, 0.0f,
	1.0f, 0.0f,
	0.0f, 1.0f,

	0.0f, 1.0f,
	1.0f, 0.0f,
	1.0f, 1.0f,
};

constexpr float const element1Normals[] = {
	// right
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,

	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,

	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,

	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,

	0.0f, 0.0f, -1.0f,
	0.0f, 0.0f, -1.0f,
	0.0f, 0.0f, -1.0f,

	0.0f, 0.0f, -1.0f,
	0.0f, 0.0f, -1.0f,
	0.0f, 0.0f, -1.0f,

	0.0f, -1.0f, 0.0f,
	0.0f, -1.0f, 0.0f,
	0.0f, -1.0f, 0.0f,

	0.0f, -1.0f, 0.0f,
	0.0f, -1.0f, 0.0f,
	0.0f, -1.0f, 0.0f,

	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,

	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,

	// front
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,

	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,

	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,

	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,

	0.0f, -1.0f, 0.0f,
	0.0f, -1.0f, 0.0f,
	0.0f, -1.0f, 0.0f,

	0.0f, -1.0f, 0.0f,
	0.0f, -1.0f, 0.0f,
	0.0f, -1.0f, 0.0f,

	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,

	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,

	-1.0f, 0.0f, 0.0f,
	-1.0f, 0.0f, 0.0f,
	-1.0f, 0.0f, 0.0f,

	-1.0f, 0.0f, 0.0f,
	-1.0f, 0.0f, 0.0f,
	-1.0f, 0.0f, 0.0f,

	// left
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,

	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,

	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,

	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,

	0.0f, 0.0f, -1.0f,
	0.0f, 0.0f, -1.0f,
	0.0f, 0.0f, -1.0f,

	0.0f, 0.0f, -1.0f,
	0.0f, 0.0f, -1.0f,
	0.0f, 0.0f, -1.0f,

	0.0f, -1.0f, 0.0f,
	0.0f, -1.0f, 0.0f,
	0.0f, -1.0f, 0.0f,

	0.0f, -1.0f, 0.0f,
	0.0f, -1.0f, 0.0f,
	0.0f, -1.0f, 0.0f,

	-1.0f, 0.0f, 0.0f,
	-1.0f, 0.0f, 0.0f,
	-1.0f, 0.0f, 0.0f,

	-1.0f, 0.0f, 0.0f,
	-1.0f, 0.0f, 0.0f,
	-1.0f, 0.0f, 0.0f,

	// back

	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,

	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,

	0.0f, 0.0f, -1.0f,
	0.0f, 0.0f, -1.0f,
	0.0f, 0.0f, -1.0f,

	0.0f, 0.0f, -1.0f,
	0.0f, 0.0f, -1.0f,
	0.0f, 0.0f, -1.0f,

	0.0f, -1.0f, 0.0f,
	0.0f, -1.0f, 0.0f,
	0.0f, -1.0f, 0.0f,

	0.0f, -1.0f, 0.0f,
	0.0f, -1.0f, 0.0f,
	0.0f, -1.0f, 0.0f,

	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,

	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,

	-1.0f, 0.0f, 0.0f,
	-1.0f, 0.0f, 0.0f,
	-1.0f, 0.0f, 0.0f,

	-1.0f, 0.0f, 0.0f,
	-1.0f, 0.0f, 0.0f,
	-1.0f, 0.0f, 0.0f,

	// top and bottom
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,

	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,


	0.0f, -1.0f, 0.0f,
	0.0f, -1.0f, 0.0f,
	0.0f, -1.0f, 0.0f,

	0.0f, -1.0f, 0.0f,
	0.0f, -1.0f, 0.0f,
	0.0f, -1.0f, 0.0f
};

constexpr float const element1Positions[] = {
	//right
	0.25f, -1.0f, 0.25f, //front
	2.25f, -1.0f, 0.25f,
	0.25f, 1.0f, 0.25f,

	0.25f, 1.0f, 0.25f,
	2.25f, -1.0f, 0.25f,
	2.25f, 1.0f, 0.25f,

	0.25f, 1.0f, 0.25f, //top
	2.25f, 1.0f, 0.25f,
	0.25f, 1.0f, -0.25f,

	0.25f, 1.0f, -0.25f,
	2.25, 1.0f, 0.25f,
	2.25, 1.0f, -0.25f,

	0.25f, 1.0f, -0.25f, //back
	2.25, 1.0f, -0.25f,
	0.25f, -1.0f, -0.25f,

	0.25f, -1.0f, -0.25f,
	2.25, 1.0f, -0.25f,
	2.25, -1.0f, -0.25f,

	0.25f, -1.0f, -0.25f, //bottom
	2.25, -1.0f, -0.25f,
	0.25f, -1.0f, 0.25f,

	0.25f, -1.0f, 0.25f,
	2.25, -1.0f, -0.25f,
	2.25, -1.0f, 0.25f,

	2.25, -1.0f, 0.25f, //rightside
	2.25, -1.0f, -0.25f,
	2.25, 1.0f, 0.25f,

	2.25, 1.0f, 0.25f,
	2.25, -1.0f, -0.25f,
	2.25, 1.0f, -0.25f,

	//front
	-0.25f, -1.0f, 2.25, //front
	0.25f, -1.0f, 2.25,
	-0.25f, 1.0f, 2.25,

	-0.25f, 1.0f, 2.25,
	0.25f, -1.0f, 2.25,
	0.25f, 1.0f, 2.25,

	-0.25f, 1.0f, 2.25, //top
	0.25f, 1.0f, 2.25,
	-0.25f, 1.0f, 0.25,

	-0.25f, 1.0f, 0.25,
	0.25f, 1.0f, 2.25,
	0.25f, 1.0f, 0.25,

	-0.25f, -1.0f, 0.25, //bottom
	0.25f, -1.0f, 0.25,
	-0.25f, -1.0f, 2.25,

	-0.25f, -1.0f, 2.25,
	0.25f, -1.0f, 0.25,
	0.25f, -1.0f, 2.25,

	0.25f, -1.0f, 2.25, //rightside
	0.25f, -1.0f, 0.25,
	0.25f, 1.0f, 2.25,

	0.25f, 1.0f, 2.25,
	0.25f, -1.0f, 0.25,
	0.25f, 1.0f, 0.25,

	-0.25f, -1.0f, 0.25, //leftside
	-0.25f, -1.0f, 2.25,
	-0.25f, 1.0f, 0.25,

	-0.25f, 1.0f, 0.25,
	-0.25f, -1.0f, 2.25,
	-0.25f, 1.0f, 2.25,

	// left
	-2.25f, -1.0f, 0.25f, //front
	-0.25, -1.0f, 0.25f,
	-2.25f, 1.0f, 0.25f,

	-2.25f, 1.0f, 0.25f,
	-0.25f, -1.0f, 0.25f,
	-0.25f, 1.0f, 0.25f,

	-2.25f, 1.0f, 0.25f, //top
	-0.25f, 1.0f, 0.25f,
	-2.25f, 1.0f, -0.25f,

	-2.25f, 1.0f, -0.25f,
	-0.25, 1.0f, 0.25f,
	-0.25, 1.0f, -0.25f,

	-2.25f, 1.0f, -0.25f, //back
	-0.25, 1.0f, -0.25f,
	-2.25f, -1.0f, -0.25f,

	-2.25f, -1.0f, -0.25f,
	-0.25, 1.0f, -0.25f,
	-0.25, -1.0f, -0.25f,

	-2.25f, -1.0f, -0.25f, //bottom
	-0.25, -1.0f, -0.25f,
	-2.25f, -1.0f, 0.25f,

	-2.25f, -1.0f, 0.25f,
	-0.25, -1.0f, -0.25f,
	-0.25, -1.0f, 0.25f,

	-2.25, -1.0f, -0.25f, //leftside
	-2.25, -1.0f, 0.25f,
	-2.25, 1.0f, -0.25f,

	-2.25, 1.0f, -0.25f,
	-2.25, -1.0f, 0.25f,
	-2.25, 1.0f, 0.25f,

	//back
	-0.25f, 1.0f, -0.25f, //top
	0.25f, 1.0f, -0.25f,
	-0.25f, 1.0f, -2.25f,

	-0.25f, 1.0f, -2.25f,
	0.25f, 1.0f, -0.25f,
	0.25f, 1.0f, -2.25f,

	-0.25f, 1.0f, -2.25f, //back
	0.25f, 1.0f, -2.25f,
	-0.25f, -1.0f, -2.25f,

	-0.25f, -1.0f, -2.25f,
	0.25f, 1.0f, -2.25f,
	0.25f, -1.0f, -2.25f,

	-0.25f, -1.0f, -2.25f, //bottom
	0.25f, -1.0f, -2.25f,
	-0.25f, -1.0f, -0.25f,

	-0.25f, -1.0f, -0.25f,
	0.25f, -1.0f, -2.25f,
	0.25f, -1.0f, -0.25f,

	0.25f, -1.0f, -0.25f, //rightside
	0.25f, -1.0f, -2.25f,
	0.25f, 1.0f, -0.25f,

	0.25f, 1.0f, -0.25f,
	0.25f, -1.0f, -2.25f,
	0.25f, 1.0f, -2.25f,

	-0.25f, -1.0f, -2.25f, //leftside
	-0.25f, -1.0f, -0.25f,
	-0.25f, 1.0f, -2.25f,

	-0.25f, 1.0f, -2.25f,
	-0.25f, -1.0f, -0.25f,
	-0.25f, 1.0f, -0.25f,

	// top and bootom
	-0.25f, 1.0f, 0.25f, //top
	0.25f, 1.0f, 0.25f,
	-0.25f, 1.0f, -0.25f,

	-0.25f, 1.0f, -0.25f,
	0.25, 1.0f, 0.25f,
	0.25, 1.0f, -0.25f,

	-0.25f, -1.0f, -0.25f, //bottom
	0.25, -1.0f, -0.25f,
	-0.25f, -1.0f, 0.25f,

	-0.25f, -1.0f, 0.25f,
	0.25, -1.0f, -0.25f,
	0.25, -1.0f, 0.25f
};

#endif // CUBE_HPP_6874B39C_112D_4D34_BD85_AB81A730955B
