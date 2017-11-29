#ifndef MODEL_H
#define MODEL_H

#include <QtCore/QString>
#include <QtCore/QVector>

#include <math.h>

#include "Point3d.h"
#include "Utilities/Pnt3f.H"
#include "ControlPoint.H"

class Model {
public:

    Model() {}
    Model(const QString &filePath, int s, Point3d p);


    void draw();
	void setPosi(Point3d);
	void setScale(int);
	void update();
	void setOffset(Point3d);

    void set_base(Pnt3f x, Pnt3f y, Pnt3f z);
    QString fileName() const { return m_fileName; }
    int faces() const { return m_pointIndices.size() / 3; }
    int edges() const { return m_edgeIndices.size() / 2; }
    int points() const { return m_points.size(); }


private:
    QString m_fileName;
	QVector<Point3d> ori_m_points;
    QVector<Point3d> m_points;
    QVector<Point3d> m_normals;
    QVector<int> m_edgeIndices;
    QVector<int> m_pointIndices;
    QVector<Point3d> n_points;
    QVector<Point3d> n_normals;
	int scale;
	Point3d posi;
	Point3d offset;
	Pnt3f bx, by, bz;
};

#endif