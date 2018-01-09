#pragma once
#include <QtOpenGL/qgl.h>
#include <vector>
using std::vector;
class Skybox
{
public:
	Skybox();
	vector<GLuint> texID;
	void render();
};