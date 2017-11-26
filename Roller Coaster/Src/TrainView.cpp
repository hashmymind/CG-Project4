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

	// draw the train
    if (this->camera != 2) { // don't draw the train if you're looking out the front window
		drawTrain();
    }
}
void TrainView::drawTrack(bool doingShadows) {
    spline_t type_spline = (spline_t) this->curve;
    for (size_t i = 0; i < this->m_pTrack->points.size(); ++i) {
        // pos
        Pnt3f cp_pos[4] = {
            this->m_pTrack->points[((i - 1) + this->m_pTrack->points.size()) % this->m_pTrack->points.size()].pos, // for G
            this->m_pTrack->points[i].pos,
            this->m_pTrack->points[(i + 1) % this->m_pTrack->points.size()].pos,
            this->m_pTrack->points[(i + 2) % this->m_pTrack->points.size()].pos // for G
        };
        // orient
        Pnt3f cp_orient[4] = {
            this->m_pTrack->points[((i - 1) + this->m_pTrack->points.size()) % this->m_pTrack->points.size()].orient, // for G
            this->m_pTrack->points[i].orient,
            this->m_pTrack->points[(i + 1) % this->m_pTrack->points.size()].orient,
            this->m_pTrack->points[(i + 2) % this->m_pTrack->points.size()].orient // for G
        };

        float percent = 1.0f / DIVIDE_LINE;
        float t = 0;

        Pnt3f qt = cp_pos[1];
        for (size_t j = 0; j < DIVIDE_LINE; j++) {
            Pnt3f orient_t;
            Pnt3f qt0 = qt;
            switch (type_spline) {
                case spline_Linear:
                    orient_t = Linear(cp_orient[1], cp_orient[2], t);
                    qt = Linear(cp_pos[1], cp_pos[2], t += percent);
                    break;
                case spline_Cardinal:
                    orient_t = Cardinal(cp_orient[0], cp_orient[1], cp_orient[2], cp_orient[3], t);
                    qt = Cardinal(cp_pos[0], cp_pos[1], cp_pos[2], cp_pos[3], t += percent);
                    break;
                case spline_Cubic:
                    orient_t = Cubic(cp_orient[0], cp_orient[1], cp_orient[2], cp_orient[3], t);
                    qt = Cubic(cp_pos[0], cp_pos[1], cp_pos[2], cp_pos[3], t += percent);
                    break;
            }
            Pnt3f qt1 = qt;
            // cross
            orient_t.normalize();
            Pnt3f cross_t = (qt1 - qt0) * orient_t;
            cross_t.normalize();
            cross_t = cross_t * 2.5f;
            // Draw.
            glLineWidth(3);
            glBegin(GL_LINES);
            if (!doingShadows) {
                glColor3ub(32, 32, 64);
            }
            glVertex3f(qt0.x + cross_t.x, qt0.y + cross_t.y, qt0.z + cross_t.z);
            glVertex3f(qt1.x + cross_t.x, qt1.y + cross_t.y, qt1.z + cross_t.z);

            glVertex3f(qt0.x - cross_t.x, qt0.y - cross_t.y, qt0.z - cross_t.z);
            glVertex3f(qt1.x - cross_t.x, qt1.y - cross_t.y, qt1.z - cross_t.z);
            glEnd();
            glLineWidth(1);
        }
    }
}

void TrainView::drawTrain() {
    float t = this->t_time;
    spline_t type_spline = (spline_t) this->curve;
    t *= m_pTrack->points.size();
    size_t i;
    for (i = 0; t > 1; t -= 1) { i++; }
    // pos
    Pnt3f cp_pos[4] = {
        this->m_pTrack->points[((i - 1) + this->m_pTrack->points.size()) % this->m_pTrack->points.size()].pos, // for G
        this->m_pTrack->points[i].pos,
        this->m_pTrack->points[(i + 1) % this->m_pTrack->points.size()].pos,
        this->m_pTrack->points[(i + 2) % this->m_pTrack->points.size()].pos // for G
    };
    // orient
    Pnt3f cp_orient[4] = {
        this->m_pTrack->points[((i - 1) + this->m_pTrack->points.size()) % this->m_pTrack->points.size()].orient, // for G
        this->m_pTrack->points[i].orient,
        this->m_pTrack->points[(i + 1) % this->m_pTrack->points.size()].orient,
        this->m_pTrack->points[(i + 2) % this->m_pTrack->points.size()].orient // for G
    };

    Pnt3f qt, orient_t;
    switch (type_spline) {
        case spline_Linear:
            qt = Linear(cp_pos[1], cp_pos[2], t);
            orient_t = Linear(cp_orient[1], cp_orient[2], t);
            break;
        case spline_Cardinal:
            qt = Cardinal(cp_pos[0], cp_pos[1], cp_pos[2], cp_pos[3], t);
            orient_t = Cardinal(cp_orient[0], cp_orient[1], cp_orient[2], cp_orient[3], t);
            break;
        case spline_Cubic:
            qt = Cubic(cp_pos[0], cp_pos[1], cp_pos[2], cp_pos[3], t);
            orient_t = Cubic(cp_orient[0], cp_orient[1], cp_orient[2], cp_orient[3], t);
            break;
    }
    // Draw.
    glColor3ub(255, 255, 255);
    glBegin(GL_QUADS);
    // [TODO] draw train.
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(qt.x - 5, qt.y - 5, qt.z - 5);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(qt.x + 5, qt.y - 5, qt.z - 5);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(qt.x + 5, qt.y + 5, qt.z - 5);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(qt.x - 5, qt.y + 5, qt.z - 5);
    glEnd();
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

inline Pnt3f TrainView::Linear(Pnt3f p1, Pnt3f p2, float t) {
    return (1 - t) * p1 + t * p2;
}
// [TODO]
inline Pnt3f TrainView::Cardinal(Pnt3f p0, Pnt3f p1, Pnt3f p2, Pnt3f p3, float t) {
    return Pnt3f();
}
// [TODO]
inline Pnt3f TrainView::Cubic(Pnt3f p0, Pnt3f p1, Pnt3f p2, Pnt3f p3, float t) {
    return Pnt3f();
}
// [TODO]
void TrainView::trainCamView(float aspect) {
}
