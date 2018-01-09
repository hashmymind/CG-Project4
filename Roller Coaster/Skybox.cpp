#include "Skybox.h"
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QVarLengthArray>

#include <QtOpenGL/QtOpenGL>
GLint width = 2000,height = 2000,length = 2000;
Skybox::Skybox() {
	// init texID
	this->texID.resize(6);
	
}

void Skybox::render() {
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_MULTISAMPLE);
	glBindTexture(GL_TEXTURE_2D, this->texID[0]);
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 1.0f, 0.0f);
	glTexCoord2d(1.0f, 0.0f);	glVertex3f(width, height, -length);	// Top Right Of The Quad (Top)
	glTexCoord2d(1.0f, 1.0f);	glVertex3f(-width, height, -length);	// Top Left Of The Quad (Top)
	glTexCoord2d(0.0, 1.0f);	glVertex3f(-width, height, length);	// Bottom Left Of The Quad (Top)
	glTexCoord2d(0.0, 0.0f);	glVertex3f(width, height, length);	// Bottom Right Of The Quad (Top)
	glEnd();

	glBindTexture(GL_TEXTURE_2D, this->texID[1]);
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 1.0f, 0.0f);
	glTexCoord2d(1.0f, 0.0f);	glVertex3f(width, -height, -length);	// Top Right Of The Quad (Top)
	glTexCoord2d(1.0f, 1.0f);	glVertex3f(-width, -height, -length);	// Top Left Of The Quad (Top)
	glTexCoord2d(0.0, 1.0f);	glVertex3f(-width, -height, length);	// Bottom Left Of The Quad (Top)
	glTexCoord2d(0.0, 0.0f);	glVertex3f(width, -height, length);	// Bottom Right Of The Quad (Top)
	glEnd();


	glBindTexture(GL_TEXTURE_2D, this->texID[2]);
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 0.0f, -1.0f);
	glTexCoord2d(0.0f, 0.0f);	glVertex3f(width, height, length);	// Top Right Of The Quad (Front)
	glTexCoord2d(1.0f, 0.0f);	glVertex3f(-width, height, length);	// Top Left Of The Quad (Front)
	glTexCoord2d(1.0f, 1.0f);	glVertex3f(-width, -height, length);	// Bottom Left Of The Quad (Front)
	glTexCoord2d(0.0f, 1.0f);	glVertex3f(width, -height, length);	// Bottom Right Of The Quad (Front)
	glEnd();

	glBindTexture(GL_TEXTURE_2D, this->texID[3]);
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 0.0f, 1.0f);
	glTexCoord2d(0.0f, 1.0f);	glVertex3f(width, -height, -length);	// Bottom Left Of The Quad (Back)
	glTexCoord2d(1.0f, 1.0f);	glVertex3f(-width, -height, -length);	// Bottom Right Of The Quad (Back)
	glTexCoord2d(1.0f, 0.0f);	glVertex3f(-width, height, -length);	// Top Right Of The Quad (Back)
	glTexCoord2d(0.0f, 0.0f);	glVertex3f(width, height, -length);	// Top Left Of The Quad (Back)
	glEnd();

	glBindTexture(GL_TEXTURE_2D, this->texID[4]);
	glBegin(GL_QUADS);
	glNormal3f(-1.0f, 0.0f, 0.0f);
	glTexCoord2d(1.0f, 0.0f);	glVertex3f(-width, height, -length);	// Top Right Of The Quad (Left)
	glTexCoord2d(1.0f, 1.0f);	glVertex3f(-width, -height, -length);	// Top Left Of The Quad (Left)
	glTexCoord2d(0.0f, 1.0f);	glVertex3f(-width, -height, length);	// Bottom Left Of The Quad (Left)
	glTexCoord2d(0.0f, 0.0f);	glVertex3f(-width, height, length);	// Bottom Right Of The Quad (Left)
	glEnd();

	glBindTexture(GL_TEXTURE_2D, this->texID[5]);
	glBegin(GL_QUADS);
	glNormal3f(1.0f, 0.0f, 0.0f);
	glTexCoord2d(0.0f, 0.0f);	glVertex3f(width, height, -length);	// Top Right Of The Quad (Right)
	glTexCoord2d(1.0f, 0.0f);	glVertex3f(width, height, length);	// Top Left Of The Quad (Right)
	glTexCoord2d(1.0f, 1.0f);	glVertex3f(width, -height, length);	// Bottom Left Of The Quad (Right)
	glTexCoord2d(0.0f, 1.0f);	glVertex3f(width, -height, -length);	// Bottom Right Of The Quad (Right)
	glEnd();
	glDisable(GL_TEXTURE_2D);
}