#pragma once
#include <QtOpenGL/qgl.h>
class Wave {
public:
	Wave(int width, int length, int baseX, int baseY, int baseZ, int maxiHeigth, int interval, GLuint texID);
	void render();
private:
	int _width, _length,_baseX,_baseY,_baseZ,_maxiHeigth, _interval;
	float _t;
	GLuint _texID;
};