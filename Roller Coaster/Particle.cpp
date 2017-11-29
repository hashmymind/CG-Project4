#pragma once
#include "Particle.h"
#define MAX_PARTICLES 1000  
#define MAX_FIRES 5  
ParticleSystem::ParticleSystem() {
    this->nOfFires = 0;
    this->Tick1 = this->Tick2 = GetTickCount();
    this->DTick = 0;
}
void ParticleSystem::ProcessParticles() {
    Tick1 = Tick2;
    Tick2 = GetTickCount();
    DTick = float(Tick2 - Tick1);
    DTick *= 0.5f;
    Particle ep;
    if (nOfFires < MAX_FIRES) {
        InitParticle(ep);
    }
    for (int i = 0; i < Particles.size(); i++) {
        Particle& par = Particles[i];
        par.life -= par.fade*(float(DTick)*0.1f);//Particle壽命衰減 
        if (par.life <= 0.05f) {//當壽命小於一定值          
            if (par.nExpl) {//爆炸效果
                switch (par.nExpl) {
                case 1:
                    Explosion1(par);
                    break;
                case 2:
                    Explosion2(par);
                    break;
                case 3:
                    Explosion3(par);
                    break;
                case 4:
                    Explosion4(par);
                    break;
                case 5:
                    Explosion5(par);
                    break;
                case 6:
                    Explosion6(par);
                    break;
                case 7:
                    Explosion7(par);
                    break;
                default:
                    break;
                }
            }
            if (par.bFire)
                nOfFires--;
            Particles.erase(Particles.begin() + i); // Delete it.
            i--;
        } else {
            par.xpos += par.xspeed*DTick;   par.ypos += par.yspeed*DTick;
            par.zpos += par.zspeed*DTick;   par.yspeed -= grav*DTick;
            if (par.bAddParts) {//假如有尾巴  
                par.AddCount += 0.01f*DTick;//AddCount變化愈慢，尾巴粒子愈小  
                if (par.AddCount > par.AddSpeed) {//AddSpeed愈大，尾巴粒子愈小  
                    par.AddCount = 0;
                    ep.b = par.b;  ep.g = par.g;  ep.r = par.r;
                    ep.life = par.life*0.9f;//壽命變短  
                    ep.fade = par.fade*7.0f;//衰减快一些  
                    ep.size = 0.6f;//粒子尺寸小一些  
                    ep.xpos = par.xpos;  ep.ypos = par.ypos;  ep.zpos = par.zpos;
                    ep.xspeed = 0.0f;    ep.yspeed = 0.0f;  ep.zspeed = 0.0f;
                    ep.bFire = 0;
                    ep.nExpl = 0;
                    ep.bAddParts = 0;//尾巴粒子没有尾巴  
                    ep.AddCount = 0.0f;
                    ep.AddSpeed = 0.0f;
                    Particles.push_back(ep);
                }
            }
        }
    }
}
void ParticleSystem::DrawParticles() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
    //glTranslatef(0, 0, -60);
    for (auto& par : Particles) {
        glPushMatrix();
        glColor4f(par.r, par.g, par.b, par.life);
        // Rotate.
        glRotatef(par.rotX, par.xpos + 1, par.ypos, par.zpos);
        glRotatef(par.rotY, par.xpos, par.ypos + 1, par.zpos);
        glRotatef(par.rotZ, par.xpos, par.ypos, par.zpos + 1);

        glBegin(GL_TRIANGLE_STRIP);
        glTexCoord2d(1, 1);
        glVertex3f(par.xpos + par.size, par.ypos + par.size, par.zpos);
        glTexCoord2d(0, 1);
        glVertex3f(par.xpos - par.size, par.ypos + par.size, par.zpos);
        glTexCoord2d(1, 0);
        glVertex3f(par.xpos + par.size, par.ypos - par.size, par.zpos);
        glTexCoord2d(0, 0);
        glVertex3f(par.xpos - par.size, par.ypos - par.size, par.zpos);
        glEnd();
        glPopMatrix();
    }
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
}
void ParticleSystem::InitParticle(Particle& ep) {
    ep.b = float(rand() % 100) / 60.0f;//顏色隨機
    ep.g = float(rand() % 100) / 60.0f;
    ep.r = float(rand() % 100) / 60.0f;
    ep.life = 1.0f;//初始壽命
    ep.fade = 0.005f + float(rand() % 21) / 10000.0f;//衰减速度
    ep.size = 1;//大小  
    ep.xpos = 30.0f - float(rand() % 601) / 10.0f;//位置 
    ep.ypos = 0.0f;
    ep.zpos = 20.0f - float(rand() % 401) / 10.0f;
    ep.rotX = float(rand() % 360);
    ep.rotY = float(rand() % 360);
    ep.rotZ = float(rand() % 360);
    if (!int(ep.xpos))//x方向速度(z方向相同)
    {
        ep.xspeed = 0.0f;
    } else {
        if (ep.xpos<0) {
            ep.xspeed = (rand() % int(-ep.xpos)) / 1500.0f;
        } else {
            ep.xspeed = -(rand() % int(-ep.xpos)) / 1500.0f;
        }
    }
    ep.yspeed = 0.04f + float(rand() % 11) / 1000.0f;//y方向速度(向上)
    ep.bFire = 1;
    ep.nExpl = 1 + rand() % 6;//粒子效果  
    ep.bAddParts = 1;//設定有尾巴 
    ep.AddCount = 0.0f;
    ep.AddSpeed = 0.2f;
    nOfFires++;//粒子數+1 
    Particles.push_back(ep);//加入粒子列表
}


