#include "TrainView.h"
#include "Cube.h"
#include<iostream>

TrainView::TrainView(QWidget *parent) :  
QGLWidget(parent)  
{  
	resetArcball();
}  
TrainView::~TrainView()  
{}  
void TrainView::initializeGL()
{
	initializeOpenGLFunctions();
    GLuint tex;
    loadTexture2D("tex/unnamed.png", tex, true);
    this->trainModel = new Model(TRAIN_PATH, tex, 25, Point3d(0, 5, 0), Pnt3f(60, 60, 60));
    // Cars.
    this->cars.push_back(new Model(TRAIN_PATH, 0, 25, Point3d(0, 5, 0), Pnt3f(rand() % 255, rand() % 255, rand() % 255)));
    this->cars.push_back(new Model(TRAIN_PATH, 0, 25, Point3d(0, 5, 0), Pnt3f(rand() % 255, rand() % 255, rand() % 255)));
    // Other models.
    //loadTexture2D("tex/iron2.jpg", tex, true);
    tunnels.push_back(new Tunnel(0.83, "mod/tunnel.obj", 0, 80, Point3d(0,10,0), Pnt3f(60, 60, 120)));

    // Particle system.
    this->particle = new ParticleSystem;
    loadTexture2D("tex/cloud3.png", this->particle->textureID);
}
void TrainView:: resetArcball()
	//========================================================================
{
	// Set up the camera to look at the world
	// these parameters might seem magical, and they kindof are
	// a little trial and error goes a long way
	arcball.setup(this, 40, 250, .2f, .4f, 0);
}

void TrainView::paintGL()
{

	//*********************************************************************
	//
	// * Set up basic opengl informaiton
	//
	//**********************************************************************
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	// Set up the view port
	glViewport(0,0,width(),height());
	// clear the window, be sure to clear the Z-Buffer too
	glClearColor(0,0,0.3f,0);
	
	// we need to clear out the stencil buffer since we'll use
	// it for shadows
	glClearStencil(0);
	glEnable(GL_DEPTH);

	// Blayne prefers GL_DIFFUSE
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	// prepare for projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	setProjection();		// put the code to set up matrices here

	//######################################################################
	// TODO: 
	// you might want to set the lighting up differently. if you do, 
	// we need to set up the lights AFTER setting up the projection
	//######################################################################
	// enable the lighting
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	// top view only needs one light
	if (this->camera == 1) {
		glDisable(GL_LIGHT1);
		glDisable(GL_LIGHT2);
	} else {
		glEnable(GL_LIGHT1);
		glEnable(GL_LIGHT2);
	}

	//*********************************************************************
	//
	// * set the light parameters
	//
	//**********************************************************************
	GLfloat lightPosition1[]	= {0,1,1,0}; // {50, 200.0, 50, 1.0};
	GLfloat lightPosition2[]	= {1, 0, 0, 0};
	GLfloat lightPosition3[]	= {0, -1, 0, 0};
	GLfloat yellowLight[]		= {0.5f, 0.5f, .1f, 1.0};
	GLfloat whiteLight[]		= {1.0f, 1.0f, 1.0f, 1.0};
	GLfloat blueLight[]			= {.1f,.1f,.3f,1.0};
	GLfloat grayLight[]			= {.3f, .3f, .3f, 1.0};

	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition1);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, whiteLight);
	glLightfv(GL_LIGHT0, GL_AMBIENT, grayLight);

	glLightfv(GL_LIGHT1, GL_POSITION, lightPosition2);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, yellowLight);

	glLightfv(GL_LIGHT2, GL_POSITION, lightPosition3);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, blueLight);



	//*********************************************************************
	// now draw the ground plane
	//*********************************************************************
	setupFloor();
	glDisable(GL_LIGHTING);
	drawFloor(200,10);


	//*********************************************************************
	// now draw the object and we need to do it twice
	// once for real, and then once for shadows
	//*********************************************************************
	glEnable(GL_LIGHTING);
	setupObjects();

	drawStuff();

    // Particle.
    particle->ProcessParticles();
    particle->DrawParticles();

	// this time drawing is for shadows (except for top view)
	if (this->camera != 1) {
		setupShadows();
		drawStuff(true);
		unsetupShadows();
	}

    // Train run.
    if (this->isrun) {
        this->tPos = this->advanceTrain(this->tPos); // Advance train.
        //this->trainGravity(); // Gravity.
    }
}

