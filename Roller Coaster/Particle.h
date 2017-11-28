#pragma once
#include <QtOpenGL/qgl.h>
#include <vector>

using namespace std;

struct Particle {
    GLfloat xpos;//(xpos,ypos,zpos)為particle的position
    GLfloat ypos;
    GLfloat zpos;
    GLfloat xspeed;//(xspeed,yspeed,zspeed)為particle的speed
    GLfloat yspeed;
    GLfloat zspeed;
    GLfloat r;//(r,g,b)為particle的color
    GLfloat g;
    GLfloat b;
    GLfloat life;// particle的壽命
    GLfloat fade;// particle的衰減速度
    GLfloat size;// particle的大小
    GLbyte  bFire;
    GLbyte  nExpl;//哪種particle效果
    GLbyte  bAddParts;// particle是否含有尾巴
    GLfloat AddSpeed;//尾巴粒子的加速度
    GLfloat AddCount;//尾巴粒子的增加量
};
class ParticleSystem {
public:
    vector<Particle> Particles;
    UINT nOfFires;
    UINT Tick1, Tick2;
    float DTick;
    const GLfloat grav = 0.00003f;
    GLuint textureID;

    ParticleSystem();

    void InitParticle(Particle& ep);
    void ProcessParticles();
    void DrawParticles();

    void Explosion1(const Particle& par);
    void Explosion2(const Particle& par);
    void Explosion3(const Particle& par);
    void Explosion4(const Particle& par);
    void Explosion5(const Particle& par);
    void Explosion6(const Particle& par);
    void Explosion7(const Particle& par);
};