void ParticleSystem::Explosion1(const Particle& par) {
    Particle ep;
    for (int i = 0; i < 100; i++) {
        ep.b = float(rand() % 100) / 60.0f;
        ep.g = float(rand() % 100) / 60.0f;
        ep.r = float(rand() % 100) / 60.0f;
        ep.life = 1.0f;
        ep.fade = 0.01f + float(rand() % 31) / 10000.0f;
        ep.size = 0.8f;
        ep.xpos = par.xpos;
        ep.ypos = par.ypos;
        ep.zpos = par.zpos;
        ep.rotX = float(rand() % 360);
        ep.rotY = float(rand() % 360);
        ep.rotZ = float(rand() % 360);
        ep.xspeed = 0.02f - float(rand() % 41) / 1000.0f;
        ep.yspeed = 0.02f - float(rand() % 41) / 1000.0f;
        ep.zspeed = 0.02f - float(rand() % 41) / 1000.0f;
        ep.bFire = 0;
        ep.nExpl = 0;
        ep.bAddParts = 0;
        ep.AddCount = 0.0f;
        ep.AddSpeed = 0.0f;
        Particles.push_back(ep);
    }
}
void ParticleSystem::Explosion2(const Particle& par) {
    Particle ep;
    for (int i = 0; i < 1000; i++) {
        ep.b = par.b;
        ep.g = par.g;
        ep.r = par.r;
        ep.life = 1.0f;
        ep.fade = 0.01f + float(rand() % 31) / 10000.0f;
        ep.size = 0.8f;
        ep.xpos = par.xpos;
        ep.ypos = par.ypos;
        ep.zpos = par.zpos;
        ep.rotX = float(rand() % 360);
        ep.rotY = float(rand() % 360);
        ep.rotZ = float(rand() % 360);
        ep.xspeed = 0.02f - float(rand() % 41) / 1000.0f;
        ep.yspeed = 0.02f - float(rand() % 41) / 1000.0f;
        ep.zspeed = 0.02f - float(rand() % 41) / 1000.0f;
        ep.bFire = 0;
        ep.nExpl = 0;
        ep.bAddParts = 0;
        ep.AddCount = 0.0f;
        ep.AddSpeed = 0.0f;
        Particles.push_back(ep);
    }
}
void ParticleSystem::Explosion3(const Particle& par) {
    Particle ep;
    float PIAsp = 3.1415926 / 180;
    for (int i = 0; i < 30; i++) {
        float angle = float(rand() % 360)*PIAsp;
        ep.b = par.b;
        ep.g = par.g;
        ep.r = par.r;
        ep.life = 1.5f;
        ep.fade = 0.01f + float(rand() % 31) / 10000.0f;
        ep.size = 0.8f;
        ep.xpos = par.xpos;
        ep.ypos = par.ypos;
        ep.zpos = par.zpos;
        ep.rotX = float(rand() % 360);
        ep.rotY = float(rand() % 360);
        ep.rotZ = float(rand() % 360);
        ep.xspeed = (float) sin(angle)*0.01f;
        ep.yspeed = 0.01f + float(rand() % 11) / 1000.0f;
        ep.zspeed = (float) cos(angle)*0.01f;
        ep.bFire = 0;
        ep.nExpl = 0;
        ep.bAddParts = 1;
        ep.AddCount = 0.0f;
        ep.AddSpeed = 0.2f;
        Particles.push_back(ep);
    }
}
void ParticleSystem::Explosion4(const Particle& par) {
    Particle ep;
    float PIAsp = 3.1415926 / 180;
    for (int i = 0; i < 30; i++) {
        float angle = float(rand() % 360)*PIAsp;
        ep.b = float(rand() % 100) / 60.0f;
        ep.g = float(rand() % 100) / 60.0f;
        ep.r = float(rand() % 100) / 60.0f;
        ep.life = 1.5f;
        ep.fade = 0.01f + float(rand() % 31) / 10000.0f;
        ep.size = 0.8f;
        ep.xpos = par.xpos;
        ep.ypos = par.ypos;
        ep.zpos = par.zpos;
        ep.rotX = float(rand() % 360);
        ep.rotY = float(rand() % 360);
        ep.rotZ = float(rand() % 360);
        ep.xspeed = (float) sin(angle)*0.01f;
        ep.yspeed = 0.01f + float(rand() % 11) / 1000.0f;
        ep.zspeed = (float) cos(angle)*0.01f;
        ep.bFire = 0;
        ep.nExpl = 0;
        ep.bAddParts = 1;
        ep.AddCount = 0.0f;
        ep.AddSpeed = 0.2f;
        Particles.push_back(ep);
    }
}
void ParticleSystem::Explosion5(const Particle& par) {
    Particle ep;
    for (int i = 0; i < 30; i++) {
        ep.b = par.b;
        ep.g = par.g;
        ep.r = par.r;
        ep.life = 0.8f;
        ep.fade = 0.01f + float(rand() % 31) / 10000.0f;
        ep.size = 0.8f;
        ep.xpos = par.xpos;
        ep.ypos = par.ypos;
        ep.zpos = par.zpos;
        ep.rotX = float(rand() % 360);
        ep.rotY = float(rand() % 360);
        ep.rotZ = float(rand() % 360);
        ep.xspeed = 0.01f - float(rand() % 21) / 1000.0f;
        ep.yspeed = 0.01f - float(rand() % 21) / 1000.0f;
        ep.zspeed = 0.01f - float(rand() % 21) / 1000.0f;
        ep.bFire = 0;
        ep.nExpl = 7;
        ep.bAddParts = 0;
        ep.AddCount = 0.0f;
        ep.AddSpeed = 0.0f;
        Particles.push_back(ep);
    }
}
void ParticleSystem::Explosion6(const Particle& par) {
    Particle ep;
    for (int i = 0; i < 100; i++) {
        ep.b = float(rand() % 100) / 60.0f;
        ep.g = float(rand() % 100) / 60.0f;
        ep.r = float(rand() % 100) / 60.0f;
        ep.life = 0.8f;
        ep.fade = 0.01f + float(rand() % 31) / 10000.0f;
        ep.size = 0.8f;
        ep.xpos = par.xpos;
        ep.ypos = par.ypos;
        ep.zpos = par.zpos;
        ep.rotX = float(rand() % 360);
        ep.rotY = float(rand() % 360);
        ep.rotZ = float(rand() % 360);
        ep.xspeed = 0.01f - float(rand() % 21) / 1000.0f;
        ep.yspeed = 0.01f - float(rand() % 21) / 1000.0f;
        ep.zspeed = 0.01f - float(rand() % 21) / 1000.0f;
        ep.bFire = 0;
        ep.nExpl = 7;
        ep.bAddParts = 0;
        ep.AddCount = 0.0f;
        ep.AddSpeed = 0.0f;
        Particles.push_back(ep);
    }
}
void ParticleSystem::Explosion7(const Particle& par) {
    Particle ep;
    for (int i = 0; i < 10; i++) {
        ep.b = par.b;
        ep.g = par.g;
        ep.r = par.r;
        ep.life = 0.5f;
        ep.fade = 0.01f + float(rand() % 31) / 10000.0f;
        ep.size = 0.6f;
        ep.xpos = par.xpos;
        ep.ypos = par.ypos;
        ep.zpos = par.zpos;
        ep.rotX = float(rand() % 360);
        ep.rotY = float(rand() % 360);
        ep.rotZ = float(rand() % 360);
        ep.xspeed = 0.02f - float(rand() % 41) / 1000.0f;
        ep.yspeed = 0.02f - float(rand() % 41) / 1000.0f;
        ep.zspeed = 0.02f - float(rand() % 41) / 1000.0f;
        ep.bFire = 0;
        ep.nExpl = 0;
        ep.bAddParts = 0;
        ep.AddCount = 0.0f;
        ep.AddSpeed = 0.0f;
        Particles.push_back(ep);
    }
}
