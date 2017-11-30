#pragma once
#ifndef TRAINVIEW_H  
#define TRAINVIEW_H
#include <QtOpenGL/QGLWidget> 
#include <QtGui/QtGui>  
#include <QtOpenGL/QtOpenGL>  
#include <GL/GLU.h>
#include <vector>
#include <QtGui/QOpenGLFunctions_4_3_Core>
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib") 
#include "Utilities/ArcBallCam.H"
#include "Utilities/3DUtils.H"
#include "Track.H"
#include "Model.h"
#include "Particle.h"

using std::vector;

class AppMain;
class CTrack;

enum spline_t {
    spline_Linear,
    spline_Cardinal,
    spline_Cubic
};

//#######################################################################
// TODO
// You might change the TrainView in order to add different objects to
// be drawn, or adjust the lighting, or ...
//#######################################################################


class TrainView : public QGLWidget, protected QOpenGLFunctions_4_3_Core
{  
	Q_OBJECT  
public:  
	explicit TrainView(QWidget *parent = 0);  
	~TrainView();  

public:
	// overrides of important window things
	//virtual int handle(int);
	virtual void paintGL();

	// all of the actual drawing happens in this routine
	// it has to be encapsulated, since we draw differently if
	// we're drawing shadows (no colors, for example)
	void drawStuff(bool doingShadows=false);
	// setup the projection - assuming that the projection stack has been
	// cleared for you
	void setProjection();

	// Reset the Arc ball control
	void resetArcball();

	// pick a point (for when the mouse goes down)
	void doPick(int mx, int my);

	void initializeGL();

    float advanceTrain(float t);
    void trainGravity();
private:
    inline Pnt3f Linear(Pnt3f p1, Pnt3f p2, float t);
    inline Pnt3f Cardinal(Pnt3f p0, Pnt3f p1, Pnt3f p2, Pnt3f p3, float t);
    inline Pnt3f Cubic(Pnt3f p0, Pnt3f p1, Pnt3f p2, Pnt3f p3, float t);
    void calcPosition(Pnt3f& qt, Pnt3f& orient, Pnt3f cpPos[4], Pnt3f cpOrient[4], float t, spline_t& type_spline);
    void calcTrain(Pnt3f& qt, Pnt3f& orient, float t);
    void drawTrack(bool);
    void drawTrain(bool drawingTrain, bool doingShadows);
    void drawTunnel(bool);
    void trainCamView(float);

    void loadTexture2D(QString str, GLuint &textureID, bool repeat = false);

public:
	ArcBallCam		arcball;			// keep an ArcBall for the UI
	int				selectedCube;  // simple - just remember which cube is selected

	CTrack*			m_pTrack;		// The track of the entire scene

	int camera;
	int curve;
	int track;
	bool isrun;
    // Velocity
    float velocity, oriVelocity;
    const float G = 0.1f; // Gravity.
    const float minimumVelocityRate = 0.4f; // 受重力影響後火車運行的最低速度比率
    // Min & Max(for slider control.)
    const float minVelocity = 0.1f;
    const float maxVelocity = 25.0f;

    float tPos; // 0 ~ 1 代表在整個軌道迴圈的位置
	vector<float> arclen; // use after drawTrack was called
    Model *trainModel;
	vector<Model*> cars;
    Pnt3f trainPos;
    Pnt3f trainBasisX, trainBasisY, trainBasisZ;
    float verticalDir, horizontalDir;
    vector<Tunnel*> tunnels;
    vector<Model*> models;

    ParticleSystem* particle;

	Pnt3f lpt1pc, lpt1mc;


    const int DIVIDE_LINE = 1000;
	const float INTERVAL = 5.0;
	const float TRACK_WIDTH = 2;
	const QString TRAIN_PATH = "mod/train.obj";

    // 開關
    bool renderTunnels, renderParticles, renderModels, renderCars, useGravity;
};  

#endif // TRAINVIEW_H  