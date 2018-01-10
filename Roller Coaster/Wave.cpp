#include "Wave.h"
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QVarLengthArray>
#include <cmath>
#include <QtOpenGL/QtOpenGL>

Wave::Wave(int width, int length, int baseX, int baseY, int baseZ, int maxiHeigth, int interval, GLuint texID) :_width(width), _length(length), _baseX(baseX), _baseY(baseY), _baseZ(baseZ),_maxiHeigth(maxiHeigth), _interval(interval),_texID(texID) {
	this->_t = 0;
}

void Wave::render() {
	int widthCount = this->_width / this->_interval, lengthCount = this->_length / this->_interval;
	int nowW = this->_baseX - this->_width / 2,nowL=this->_baseZ-this->_length/2,nowH=this->_baseY;
	glEnable(GL_TEXTURE_2D);
	for (int l = 1; l <= lengthCount; ++l) {
		for (int w = 1; w <= widthCount; ++w) {
			float innerW = nowW+(w-1)*this->_interval, innerL= nowL + (l - 1)*this->_interval;
			glBindTexture(GL_TEXTURE_2D, this->_texID);
			glBegin(GL_QUADS);
			glNormal3f(0.0f, 1.0f, 0.0f);
			glTexCoord2d(1.0f, 0.0f);	glVertex3f(innerW + this->_interval, this->_maxiHeigth*sin(this->_t + w + l-1)+nowH, innerL);	// Top Right Of The Quad (Top)
			glTexCoord2d(1.0f, 1.0f);	glVertex3f(innerW, this->_maxiHeigth*sin(this->_t + w - 1 +l-1) + nowH, innerL);	// Top Left Of The Quad (Top)
			glTexCoord2d(0.0, 1.0f);	glVertex3f(innerW, this->_maxiHeigth*sin(this->_t + w - 1+l) + nowH, innerL + this->_interval);	// Bottom Left Of The Quad (Top)
			glTexCoord2d(0.0, 0.0f);	glVertex3f(innerW + this->_interval, this->_maxiHeigth*sin(this->_t + w+l) + nowH, innerL + this->_interval);	// Bottom Right Of The Quad (Top)
			glEnd();
		}
	}
	glDisable(GL_TEXTURE_2D);
	this->_t = (this->_t + 0.0314);
	this->_t = fabs(this->_t - 3.14 * 2) < 0.0000001 ? 0 : this->_t;
}