//************************************************************************
//
// * This sets up both the Projection and the ModelView matrices
//   HOWEVER: it doesn't clear the projection first (the caller handles
//   that) - its important for picking
//========================================================================
void TrainView::
setProjection()
//========================================================================
{
	// Compute the aspect ratio (we'll need it)
	float aspect = static_cast<float>(width()) / static_cast<float>(height());

	// Check whether we use the world camp
	if (this->camera == 0){
		arcball.setProjection(false);
		update();
	// Or we use the top cam
	}else if (this->camera == 1) {
		float wi, he;
		if (aspect >= 1) {
			wi = 110;
			he = wi / aspect;
		} 
		else {
			he = 110;
			wi = he * aspect;
		}

		// Set up the top camera drop mode to be orthogonal and set
		// up proper projection matrix
		glMatrixMode(GL_PROJECTION);
		glOrtho(-wi, wi, -he, he, 200, -200);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glRotatef(-90,1,0,0);
		update();
	} 
	else {
		trainCamView(aspect);
		update();
	}
}

//************************************************************************
//
// * this draws all of the stuff in the world
//
//	NOTE: if you're drawing shadows, DO NOT set colors (otherwise, you get 
//       colored shadows). this gets called twice per draw 
//       -- once for the objects, once for the shadows
//########################################################################
// TODO: 
// if you have other objects in the world, make sure to draw them
//########################################################################
//========================================================================
void TrainView::drawStuff(bool doingShadows)
{
	// Draw the control points
	// don't draw the control points if you're driving 
	// (otherwise you get sea-sick as you drive through them)
	if (this->camera != 2) {
		for(size_t i = 0; i < this->m_pTrack->points.size(); ++i) {
			if (!doingShadows) {
				if ( ((int) i) != selectedCube)
					glColor3ub(240, 60, 60);
				else
					glColor3ub(240, 240, 30);
			}
			this->m_pTrack->points[i].draw();
		}
		update();
	}
	// draw the track
	drawTrack(doingShadows);
    // draw train.
	drawTrain(true, doingShadows);// don't draw the train if you're looking out the front window

    // draw models
    for (int i = 0; i < models.size(); i++) {
        models[i]->Draw();
    }
    drawTunnel(doingShadows);
}
void TrainView::calcPosition(Pnt3f& qt, Pnt3f& orient, Pnt3f cpPos[4], Pnt3f cpOrient[4], float t, spline_t& type_spline) {
    switch (type_spline) {
    case spline_Linear:
        orient = this->Linear(cpOrient[1], cpOrient[2], t);
        qt = this->Linear(cpPos[1], cpPos[2], t);
        break;
    case spline_Cardinal:
        orient = this->Cardinal(cpOrient[0], cpOrient[1], cpOrient[2], cpOrient[3], t);
        qt = this->Cardinal(cpPos[0], cpPos[1], cpPos[2], cpPos[3], t);
        break;
    case spline_Cubic:
        orient = this->Cubic(cpOrient[0], cpOrient[1], cpOrient[2], cpOrient[3], t);
        qt = this->Cubic(cpPos[0], cpPos[1], cpPos[2], cpPos[3], t);
        break;
    }
}
void TrainView::drawTrack(bool doingShadows) {
	this->arclen.clear();
	this->arclen.resize(this->m_pTrack->points.size());
    spline_t type_spline = (spline_t) this->curve;
	float intervalCount = 0;
    for (size_t i = 0; i < this->m_pTrack->points.size(); ++i) {
        // pos
        Pnt3f cpPos[4] = {
            this->m_pTrack->points[((i - 1) + this->m_pTrack->points.size()) % this->m_pTrack->points.size()].pos, // for G
            this->m_pTrack->points[i].pos,
            this->m_pTrack->points[(i + 1) % this->m_pTrack->points.size()].pos,
            this->m_pTrack->points[(i + 2) % this->m_pTrack->points.size()].pos // for G
        };
        // orient
        Pnt3f cpOrient[4] = {
            this->m_pTrack->points[((i - 1) + this->m_pTrack->points.size()) % this->m_pTrack->points.size()].orient, // for G
            this->m_pTrack->points[i].orient,
            this->m_pTrack->points[(i + 1) % this->m_pTrack->points.size()].orient,
            this->m_pTrack->points[(i + 2) % this->m_pTrack->points.size()].orient // for G
        };
        float percent = 1.0f / DIVIDE_LINE;
        float t = 0;

		float partialLen = 0;
        Pnt3f qt, orient;
        for (size_t j = 0; t<=1; j++) {
            Pnt3f qt0 = qt;
            calcPosition(qt, orient, cpPos, cpOrient, t, type_spline);
            Pnt3f qt1 = qt;
			// calculate partial length
			float dist = sqrt(pow(qt1.x - qt0.x, 2) + pow(qt1.y - qt0.y, 2) + pow(qt1.z - qt0.z, 2));
			if (j) {
				partialLen += dist;
				intervalCount += dist;
			}
            // cross
            orient.normalize();
            Pnt3f cross = (qt1 - qt0) * orient;
            cross.normalize();
            cross = cross * 2.5f;

            // Draw.
			if (j == 0) { 
				lpt1pc = qt1 + cross;
				lpt1mc = qt1 - cross;
				t += percent;
				continue;
			}
			else if (j == 1) {
				// supporting struction
				Pnt3f tmp = cross * 2;
				Pnt3f vqt = (5.0 / dist) * (qt0 - qt1);
				glBegin(GL_QUADS);

				glColor3ub(0x66, 0xCC, 0xFF);
				glVertex3f(qt0.x - tmp.x, qt0.y - tmp.y -0.2, qt0.z - tmp.z);
				glVertex3f(qt0.x + tmp.x, qt0.y + tmp.y - 0.2, qt0.z + tmp.z);
				glVertex3f(qt0.x + tmp.x + vqt.x, qt0.y + tmp.y + vqt.y-1, qt0.z + tmp.z + vqt.z);
				glVertex3f(qt0.x - tmp.x + vqt.x, qt0.y - tmp.y + vqt.y-1, qt0.z - tmp.z + vqt.z);

				glVertex3f(qt0.x - tmp.x, qt0.y - tmp.y - 0.2, qt0.z - tmp.z);
				glVertex3f(qt0.x - tmp.x + vqt.x, qt0.y - tmp.y + vqt.y - 0.2, qt0.z - tmp.z + vqt.z);
				glVertex3f(qt0.x - tmp.x + vqt.x, 0, qt0.z - tmp.z + vqt.z);
				glVertex3f(qt0.x - tmp.x, 0, qt0.z - tmp.z);

				glVertex3f(qt0.x + tmp.x + vqt.x, qt0.y + tmp.y + vqt.y - 0.2, qt0.z + tmp.z + vqt.z);
				glVertex3f(qt0.x + tmp.x, qt0.y + tmp.y - 0.2, qt0.z + tmp.z);
				glVertex3f(qt0.x + tmp.x, 0, qt0.z + tmp.z);
				glVertex3f(qt0.x + tmp.x + vqt.x,0, qt0.z + tmp.z + vqt.z);
				tmp = cross;

				glVertex3f(qt0.x - tmp.x, qt0.y - tmp.y - 0.2, qt0.z - tmp.z);
				glVertex3f(qt0.x - tmp.x + vqt.x, qt0.y - tmp.y + vqt.y - 0.2, qt0.z - tmp.z + vqt.z);
				glVertex3f(qt0.x - tmp.x + vqt.x, 0, qt0.z - tmp.z + vqt.z);
				glVertex3f(qt0.x - tmp.x, 0, qt0.z - tmp.z);

				glVertex3f(qt0.x + tmp.x + vqt.x, qt0.y + tmp.y + vqt.y - 0.2, qt0.z + tmp.z + vqt.z);
				glVertex3f(qt0.x + tmp.x, qt0.y + tmp.y - 0.2, qt0.z + tmp.z);
				glVertex3f(qt0.x + tmp.x, 0, qt0.z + tmp.z);
				glVertex3f(qt0.x + tmp.x + vqt.x, 0, qt0.z + tmp.z + vqt.z);

				glVertex3f(qt0.x - tmp.x, qt0.y - tmp.y - 0.2, qt0.z - tmp.z);
				glVertex3f(qt0.x - tmp.x*2, qt0.y - tmp.y*2 - 0.2, qt0.z - tmp.z*2);
				glVertex3f(qt0.x - tmp.x * 2, 0, qt0.z - tmp.z * 2);
				glVertex3f(qt0.x - tmp.x,0, qt0.z - tmp.z);

				glVertex3f(qt0.x - tmp.x + vqt.x, qt0.y - tmp.y + vqt.y - 0.2, qt0.z - tmp.z + vqt.z);
				glVertex3f(qt0.x - tmp.x*2 + vqt.x, qt0.y - tmp.y*2 + vqt.y - 0.2, qt0.z - tmp.z*2 + vqt.z);
				glVertex3f(qt0.x - tmp.x * 2 + vqt.x, 0, qt0.z - tmp.z * 2 + vqt.z);
				glVertex3f(qt0.x - tmp.x + vqt.x, 0, qt0.z - tmp.z + vqt.z);

				glVertex3f(qt0.x + tmp.x + vqt.x, qt0.y + tmp.y + vqt.y - 0.2, qt0.z + tmp.z + vqt.z);
				glVertex3f(qt0.x + tmp.x * 2 + vqt.x, qt0.y + tmp.y * 2 + vqt.y - 0.2, qt0.z + tmp.z * 2 + vqt.z);
				glVertex3f(qt0.x + tmp.x * 2 + vqt.x, 0, qt0.z + tmp.z * 2 + vqt.z);
				glVertex3f(qt0.x + tmp.x + vqt.x, 0, qt0.z + tmp.z + vqt.z);

				glVertex3f(qt0.x + tmp.x, qt0.y + tmp.y - 0.2, qt0.z + tmp.z);
				glVertex3f(qt0.x + tmp.x * 2, qt0.y + tmp.y * 2 - 0.2, qt0.z + tmp.z * 2);
				glVertex3f(qt0.x + tmp.x * 2, 0, qt0.z + tmp.z * 2);
				glVertex3f(qt0.x + tmp.x, 0, qt0.z + tmp.z);

				glColor3ub(60, 60, 60);
				glEnd();
				lpt1pc = qt1 + cross;
				lpt1mc = qt1 - cross;
				t += percent;
				continue;
				//
			}
			t += percent;
            glLineWidth(5);
            glBegin(GL_LINES);
            if (!doingShadows) {
                // Spline color.
                glColor3ub(66, 22, 0);
            }
			//
			glVertex3f(lpt1pc.x, lpt1pc.y, lpt1pc.z);
			glVertex3f(qt0.x + cross.x, qt0.y + cross.y, qt0.z + cross.z);
			
			glVertex3f(lpt1mc.x, lpt1mc.y, lpt1mc.z);
			glVertex3f(qt0.x - cross.x, qt0.y - cross.y, qt0.z - cross.z);

            glVertex3f(qt0.x + cross.x, qt0.y + cross.y, qt0.z + cross.z);
            glVertex3f(qt1.x + cross.x, qt1.y + cross.y, qt1.z + cross.z);

            glVertex3f(qt0.x - cross.x, qt0.y - cross.y, qt0.z - cross.z);
            glVertex3f(qt1.x - cross.x, qt1.y - cross.y, qt1.z - cross.z);
            glEnd();
			lpt1pc = qt1 + cross;
			lpt1mc = qt1 - cross;
			//
			if (this->track == 1 && intervalCount > INTERVAL) {
                if (!doingShadows) {
                    // Track color.
                    glColor3ub(100, 33, 0);
                }
				// track
				glBegin(GL_POLYGON);
				intervalCount = 0;
				Pnt3f vqt = (2.0 / dist) * (qt0 - qt1);
                cross = cross * 1.4f;
                const float WOOD_HEIGHT = 1.0f;
                Pnt3f vtx[8] = {
                    qt0 + vqt + cross + Pnt3f(0,WOOD_HEIGHT / 2,0),
                    qt0 + vqt - cross + Pnt3f(0,WOOD_HEIGHT / 2,0),
                    qt0 - cross + Pnt3f(0,WOOD_HEIGHT / 2,0),
                    qt0 + cross + Pnt3f(0,WOOD_HEIGHT / 2,0),
                    qt0 + cross - Pnt3f(0,WOOD_HEIGHT / 2,0),
                    qt0 - cross - Pnt3f(0,WOOD_HEIGHT / 2,0),
                    qt0 + vqt - cross - Pnt3f(0,WOOD_HEIGHT / 2,0),
                    qt0 + vqt +  cross - Pnt3f(0,WOOD_HEIGHT / 2,0)
                };
                // forward 
                Pnt3f f = (qt1 - qt0);
                f.normalize();
                cross.normalize();
                // up.
                Pnt3f up = cross * f;
                up.normalize();
                Pnt3f nml[3] = { //UFR
                    up,
                    f,
                    cross
                };
                Cube wood(vtx, nml);
                wood.Draw();
				glEnd();

			}
			else if (this->track == 2) {
                if (!doingShadows) {
                    // Track color.
                    glColor3ub(32, 32, 64);
                }
				// plane
				glBegin(GL_POLYGON);
				Pnt3f vqt = (qt0 - qt1)*2;
				cross = cross * 1.4f;
				glVertex3f(qt0.x - cross.x, qt0.y - cross.y-0.1, qt0.z - cross.z);
				glVertex3f(qt0.x + cross.x, qt0.y + cross.y - 0.1, qt0.z + cross.z);
				glVertex3f(qt0.x + cross.x + vqt.x, qt0.y + cross.y + vqt.y - 0.1, qt0.z + cross.z + vqt.z);
				glVertex3f(qt0.x - cross.x + vqt.x, qt0.y - cross.y + vqt.y - 0.1, qt0.z - cross.z + vqt.z);
				glEnd();
			}
            glLineWidth(1);
        }
		this->arclen[i] = partialLen;
    }
}
// 傳入0~1的數 回傳train的座標資訊
void TrainView::calcTrain(Pnt3f& qt, Pnt3f& orient, float t) {
    spline_t type_spline = (spline_t) this->curve;
    t *= m_pTrack->points.size();
    size_t i;
    for (i = 0; t > 1; t -= 1) { i++; }
    // pos
    Pnt3f cpPos[4] = {
        this->m_pTrack->points[((i - 1) + this->m_pTrack->points.size()) % this->m_pTrack->points.size()].pos, // for G
        this->m_pTrack->points[i].pos,
        this->m_pTrack->points[(i + 1) % this->m_pTrack->points.size()].pos,
        this->m_pTrack->points[(i + 2) % this->m_pTrack->points.size()].pos // for G
    };
    // orient
    Pnt3f cpOrient[4] = {
        this->m_pTrack->points[((i - 1) + this->m_pTrack->points.size()) % this->m_pTrack->points.size()].orient, // for G
        this->m_pTrack->points[i].orient,
        this->m_pTrack->points[(i + 1) % this->m_pTrack->points.size()].orient,
        this->m_pTrack->points[(i + 2) % this->m_pTrack->points.size()].orient // for G
    };

    calcPosition(qt, orient, cpPos, cpOrient, t, type_spline);
}

