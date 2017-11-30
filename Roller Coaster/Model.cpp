#include "model.h"

#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QVarLengthArray>

#include <QtOpenGL/QtOpenGL>

Model::Model(const QString &filePath, int _textureID, float _scale, Point3d _offset, Pnt3f _rgb, Point3d _posi)
    : m_fileName(QFileInfo(filePath).fileName()) {
    // Default.
    float s = 1.0f; 
    Point3d p(0, 0, 0);

    this->textureID = _textureID;
    this->scale = _scale;
    this->offset = _offset;
    this->rgb = _rgb;
    this->posi = _posi;
    this->bx = Pnt3f(1, 0, 0);
    this->by = Pnt3f(0, 1, 0);
    this->bz = Pnt3f(0, 0, 1);


    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
        return;
    Point3d boundsMin(1e9, 1e9, 1e9);
    Point3d boundsMax(-1e9, -1e9, -1e9);

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
            UV uv;
            ts >> uv.x >> uv.y;
            uvs << uv;
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

    const Point3d bounds = boundsMax - boundsMin;
    const qreal scale = s / qMax(bounds.x, qMax(bounds.y, bounds.z));

    n_points.clear();
    for (int i = 0; i < m_points.size(); ++i) {
        m_points[i] = (m_points[i] + p - (boundsMin + bounds * 0.5)) * scale;
        n_points.push_back(m_points[i]);
    }

    m_normals.resize(m_points.size());

    for (int i = 0; i < m_pointIndices.size(); i += 3) {
        const Point3d a = m_points.at(m_pointIndices.at(i));
        const Point3d b = m_points.at(m_pointIndices.at(i + 1));
        const Point3d c = m_points.at(m_pointIndices.at(i + 2));

        const Point3d normal = cross(b - a, c - a).normalize();

        for (int j = 0; j < 3; ++j)
            m_normals[m_pointIndices.at(i + j)] += normal;
    }

    n_normals.clear();
    for (int i = 0; i < m_normals.size(); ++i) {
        m_normals[i] = m_normals[i].normalize();
        n_normals.push_back(m_normals[i]);
    }
}
void Model::Draw(bool doingShadows) {
    glPushMatrix();
    glTranslatef(this->posi.x, this->posi.y, this->posi.z);
    // Change basis.
    const float fM[4 * 4] = { this->bx.x, this->bx.y, this->bx.z, 0, this->by.x, this->by.y, this->by.z, 0, this->bz.x, this->bz.y, this->bz.z, 0, 0, 0, 0, 1 };
    glMultMatrixf(fM);
    glTranslatef(this->offset.x, this->offset.y, this->offset.z);
    glScalef(this->scale, this->scale, this->scale);
    if (!doingShadows) {
        glColor3ub(this->rgb.x, this->rgb.y, this->rgb.z);
        this->render(this->textureID);
    } else {
        this->render(false);
    }
    glPopMatrix();
}

void Model::setPosi(Point3d p) {
    this->posi = p;
}
void Model::setBasis(Pnt3f x, Pnt3f y, Pnt3f z) {
    this->bx = x;
    this->by = y;
    this->bz = z;
}
void Model::setScale(int s) {
    this->scale = s;
}

void Model::setOffset(Point3d ofst) {
    this->offset = ofst;
}


void Model::render(bool useTex, bool wireframe, bool normals) const {
    //glEnable(GL_DEPTH_TEST);
    glEnableClientState(GL_VERTEX_ARRAY);
    if (wireframe) {
        glVertexPointer(3, GL_FLOAT, 0, (float *) n_points.data());
        glDrawElements(GL_LINES, m_edgeIndices.size(), GL_UNSIGNED_INT, m_edgeIndices.data());
    } else {
        /*glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glEnable(GL_COLOR_MATERIAL);
        glShadeModel(GL_SMOOTH);*/

        if (useTex) {
            glEnable(GL_NORMALIZE);
            glEnable(GL_TEXTURE_2D);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glEnableClientState(GL_NORMAL_ARRAY);
            glTexCoordPointer(2, GL_FLOAT, 0, (float *) uvs.data());
            glBindTexture(GL_TEXTURE_2D, this->textureID);
            glVertexPointer(3, GL_FLOAT, 0, (float *) n_points.data());
            glNormalPointer(GL_FLOAT, 0, (float *) n_normals.data());
            glDrawElements(GL_TRIANGLES, m_pointIndices.size(), GL_UNSIGNED_INT, m_pointIndices.data());
            glDisableClientState(GL_NORMAL_ARRAY);
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
            glDisable(GL_TEXTURE_2D);
            glDisable(GL_NORMALIZE);
        } else {
            glEnable(GL_NORMALIZE);
            glEnableClientState(GL_NORMAL_ARRAY);
            glVertexPointer(3, GL_FLOAT, 0, (float *) n_points.data());
            glNormalPointer(GL_FLOAT, 0, (float *) n_normals.data());
            glDrawElements(GL_TRIANGLES, m_pointIndices.size(), GL_UNSIGNED_INT, m_pointIndices.data());
            glDisableClientState(GL_NORMAL_ARRAY);
            glDisable(GL_NORMALIZE);
        }
        /*glDisable(GL_COLOR_MATERIAL);
        glDisable(GL_LIGHT0);
        glDisable(GL_LIGHTING);*/
    }

    if (normals) {
        QVector<Point3d> normals;
        for (int i = 0; i < m_normals.size(); ++i)
            normals << n_points.at(i) << (n_points.at(i) + n_normals.at(i) * 0.02f);
        glVertexPointer(3, GL_FLOAT, 0, (float *) normals.data());
        glDrawArrays(GL_LINES, 0, normals.size());
    }
    glDisableClientState(GL_VERTEX_ARRAY);
    //glDisable(GL_DEPTH_TEST);
}

Tunnel::Tunnel(float t, const QString & filePath, int _textureID, float _scale, Point3d _offset, Pnt3f _rgb) 
    : Model(filePath, _textureID, _scale, _offset, _rgb){
    this->t = t;
}