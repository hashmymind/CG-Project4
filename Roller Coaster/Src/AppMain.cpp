#include "AppMain.h"

#include "Utilities/3DUtils.H"
#include "Track.H"
#include <math.h>
#include <time.h>
#include <iostream>


AppMain* AppMain::Instance = NULL;
AppMain::AppMain(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	trainview = new TrainView();  
	trainview->m_pTrack =  &m_Track;
	setGeometry(100,25,1000,768);   
	ui.mainLayout->layout()->addWidget(trainview);
	trainview->installEventFilter(this);
    this->trainview->setFocusPolicy(Qt::StrongFocus);
    this->canpan = false;
	this->isHover = false;
	this->trainview->camera = 0;
	this->trainview->track = 0;
	this->trainview->curve = 0;
	this->trainview->isrun = false;
    this->trainview->tPos = 0;
    this->trainview->verticalDir = 0;
    this->trainview->horizontalDir = 0;
    this->trainview->velocity = this->trainview->oriVelocity = (this->trainview->minVelocity + this->trainview->maxVelocity) / 2;
    this->lockCursor = false;
    this->mOffsetX = 0;
    this->mOffsetY = 0;
    this->mouseX = 0;
    this->mouseY = 0;

    // Timer.
    timer = new QTimer(this);
    this->timer->start(20);
    connect(timer, SIGNAL(timeout()), this, SLOT(TrainRun()));

	setWindowTitle( "Roller Coaster" );

	connect( ui.aLoadPath	,SIGNAL(triggered()),this,SLOT(LoadTrackPath())	);
	connect( ui.aSavePath	,SIGNAL(triggered()),this,SLOT(SaveTrackPath())	);
	connect( ui.aExit		,SIGNAL(triggered()),this,SLOT(ExitApp())		);

	connect( ui.comboCamera	,SIGNAL(currentIndexChanged(QString)),this,SLOT(ChangeCameraType(QString)));
	connect( ui.aWorld		,SIGNAL(triggered()),this,SLOT(ChangeCamToWorld())	);
	connect( ui.aTop		,SIGNAL(triggered()),this,SLOT(ChangeCamToTop())	);
	connect( ui.aTrain		,SIGNAL(triggered()),this,SLOT(ChangeCamToTrain())	);

	connect( ui.comboCurve	,SIGNAL(currentIndexChanged(QString)),this,SLOT(ChangeCurveType(QString)));
	connect( ui.aLinear		,SIGNAL(triggered()),this,SLOT(ChangeCurveToLinear())	);
	connect( ui.aCardinal	,SIGNAL(triggered()),this,SLOT(ChangeCurveToCardinal())	);
	connect( ui.aCubic		,SIGNAL(triggered()),this,SLOT(ChangeCurveToCubic())	);

	connect( ui.comboTrack	,SIGNAL(currentIndexChanged(QString)),this,SLOT(ChangeTrackType(QString)));
	connect( ui.aLine		,SIGNAL(triggered()),this,SLOT(ChangeTrackToLine())		);
	connect( ui.aTrack		,SIGNAL(triggered()),this,SLOT(ChangeTrackToTrack())	);
	connect( ui.aRoad		,SIGNAL(triggered()),this,SLOT(ChangeTrackToRoad())		);

	connect( ui.bPlay		,SIGNAL(clicked()),this,SLOT(SwitchPlayAndPause())				);
	connect( ui.sSpeed		,SIGNAL(valueChanged(int)),this,SLOT(ChangeSpeedOfTrain(int))	);
	connect( ui.bAdd		,SIGNAL(clicked()),this,SLOT(AddControlPoint())					);
	connect( ui.bDelete		,SIGNAL(clicked()),this,SLOT(DeleteControlPoint())				);

	connect( ui.rcpxadd		,SIGNAL(clicked()),this,SLOT(RotateControlPointAddX())					);
	connect( ui.rcpxsub		,SIGNAL(clicked()),this,SLOT(RotateControlPointSubX())				);
	connect( ui.rcpzadd		,SIGNAL(clicked()),this,SLOT(RotateControlPointAddZ())					);
	connect( ui.rcpzsub		,SIGNAL(clicked()),this,SLOT(RotateControlPointSubZ())				);

    connect(ui.actionSlow, SIGNAL(triggered()), this, SLOT(ChangeSpeedSlow()));
    connect(ui.actionNormal, SIGNAL(triggered()), this, SLOT(ChangeSpeedNormal()));
    connect(ui.actionFast, SIGNAL(triggered()), this, SLOT(ChangeSpeedFast()));


}

AppMain::~AppMain()
{

}