// Return the next position.
float TrainView::advanceTrain(float t) {
    float tt = t * m_pTrack->points.size();
    size_t i;
    for (i = 0; tt > 1; tt -= 1) { i++; }
    t += (this->velocity / this->m_pTrack->points.size() / (this->arclen[i]));
    if (t > 1.0f) {
        t -= 1.0f;
    } else if (t < 0.0f) {
        t += 1.0f;
    }
    return t;
}
void TrainView::trainGravity() {
    this->velocity -= this->trainBasisZ.y * this->G;
    if (this->velocity < this->oriVelocity * this->minimumVelocityRate) {
        this->velocity = this->oriVelocity * this->minimumVelocityRate;
    } else if (this->velocity > this->oriVelocity) { // 阻力
      this->velocity -= this->G / 6;
      if (this->velocity < this->oriVelocity) { this->velocity = this->oriVelocity; }
      }
}
void TrainView::drawTrain(bool drawingTrain, bool doingShadows) {
    if (doingShadows) {
        this->trainModel->Draw(true);
        for (int i = 0; i < cars.size(); i++) {
            this->cars[i]->Draw(true);
        }
        return;
    }
    Pnt3f qt, orient;
    calcTrain(qt, orient, this->tPos);
    // Update cureent train coordinate.
    this->trainPos = qt;
    this->trainBasisY = orient;
    this->trainBasisY.normalize();
    // Update train direction.
    float nextT = advanceTrain(this->tPos);
    calcTrain(qt, orient, nextT);
    this->trainBasisZ = qt - this->trainPos;
    this->trainBasisZ.normalize();
    // Cross get X.
    this->trainBasisX = this->trainBasisZ * this->trainBasisY;
    this->trainBasisX.normalize();
    // Cross get Y.(cuz orient is not real Y.)
    this->trainBasisY = this->trainBasisX * this->trainBasisZ;
    this->trainBasisY.normalize();

	// do the things above to cars
    Pnt3f carQT;
    Pnt3f carTBX;
    Pnt3f carTBY;
    Pnt3f carTBZ;
	float totalArclen = 0;
    for (int i = 0; i < this->arclen.size(); ++i) { totalArclen += this->arclen[i]; }
	
	float gap = 25;
	for (int i = 0; i < this->cars.size(); ++i) {
		float posi = this->tPos,gapLeft = (i+1)*gap;
		while (gapLeft > 0.00001) {
			while (posi < 0.0)posi += 1;
			float tmp = posi*this->m_pTrack->points.size();
			size_t k;
			for (k = 0; tmp > 1; tmp -= 1) { k++; }
			float distLeft = tmp*this->arclen[k];
			if (distLeft > gapLeft) {
				posi -= (gapLeft / this->arclen[k])/ this->m_pTrack->points.size();
				gapLeft = 0;
			}
			else {
				posi = float(k)/ this->m_pTrack->points.size();
				posi -= 0.00000001;
				gapLeft -= distLeft;
			}
		}
		while (posi > 1.0)posi -= 1;
		while (posi < 0.0)posi += 1;
		calcTrain(qt, orient, posi);

        carQT = qt;
        carTBY = orient;
        carTBY.normalize();

        // Update train direction.
        nextT = advanceTrain(posi);
        calcTrain(qt, orient, nextT);
        carTBZ = qt - carQT;
        carTBZ.normalize();
        // Cross get X.
        carTBX = carTBZ * carTBY;
        carTBX.normalize();
        // Cross get Y.(cuz orient is not real Y.)
        carTBY = carTBX * carTBZ;
        carTBY.normalize();
        if (drawingTrain) {
            this->cars[i]->setBasis(carTBX, carTBY, carTBZ);
            this->cars[i]->setPosi(Point3d(carQT.x, carQT.y, carQT.z));
            this->cars[i]->Draw();
        }

	}

	//*/

    // Draw.
    if (drawingTrain) {
		/*glBegin(GL_LINES);
		Pnt3f xx, yy, zz;
		xx = this->trainBasisX * 20;
		xx = xx + qt;
		yy = this->trainBasisY * 20;
		yy = yy + qt;
		zz = this->trainBasisZ * 20;
		zz = zz + qt;
		glColor3ub(60, 60, 60);
		glVertex3f(qt.x,qt.y, qt.z);
		glVertex3f(xx.x , xx.y, xx.z);
		glColor3ub(120, 120, 120);
		glVertex3f(qt.x, qt.y, qt.z);
		glVertex3f(yy.x, yy.y, yy.z);
		glColor3ub(240, 240, 240);
		glVertex3f(qt.x, qt.y, qt.z);
		glVertex3f(zz.x, zz.y, zz.z);
		glEnd();*/
		this->trainModel->setBasis(this->trainBasisX, this->trainBasisY, this->trainBasisZ);
		this->trainModel->setPosi(Point3d(this->trainPos.x, this->trainPos.y, this->trainPos.z));
		this->trainModel->Draw();
    }
}

