#ifndef APPMAIN_H
#define APPMAIN_H

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QWidget>
#include <QtWidgets/QFileDialog>
#include "ui_AppMain.h"
#include <QtOpenGL/qgl.h>
#include "TrainView.h"  
#include "Track.H"

class CTrack;

class AppMain : public QMainWindow
{
	Q_OBJECT

public:
	AppMain(QWidget *parent = 0);
	~AppMain();

	void ToggleMenuBar();
	void ToggleToolBar();
	void ToggleStatusBar();
	void TogglePanel();

	static AppMain *getInstance();
	static AppMain *Instance;

public:
    // call this method when things change
    void CheckSelected();

public:
	// keep track of the stuff in the world
	CTrack				m_Track;

	// the widgets that make up the Window
	TrainView*			trainview;

	bool canpan;
	bool isHover;
private:
    // Mouse offset.
    float mouseX, mouseY, mOffsetX, mOffsetY;
    // Cursor.
    bool lockCursor;

private:
    void NextCameraType();
    void NextCurveType();
    void NextTrackType();
	void UpdateCameraState( int index );
	void UpdateCurveState( int index );
	void UpdateTrackState( int index );
    void UpdateMouse();
	void rollx( float dir );
	void rollz( float dir );
	Ui::AppMainClass ui;

	private slots:
		void LoadTrackPath();
		void SaveTrackPath();
		void ExitApp();

		void ChangeCameraType( QString type );
		void ChangeCamToWorld();
		void ChangeCamToTop();
		void ChangeCamToTrain();

		void ChangeCurveType( QString type );
		void ChangeCurveToLinear();
		void ChangeCurveToCardinal();
		void ChangeCurveToCubic();

		void ChangeTrackType( QString type );
		void ChangeTrackToLine();
		void ChangeTrackToTrack();
		void ChangeTrackToRoad();

		void SwitchPlayAndPause();
		void ChangeSpeedOfTrain( int val );

		void AddControlPoint();
		void DeleteControlPoint();

		void RotateControlPointAddX();
		void RotateControlPointSubX();
		void RotateControlPointAddZ();
		void RotateControlPointSubZ();

        void ChangeSpeedSlow();
        void ChangeSpeedNormal();
        void ChangeSpeedFast();

protected:
	bool eventFilter(QObject *watched, QEvent *e); 
};

#endif // APPMAIN_H