bool AppMain::eventFilter(QObject *watched, QEvent *e) {
    // Train View Camera Control.
    if (this->trainview->camera == 2) {
        this->UpdateMouse();
        this->trainview->horizontalDir += mOffsetX / -8.0f;
        this->trainview->verticalDir +=  mOffsetY / -8.0f;
        if (this->trainview->verticalDir > 60) { this->trainview->verticalDir = 60; }
        else if (this->trainview->verticalDir < -60) { this->trainview->verticalDir = -60; }
    }
	if (e->type() == QEvent::MouseButtonPress) {
		QMouseEvent *event = static_cast<QMouseEvent*> (e);
		// Get the mouse position
		float x, y;
		trainview->arcball.getMouseNDC((float)event->localPos().x(), (float)event->localPos().y(), x,y);

		// Compute the mouse position
		trainview->arcball.down(x, y);
		if(event->button()==Qt::LeftButton){
			trainview->doPick(event->localPos().x(), event->localPos().y());
			this->isHover = true;
			if(this->canpan)
				trainview->arcball.mode = trainview->arcball.Pan;
		}
		if(event->button()==Qt::RightButton){
			trainview->arcball.mode = trainview->arcball.Rotate;
		}
	}

	if (e->type() == QEvent::MouseButtonRelease) {
		this->canpan = false;
		this->isHover = false;
		trainview->arcball.mode = trainview->arcball.None;
	}

	if (e->type() == QEvent::Wheel) {
		QWheelEvent *event = static_cast<QWheelEvent*> (e);
		float zamt = (event->delta() < 0) ? 1.1f : 1/1.1f;
		trainview->arcball.eyeZ *= zamt;
	}

	if (e->type() == QEvent::MouseMove) {
		QMouseEvent *event = static_cast<QMouseEvent*> (e);
		if(isHover && trainview->selectedCube >= 0){
			ControlPoint* cp = &trainview->m_pTrack->points[trainview->selectedCube];

			double r1x, r1y, r1z, r2x, r2y, r2z;
			int x = event->localPos().x();
			int iy = event->localPos().y();
			double mat1[16],mat2[16];		// we have to deal with the projection matrices
			int viewport[4];

			glGetIntegerv(GL_VIEWPORT, viewport);
			glGetDoublev(GL_MODELVIEW_MATRIX,mat1);
			glGetDoublev(GL_PROJECTION_MATRIX,mat2);

			int y = viewport[3] - iy; // originally had an extra -1?

			int i1 = gluUnProject((double) x, (double) y, .25, mat1, mat2, viewport, &r1x, &r1y, &r1z);
			int i2 = gluUnProject((double) x, (double) y, .75, mat1, mat2, viewport, &r2x, &r2y, &r2z);

			double rx, ry, rz;
			mousePoleGo(r1x, r1y, r1z, r2x, r2y, r2z, 
				static_cast<double>(cp->pos.x), 
				static_cast<double>(cp->pos.y),
				static_cast<double>(cp->pos.z),
				rx, ry, rz,
				false);

			cp->pos.x = (float) rx;
			cp->pos.y = (float) ry;
			cp->pos.z = (float) rz;
		}
		if(trainview->arcball.mode != trainview->arcball.None) { // we're taking the drags
			float x,y;
			trainview->arcball.getMouseNDC((float)event->localPos().x(), (float)event->localPos().y(),x,y);
			trainview->arcball.computeNow(x,y);
		};
	}
    // Hotkeys.
	if(e->type() == QEvent::KeyPress){
		 QKeyEvent *event = static_cast< QKeyEvent*> (e);
         switch (event->key()) {
             // Set up the mode
             case Qt::Key_Alt:
                 this->canpan = true;
                 break;
             // Lock cursor.
             case Qt::Key_Escape:
                 this->lockCursor = !this->lockCursor;
                 break;
             case Qt::Key_Q:
                 this->NextCameraType();
                 break;
             case Qt::Key_E:
                 this->NextTrackType();
                 break;
             case Qt::Key_W:
                 this->NextCurveType();
                 break;
             case Qt::Key_R:
                 this->SwitchPlayAndPause();
                 break;
             case Qt::Key_P:
                 //Particle::ProcessParticles();
                 break;
         }
		
	}

	return QWidget::eventFilter(watched, e);
}

void AppMain::ExitApp()
{
	QApplication::quit();
}

AppMain * AppMain::getInstance()
{
	if( !Instance )
	{
		Instance = new AppMain();
		return Instance;
	}
	else 
		return Instance;
}

void AppMain::CheckSelected() {
    if (trainview->selectedCube >= ((int) m_Track.points.size()))
        trainview->selectedCube = 0;
}

void AppMain::ToggleMenuBar()
{
	ui.menuBar->setHidden( !ui.menuBar->isHidden() );
}