void TrainView::drawTunnel(bool doingShadows) {
    Pnt3f qt0, qt1, orient, bx, by, bz;
    for (int i = 0; i < this->tunnels.size(); i++) {
        this->calcTrain(qt0, orient, this->tunnels[i]->t);
        // Update train direction.
        by = orient;
        by.normalize();
        float nextT = advanceTrain(this->tunnels[i]->t);
        calcTrain(qt1, orient, nextT);
        bz = qt1 - qt0;
        bz.normalize();
        // Cross get X.
        bx = bz * by;
        bx.normalize();
        // Cross get Y.(cuz orient is not real Y.)
        by = bx * bz;
        by.normalize();
        this->tunnels[i]->setPosi(Point3d(qt0.x, qt0.y, qt0.z));
        this->tunnels[i]->setBasis(bx, by, bz);
        this->tunnels[i]->Draw(doingShadows);
    }
}

void TrainView::
	doPick(int mx, int my)
	//========================================================================
{
	// since we'll need to do some GL stuff so we make this window as 
	// active window
	makeCurrent();

	// get the viewport - most reliable way to turn mouse coords into GL coords
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity ();

	gluPickMatrix((double)mx, (double)(viewport[3]-my), 
		5, 5, viewport);

	// now set up the projection
	setProjection();

	// now draw the objects - but really only see what we hit
	GLuint buf[100];
	glSelectBuffer(100,buf);
	glRenderMode(GL_SELECT);
	glInitNames();
	glPushName(0);


	// draw the cubes, loading the names as we go
	for(size_t i=0; i<m_pTrack->points.size(); ++i) {
		glLoadName((GLuint) (i+1));
		m_pTrack->points[i].draw();
	}

	// go back to drawing mode, and see how picking did
	int hits = glRenderMode(GL_RENDER);
	if (hits) {
		// warning; this just grabs the first object hit - if there
		// are multiple objects, you really want to pick the closest
		// one - see the OpenGL manual 
		// remember: we load names that are one more than the index
		selectedCube = buf[3]-1;
	} else // nothing hit, nothing selected
		selectedCube = -1;
}
// Spline function.
inline Pnt3f TrainView::Linear(Pnt3f p1, Pnt3f p2, float t) {
    return (1 - t) * p1 + t * p2;
}
inline Pnt3f TrainView::Cardinal(Pnt3f p0, Pnt3f p1, Pnt3f p2, Pnt3f p3, float t) {
	Pnt3f result;
	Pnt3f G[4] = { p0,p1,p2,p3 };
	Pnt3f subResult[4];
	float M[4][4] = {
		{-1, 2,-1, 0},
		{ 3,-5, 0, 2},
		{-3, 4, 1, 0},
		{ 1,-1, 0, 0}
	};
	float T[4] = {t*t*t,t*t,t,1};
	for (int i = 0; i < 4; ++i) {
		subResult[i] = G[0] * M[0][i] + G[1] * M[1][i] + G[2] * M[2][i] + G[3] * M[3][i];
		subResult[i] = subResult[i] * 0.5;
	}
	result = subResult[0] * T[0] + subResult[1] * T[1] + subResult[2] * T[2] + subResult[3] * T[3];

    return result;
}
inline Pnt3f TrainView::Cubic(Pnt3f p0, Pnt3f p1, Pnt3f p2, Pnt3f p3, float t) {
	Pnt3f result;
	Pnt3f G[4] = { p0,p1,p2,p3 };
	Pnt3f subResult[4];
	float M[4][4] = {
		{ -1, 3,-3, 1 },
		{ 3,-6, 0, 4 },
		{ -3, 3, 3, 1 },
		{ 1, 0, 0, 0 }
	};
	float T[4] = { t*t*t,t*t,t,1 };
	for (int i = 0; i < 4; ++i) {
		subResult[i] = G[0] * M[0][i] + G[1] * M[1][i] + G[2] * M[2][i] + G[3] * M[3][i];
		subResult[i] = subResult[i] * (1.0/6.0);
	}
	result = subResult[0] * T[0] + subResult[1] * T[1] + subResult[2] * T[2] + subResult[3] * T[3];

	return result;
}


