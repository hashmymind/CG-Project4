#include "Cube.h"
void Cube::Draw(Pnt3f vertex[8], Pnt3f nml[3]) {
    glPushMatrix();
    glBegin(GL_QUADS);
    int numArray[24] = {  // UDFBRL
        0,3,2,1,
        7,6,5,4,
        0,1,6,7,
        3,4,5,2,
        0,7,4,3,
        1,2,5,6
    };
    Pnt3f normal[6];
    for (int i = 0; i < 3; i++) {
        normal[i * 2] = nml[i];
        normal[i * 2 + 1] = -1 * nml[i];
    }
    for (int i = 0; i < 24; i++) {
        if (i % 4 == 0) {
            glNormal3d(normal[i / 4].x, normal[i / 4].y, normal[i / 4].z);
        }
        glVertex3d(vertex[numArray[i]].x, vertex[numArray[i]].y, vertex[numArray[i]].z);
    }
    glEnd();
    glPopMatrix();
}