void AppMain::ToggleToolBar()
{
	ui.mainToolBar->setHidden( !ui.mainToolBar->isHidden() );
}

void AppMain::ToggleStatusBar()
{
	ui.statusBar->setHidden( !ui.statusBar->isHidden() );
}

void AppMain::LoadTrackPath()
{
	QString fileName = QFileDialog::getOpenFileName( 
		this,
		"OpenImage",
		"./",
		tr("Txt (*.txt)" )
		);
	QByteArray byteArray = fileName.toLocal8Bit();
	const char* fname = byteArray.data();
	if ( !fileName.isEmpty() )
	{
		this->m_Track.readPoints(fname);
	}
}

void AppMain::SaveTrackPath()
{
	QString fileName = QFileDialog::getSaveFileName( 
		this,
		"OpenImage",
		"./",
		tr("Txt (*.txt)" )
		);

	QByteArray byteArray = fileName.toLocal8Bit();
	const char* fname = byteArray.data();
	if ( !fileName.isEmpty() )
	{
		this->m_Track.writePoints(fname);
	}
}

void AppMain::TogglePanel()
{
	if( !ui.groupCamera->isHidden() )
	{
		ui.groupCamera->hide();
		ui.groupCurve->hide();
		ui.groupTrack->hide();
		ui.groupPlay->hide();
		ui.groupCP->hide();
	}
	else
	{
		ui.groupCamera->show();
		ui.groupCurve->show();
		ui.groupTrack->show();
		ui.groupPlay->show();
		ui.groupCP->show();
	}
}

void AppMain::ChangeCameraType( QString type )
{
	if( type == "World" )
	{
		this->trainview->camera = 0;
		update();
	}
	else if( type == "Top" )
	{
		this->trainview->camera = 1;
		update();
	}
	else if( type == "Train" )
	{
		this->trainview->camera = 2;
		update();
	}
    UpdateCameraState(this->trainview->camera);
}

void AppMain::ChangeCurveType( QString type )
{
	if( type == "Linear" )
	{
		this->trainview->curve = 0;
	}
	else if( type == "Cardinal" )
	{
		this->trainview->curve = 1;
	}
	else if( type == "Cubic" )
	{
		this->trainview->curve = 2;
	}
    UpdateCurveState(this->trainview->curve);
}

void AppMain::ChangeTrackType( QString type )
{
	if( type == "Line" )
	{
		this->trainview->track = 0;
	}
	else if( type == "Track" )
	{
		this->trainview->track = 1;
	}
	else if( type == "Road" )
	{
		this->trainview->track = 2;
	}
    UpdateTrackState(this->trainview->track);
}

static unsigned long lastRedraw = 0;
void AppMain::SwitchPlayAndPause()
{
	this->trainview->isrun = !(this->trainview->isrun);
	ui.bPlay->setIcon(!this->trainview->isrun ? QIcon(":/AppMain/Resources/Icons/play.ico") : QIcon(":/AppMain/Resources/Icons/pause.ico"));
    if (this->trainview->isrun) { this->trainview->velocity = this->trainview->oriVelocity; }
    std::cout << (this->trainview->isrun ? "Run\n" : "Stop\n");
}

void AppMain::ChangeSpeedOfTrain( int val )
{
    // val in 0 ~ 99
    this->trainview->oriVelocity = this->trainview->minVelocity + (this->trainview->maxVelocity - this->trainview->minVelocity) * ((float) val / 100.0f);
    this->trainview->velocity = this->trainview->oriVelocity;
}

void AppMain::AddControlPoint()
{
	// get the number of points
	size_t npts = this->m_Track.points.size();
	// the number for the new point
	size_t newidx = (this->trainview->selectedCube>=0) ? this->trainview->selectedCube : 0;

	// pick a reasonable location
	size_t previdx = (newidx + npts -1) % npts;
	Pnt3f npos = (this->m_Track.points[previdx].pos + this->m_Track.points[newidx].pos) * .5f;

	this->m_Track.points.insert(this->m_Track.points.begin() + newidx,npos);

	// make it so that the train doesn't move - unless its affected by this control point
	// it should stay between the same points
	if (ceil(this->m_Track.trainU) > ((float)newidx)) {
		this->m_Track.trainU += 1;
		if (this->m_Track.trainU >= npts) this->m_Track.trainU -= npts;
	}
	this->CheckSelected();
}

void AppMain::DeleteControlPoint()
{
	if (this->m_Track.points.size() > 4) {
		if (this->trainview->selectedCube >= 0) {
			this->m_Track.points.erase(this->m_Track.points.begin() + this->trainview->selectedCube);
		} else
			this->m_Track.points.pop_back();
	}
	this->CheckSelected();
}


