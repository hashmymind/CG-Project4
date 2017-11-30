#ifndef MODEL_H
#define MODEL_H
#pragma once
#include <QtCore/QString>
#include <QtCore/QVector>

#include <math.h>

#include "Point3d.h"
#include "Utilities/Pnt3f.H"
#include "ControlPoint.H"
struct UV {
    float x, y;
};
class Model {
public:

    Model() {}
    Model(const QString &filePath, int _textureID, float _scale, Point3d offset, Pnt3f _rgb);


    void Draw(bool doingShadows = false);
    void setBasis(Pnt3f x, Pnt3f y, Pnt3f z);
	void setPosi(Point3d);
	void setScale(int);
	void setOffset(Point3d);

    QString fileName() const { return m_fileName; }
    int faces() const { return m_pointIndices.size() / 3; }
    int edges() const { return m_edgeIndices.size() / 2; }
    int points() const { return m_points.size(); }


protected:
    QString m_fileName;
	QVector<Point3d> ori_m_points;
    QVector<Point3d> m_points;
    QVector<Point3d> m_normals;
    QVector<int> m_edgeIndices;
    QVector<int> m_pointIndices;
    QVector<Point3d> n_points;
    QVector<Point3d> n_normals;
    QVector<UV> uvs;
    int textureID;

	float scale;
	Point3d posi;
	Point3d offset;
	Pnt3f bx, by, bz;
    Pnt3f rgb;
    void render(bool useTex = false, bool wireframe = false, bool normals = false) const;
};

class Tunnel : public Model {
public:
    float t;

    Tunnel(float t, const QString &filePath, int _textureID, float _scale, Point3d _offset, Pnt3f _rgb);
};
#endif