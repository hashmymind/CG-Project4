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
		}
		else if (id == "vt") {

		}
		else if (id == "f" || id == "fo") {
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

	this->ori_m_points = this->m_points;
	this->update();
}
void Model::set_base(Pnt3f x, Pnt3f y, Pnt3f z) {
	this->bx = x;
	this->by = y;
	this->bz = z;

}

void Model::update() {
	int s = this->scale;
	Point3d p = this->posi;
	const Point3d bounds = boundsMax - boundsMin;
	const qreal sc = s / qMax(bounds.x, qMax(bounds.y, bounds.z));
	Pnt3f tmp;
	n_points.clear();
	p = p + this->offset;
	for (int i = 0; i < ori_m_points.size(); ++i) {
		tmp = this->bx*ori_m_points[i].x + this->by*ori_m_points[i].y + this->bz*ori_m_points[i].z;
		tmp = tmp * sc;
		m_points[i] = (Point3d(tmp.x, tmp.y, tmp.z) + p - (boundsMin + bounds * 0.5));


		n_points.push_back(m_points[i]);
	}
	m_normals.clear();
	m_normals.resize(m_points.size());
	n_normals.clear();

	for (int i = 0; i < m_pointIndices.size(); i += 3) {
		const Point3d a = m_points.at(m_pointIndices.at(i));
		const Point3d b = m_points.at(m_pointIndices.at(i + 1));
		const Point3d c = m_points.at(m_pointIndices.at(i + 2));

		const Point3d normal = cross(b - a, c - a).normalize();

		for (int j = 0; j < 3; ++j)
			m_normals[m_pointIndices.at(i + j)] += normal;
	}

	for (int i = 0; i < m_normals.size(); ++i) {
		m_normals[i] = m_normals[i].normalize();
		n_normals.push_back(m_normals[i]);
	}

}

void Model::setPosi(Point3d p) {
	this->posi = p;
}
void Model::setScale(int s) {
	this->scale = s;
}

void Model::setOffset(Point3d ofst) {
	this->offset = ofst;
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
	glVertexPointer(3, GL_FLOAT, 0, (float *)n_points.data());
	glNormalPointer(GL_FLOAT, 0, (float *)n_normals.data());
	glDrawElements(GL_TRIANGLES, m_pointIndices.size(), GL_UNSIGNED_INT, m_pointIndices.data());
	glDisableClientState(GL_NORMAL_ARRAY);
	/*  glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHTING);
	*/
     // Wireframe
     /*glColor3f(0.6f, 0.6f, 0.6f);
     glVertexPointer(3, GL_FLOAT, 0, (float *) n_points.data());
     glDrawElements(GL_LINES, m_edgeIndices.size(), GL_UNSIGNED_INT, m_edgeIndices.data());*/

    glDisableClientState(GL_VERTEX_ARRAY);
    //glDisable(GL_DEPTH_TEST);
}