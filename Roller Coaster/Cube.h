#pragma once
#include <QtOpenGL/qgl.h>
#include "Utilities/Pnt3f.H"
class Cube {
public:
    Pnt3f vertex[8];
    Pnt3f normal[6];
    Cube();
    Cube(Pnt3f pos, Pnt3f dir, Pnt3f scale, Pnt3f rgb);
    Cube(Pnt3f vtx[8], Pnt3f nml[3]);
    void Draw();
private:

};