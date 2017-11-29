#include "TrainView.h"  


TrainView::TrainView(QWidget *parent) :  
QGLWidget(parent)  
{  
	resetArcball();
}  
TrainView::~TrainView()  
{}  
void TrainView::initializeGL()
{
	m = new Model(QString("mod/train.obj"), 18, Point3d());
	initializeOpenGLFunctions();
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

	// this time drawing is for shadows (except for top view)
	if (this->camera != 1) {
		setupShadows();
		drawStuff(true);
		unsetupShadows();
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
	// Or do the train view or other view here
	//####################################################################
	// TODO: 
	// put code for train view projection here!	
	//####################################################################
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
	drawTrain(this->camera != 2);// don't draw the train if you're looking out the front window
    //ParticleSpace::DrawParticles();
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

		float partialLen = 0,intervalCount = 0;

        Pnt3f qt, orient;
        for (size_t j = 0; j < DIVIDE_LINE; j++) {
            t += percent;
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
			if (j == 0)continue;
            glLineWidth(5);
            glBegin(GL_LINES);
            if (!doingShadows) {
                // Spline color.
                glColor3ub(66, 22, 0);
            }
            glVertex3f(qt0.x + cross.x, qt0.y + cross.y, qt0.z + cross.z);
            glVertex3f(qt1.x + cross.x, qt1.y + cross.y, qt1.z + cross.z);

            glVertex3f(qt0.x - cross.x, qt0.y - cross.y, qt0.z - cross.z);
            glVertex3f(qt1.x - cross.x, qt1.y - cross.y, qt1.z - cross.z);
            glEnd();
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
				glVertex3f(qt0.x - cross.x, qt0.y - cross.y, qt0.z - cross.z);
				glVertex3f(qt0.x + cross.x, qt0.y + cross.y, qt0.z + cross.z);
				glVertex3f(qt0.x + cross.x + vqt.x, qt0.y + cross.y + vqt.y, qt0.z + cross.z + vqt.z);
				glVertex3f(qt0.x - cross.x+ vqt.x, qt0.y - cross.y + vqt.y, qt0.z - cross.z + vqt.z);
				glEnd();

			}
			else if (this->track == 2) {
                if (!doingShadows) {
                    // Track color.
                    glColor3ub(32, 32, 64);
                }
				// plane
				glBegin(GL_POLYGON);
				Pnt3f vqt = (qt0 - qt1);
				cross = cross * 1.4f;
				glVertex3f(qt0.x - cross.x, qt0.y - cross.y, qt0.z - cross.z);
				glVertex3f(qt0.x + cross.x, qt0.y + cross.y, qt0.z + cross.z);
				glVertex3f(qt0.x + cross.x + vqt.x, qt0.y + cross.y + vqt.y, qt0.z + cross.z + vqt.z);
				glVertex3f(qt0.x - cross.x + vqt.x, qt0.y - cross.y + vqt.y, qt0.z - cross.z + vqt.z);
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
float TrainView::advanceTrain() {
    float t = this->tPos;
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
void TrainView::drawTrain(bool drawingTrain) {
    Pnt3f qt, orient;
    calcTrain(qt, orient, this->tPos);
    // Update cureent train coordinate.
    this->trainPos = qt;
    this->trainBasisY = orient;
    this->trainBasisY.normalize();
    // Update train direction.
    float nextT = advanceTrain();
    calcTrain(qt, orient, nextT);
    this->trainBasisZ = qt - this->trainPos;
    this->trainBasisZ.normalize();
    // Cross get X.
    this->trainBasisX = this->trainBasisZ * this->trainBasisY;
    this->trainBasisX.normalize();
    // Draw.
    if (drawingTrain) {
		glColor3ub(60, 60, 60);
        
        
		
       
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
		m->set_base(this->trainBasisX, this->trainBasisY, -1*this->trainBasisZ);
		m->setPosi(Point3d(qt.x, qt.y + 5, qt.z));
		glColor3ub(60, 60, 60);
		m->draw();
        
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
    const float OFFSET_Y = 2.0f;
    const float OFFSET_Z = 0.0f;
    offset = trainBasisX * OFFSET_X + this->trainBasisY * OFFSET_Y + this->trainBasisZ * OFFSET_Z;

    Pnt3f pos = this->trainPos + offset;
    Pnt3f center = this->trainPos + offset + direction;
    Pnt3f up = this->trainBasisY;
    gluLookAt(pos.x, pos.y, pos.z, center.x, center.y, center.z, up.x, up.y, up.z);
}
