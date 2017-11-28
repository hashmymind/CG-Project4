#include "model.h"
#include "Utilities\3DUtils.h"

#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QVarLengthArray>

#include <QtOpenGL/QtOpenGL>
#include<iostream>
using namespace std;
Point3d boundsMin(1e9, 1e9, 1e9);
Point3d boundsMax(-1e9, -1e9, -1e9);
Model::Model(const QString &filePath, int s, Point3d p)
    : m_fileName(QFileInfo(filePath).fileName()) {
	this->posi = p;
	this->scale = s;
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
        return;
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString input = in.readLine();
        if (input.isEmpty() || input[0] == '#')
            continue;

        QTextStream ts(&input);
        QString id;
        ts >> id;
        if (id == "v") {
            Point3d p;
            for (int i = 0; i < 3; ++i) {
                ts >> p[i];
                boundsMin[i] = qMin(boundsMin[i], p[i]);
                boundsMax[i] = qMax(boundsMax[i], p[i]);
            }
            m_points << p;
        } else if (id == "vt") {

        } else if (id == "f" || id == "fo") {
            QVarLengthArray<int, 4> p;

            while (!ts.atEnd()) {
                QString vertex;
                ts >> vertex;
                const int vertexIndex = vertex.split('/').value(0).toInt();
                if (vertexIndex)
                    p.append(vertexIndex > 0 ? vertexIndex - 1 : m_points.size() + vertexIndex);
            }

            for (int i = 0; i < p.size(); ++i) {
                const int edgeA = p[i];
                const int edgeB = p[(i + 1) % p.size()];

                if (edgeA < edgeB)
                    m_edgeIndices << edgeA << edgeB;
            }

            for (int i = 0; i < 3; ++i)
                m_pointIndices << p[i];

            if (p.size() == 4)
                for (int i = 0; i < 3; ++i)
                    m_pointIndices << p[(i + 2) % 4];
        }
    }

	this->update();


}
void Model::set_base(Pnt3f x, Pnt3f y, Pnt3f z) {
    Pnt3f p;
    for (int i = 0; i < m_points.size(); ++i) {
        p = x*m_points[i].x + y*m_points[i].y + z*m_points[i].z;
        n_points[i] = Point3d(p.x, p.y, p.z);
    }
    for (int i = 0; i < m_normals.size(); ++i) {
        p = x*m_normals[i].x + y*m_normals[i].y + z*m_normals[i].z;
        n_normals[i] = Point3d(p.x, p.y, p.z);
    }
}

void Model::update() {
	int s = this->scale;
	Point3d p = this->posi;
	const Point3d bounds = boundsMax - boundsMin;
	const qreal scale = s / qMax(bounds.x, qMax(bounds.y, bounds.z));

	n_points.clear();
	for (int i = 0; i < m_points.size(); ++i) {
		n_points.push_back((m_points[i] + p - (boundsMin + bounds * 0.5)) * scale);
	}
}

void Model::setPosi(Point3d p) {
	this->posi = p;
}
void Model::setScale(int s) {
	this->scale = s;
}

void Model::draw() {
	this->update();
    //glEnable(GL_DEPTH_TEST);
    glEnableClientState(GL_VERTEX_ARRAY);
    /*	glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
        glEnable(GL_COLOR_MATERIAL);
        glShadeModel(GL_SMOOTH);
	*/
     glEnableClientState(GL_NORMAL_ARRAY);
     glVertexPointer(3, GL_FLOAT, 0, (float *) n_points.data());
     glNormalPointer(GL_FLOAT, 0, (float *) n_normals.data());
     glDrawElements(GL_TRIANGLES, m_pointIndices.size(), GL_UNSIGNED_INT, m_pointIndices.data());
     glDisableClientState(GL_NORMAL_ARRAY);
    /*  glDisable(GL_COLOR_MATERIAL);
        glDisable(GL_LIGHT0);
        glDisable(GL_LIGHTING);
	*/
    glDisableClientState(GL_VERTEX_ARRAY);
    //glDisable(GL_DEPTH_TEST);
}