//***************************************************************************
//
// * Rotate the selected control point about x axis
//===========================================================================
void AppMain::rollx(float dir)
{
	int s = this->trainview->selectedCube;
	if (s >= 0) {
		Pnt3f old = this->m_Track.points[s].orient;
		float si = sin(((float)M_PI_4) * dir);
		float co = cos(((float)M_PI_4) * dir);
		this->m_Track.points[s].orient.y = co * old.y - si * old.z;
		this->m_Track.points[s].orient.z = si * old.y + co * old.z;
	}
	this->CheckSelected();
} 

void AppMain::RotateControlPointAddX()
{
	rollx(1);
}

void AppMain::RotateControlPointSubX()
{
	rollx(-1);
}

void AppMain::rollz(float dir)
{
	int s = this->trainview->selectedCube;
	if (s >= 0) {

		Pnt3f old = this->m_Track.points[s].orient;

		float si = sin(((float)M_PI_4) * dir);
		float co = cos(((float)M_PI_4) * dir);

		this->m_Track.points[s].orient.y = co * old.y - si * old.x;
		this->m_Track.points[s].orient.x = si * old.y + co * old.x;
	}
	this->CheckSelected();
} 

void AppMain::RotateControlPointAddZ()
{
	rollz(1);
}

void AppMain::RotateControlPointSubZ()
{
	rollz(-1);
}

void AppMain::ChangeSpeedSlow() {
    ui.sSpeed->setValue(0);
}

void AppMain::ChangeSpeedNormal() {
    ui.sSpeed->setValue(50);
}

void AppMain::ChangeSpeedFast() {
    ui.sSpeed->setValue(99);
}

void AppMain::ChangeCamToWorld()
{
    this->ui.comboCamera->setCurrentText("World");
}

void AppMain::ChangeCamToTop()
{
    this->ui.comboCamera->setCurrentText("Top");
}

void AppMain::ChangeCamToTrain()
{
    this->ui.comboCamera->setCurrentText("Train");
}

void AppMain::ChangeCurveToLinear()
{
    this->ui.comboCurve->setCurrentText("Linear");
}
void AppMain::ChangeCurveToCardinal()
{
    this->ui.comboCurve->setCurrentText("Cardinal");
}

void AppMain::ChangeCurveToCubic()
{
    this->ui.comboCurve->setCurrentText("Cubic");
}

void AppMain::ChangeTrackToLine()
{
    this->ui.comboTrack->setCurrentText("Line");
}

void AppMain::ChangeTrackToTrack()
{
    this->ui.comboTrack->setCurrentText("Track");
}

void AppMain::ChangeTrackToRoad()
{
    this->ui.comboTrack->setCurrentText("Road");
}

void AppMain::NextCameraType() {
    this->ui.comboCamera->setCurrentIndex((this->ui.comboCamera->currentIndex() + 1) % 3);
}

void AppMain::NextCurveType() {
    this->ui.comboCurve->setCurrentIndex((this->ui.comboCurve->currentIndex() + 1) % 3);
}

void AppMain::NextTrackType() {
    this->ui.comboTrack->setCurrentIndex((this->ui.comboTrack->currentIndex() + 1) % 3);
}

void AppMain::UpdateCameraState( int index )
{
	ui.aWorld->setChecked( (index==0)?true:false );
	ui.aTop	 ->setChecked( (index==1)?true:false );
	ui.aTrain->setChecked( (index==2)?true:false );
}

void AppMain::UpdateCurveState( int index )
{
	ui.aLinear	->setChecked( (index==0)?true:false );
	ui.aCardinal->setChecked( (index==1)?true:false );
	ui.aCubic	->setChecked( (index==2)?true:false );
}

void AppMain::UpdateTrackState( int index )
{
	ui.aLine ->setChecked( (index==0)?true:false );
	ui.aTrack->setChecked( (index==1)?true:false );
	ui.aRoad ->setChecked( (index==2)?true:false );
}
void AppMain::TrainRun() {
    if (this->trainview->isrun) {
        this->trainview->tPos = this->trainview->advanceTrain(); // Advance train.
        this->trainview->trainGravity(); // Gravity.
    }
}


void AppMain::UpdateMouse() {
    static bool last;
    auto p = this->mapFromGlobal(this->cursor().pos());
    mOffsetX = p.x() - mouseX;
    mOffsetY = p.y() - mouseY;
    if (this->isActiveWindow() && lockCursor) {
        if (!last) { ShowCursor(false); }
        last = true;
        SetCursorPos(this->pos().x() + this->size().width() / 2, this->pos().y() + this->size().height() / 2);
    } else {
        if (last) { ShowCursor(true); }
        last = false;
    }
    p = this->mapFromGlobal(this->cursor().pos());
    mouseX = p.x();
    mouseY = p.y();
}

