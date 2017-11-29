#include "Cube.h"
Cube::Cube() {
}
Cube::Cube(Pnt3f pos, Pnt3f dir, Pnt3f scale, Pnt3f rgb) {
    /*this->pos = pos;
    this->dir = dir;
    this->scale = scale;
    this->rgb = rgb;*/
}
Cube::Cube(Pnt3f vtx[8], Pnt3f nml[3]) {
    for (int i = 0; i < 8; i++) {
        this->vertex[i] = vtx[i];
    }
    for (int i = 0; i < 3; i++) {
        this->normal[i * 2] = nml[i];
        this->normal[i * 2 + 1] = -1 * nml[i];
    }
}
void Cube::Draw() {
    glPushMatrix();
    /*glTranslated(this->pos.x, this->pos.y, this->pos.z);
    glRotatef(this->dir.x, 1, 0, 0);
    glRotatef(this->dir.y, 0, 1, 0);
    glRotatef(this->dir.z, 0, 0, 1);
    glScalef(this->scale.x, this->scale.y, this->scale.z);*/
    glBegin(GL_QUADS);
    int numArray[24] = {  // UDFBRL
        0,3,2,1,
        7,6,5,4,
        0,1,6,7,
        3,4,5,2,
        0,7,4,3, 
        1,2,5,6
    };
    for (int i = 0; i < 24; i++) {
        if (i % 4 == 0) {
            glNormal3d(this->normal[i / 4].x, this->normal[i / 4].y, this->normal[i / 4].z);
        }
        glVertex3d(this->vertex[numArray[i]].x, this->vertex[numArray[i]].y, this->vertex[numArray[i]].z);
    }
    /*glNormal3d(0, 0, 1);
    glVertex3d(0.5, 0.5, 0.5);
    glVertex3d(-0.5, 0.5, 0.5);
    glVertex3d(-0.5, -0.5, 0.5);
    glVertex3d(0.5, -0.5, 0.5);

    glNormal3d(0, 0, -1);
    glVertex3d(0.5, 0.5, -0.5);
    glVertex3d(0.5, -0.5, -0.5);
    glVertex3d(-0.5, -0.5, -0.5);
    glVertex3d(-0.5, 0.5, -0.5);

    glNormal3d(0, 1, 0);
    glVertex3d(0.5, 0.5, 0.5);
    glVertex3d(0.5, 0.5, -0.5);
    glVertex3d(-0.5, 0.5, -0.5);
    glVertex3d(-0.5, 0.5, 0.5);

    glNormal3d(0, -1, 0);
    glVertex3d(0.5, -0.5, 0.5);
    glVertex3d(-0.5, -0.5, 0.5);
    glVertex3d(-0.5, -0.5, -0.5);
    glVertex3d(0.5, -0.5, -0.5);

    glNormal3d(1, 0, 0);
    glVertex3d(0.5, 0.5, 0.5);
    glVertex3d(0.5, -0.5, 0.5);
    glVertex3d(0.5, -0.5, -0.5);
    glVertex3d(0.5, 0.5, -0.5);

    glNormal3d(-1, 0, 0);
    glVertex3d(-0.5, 0.5, 0.5);
    glVertex3d(-0.5, 0.5, -0.5);
    glVertex3d(-0.5, -0.5, -0.5);
    glVertex3d(-0.5, -0.5, 0.5);*/
    glEnd();
    glPopMatrix();
}