void TrainView::trainCamView(float aspect) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, aspect, 1, 2000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Rotation.
    QMatrix4x4 rotateMatrix;
    rotateMatrix.rotate(this->horizontalDir, this->trainBasisY.x, this->trainBasisY.y, this->trainBasisY.z); // Rotate Y.
    rotateMatrix.rotate(this->verticalDir, trainBasisX.x, trainBasisX.y, trainBasisX.z); // Rotate X.
    Pnt3f direction = this->trainBasisZ;
    QVector3D v(direction.x, direction.y, direction.z);
    v = rotateMatrix * v;
    direction.x = v.x();
    direction.y = v.y();
    direction.z = v.z();
    // 固定位移
    Pnt3f offset;
    const float OFFSET_X = 0.0f;
    const float OFFSET_Y = 10.0f;
    const float OFFSET_Z = 0.0f;
    offset = trainBasisX * OFFSET_X + this->trainBasisY * OFFSET_Y + this->trainBasisZ * OFFSET_Z;

    Pnt3f pos = this->trainPos + offset;
    Pnt3f center = this->trainPos + offset + direction;
    v = QVector3D(this->trainBasisY.x, this->trainBasisY.y, this->trainBasisY.z);
    v = rotateMatrix * v;
    Pnt3f up(v.x(), v.y(), v.z());
    //Pnt3f up = this->trainBasisY + direction;
    gluLookAt(pos.x, pos.y, pos.z, center.x, center.y, center.z, up.x, up.y, up.z);
}

void TrainView::loadTexture2D(QString str, GLuint &textureID, bool repeat) {
    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);


    QImage img(str);
    QImage opengl_grass = QGLWidget::convertToGLFormat(img);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, opengl_grass.width(), opengl_grass.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, opengl_grass.bits());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    if (repeat) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }
    glDisable(GL_TEXTURE_2D);
}