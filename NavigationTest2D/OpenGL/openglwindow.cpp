/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the documentation of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "openglwindow.h"

#include <QtCore/QCoreApplication>
#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLPaintDevice>
#include <QtGui/QPainter>

//Vertices
//Axis
GLdouble axisesVertices[] = {
    //X
    -100, 0, 0,
    100, 0, 0,
    //Y
    0, -100, 0,
    0, 100, 0,
    //Z
    0, 0, -100,
    0, 0, 100
};
GLfloat axisesColors[] = {
    //X
    1, 0, 0,
    1, 0, 0,
    //Y
    1, 1, 0,
    1, 1, 0,
    //Z
    0, 0, 1,
    0, 0, 1
};
//Area of Coordinates
GLfloat areaColors[] = {
    1, 1, 1,
    1, 1, 1,
    1, 1, 1,
    1, 1, 1
};

OpenGLWindow::OpenGLWindow(QWindow *parent)
    : QWindow(parent)
    , m_animating(false)
    , m_context(0)
    , m_device(0)
{
    setSurfaceType(QWindow::OpenGLSurface);
}

OpenGLWindow::~OpenGLWindow()
{
    if (nullptr != m_shortestPathBuffer)
        delete[] m_shortestPathBuffer;
    m_shortestPathBuffer = nullptr;

    if (nullptr != m_waysBuffer)
        delete[] m_waysBuffer;
    m_waysBuffer = nullptr;

    if (nullptr != m_shortestPathColors)
        delete[] m_shortestPathColors;
    m_shortestPathColors = nullptr;

    if (nullptr != m_pathColors)
        delete[] m_pathColors;
    m_pathColors = nullptr;

    delete m_device;
}
void OpenGLWindow::render(QPainter *painter)
{
    Q_UNUSED(painter);
}

void OpenGLWindow::initialize()
{
    m_program = new QOpenGLShaderProgram(this);
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    m_program->link();
    m_positionAttribute = m_program->attributeLocation("posAttr");
    m_colorAttribute    = m_program->attributeLocation("colAttr");
    m_matrixUniform     = m_program->uniformLocation("matrix");

//    glClearColor(0.32, 0.42, 0.61, 1);
//    glClearDepth(1.0f);         // Set background depth to farthest
//    glShadeModel(GL_SMOOTH);    // Enable smooth shading
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  // Nice perspective corrections
//    glEnable(GL_DEPTH_TEST); // Enable depth buffer
//    glDepthFunc(GL_LEQUAL);    // Set the type of depth-test
//    glEnable(GL_CULL_FACE);//Disable back face culling

    //Navigate
//    m_dijkstra.AddWay(CWay(e, b, true));
//	m_shortestPath = m_dijkstra.FindShortestPath(d, c, m_pathDistance, 6);
	m_dijkstra.LoadShapeFile("E:/Workstation/C++/NavigationTest2D/Data/test/test.shp");

	double pathDistance = 0;
	CCoordinate sourceCoordinate		(51.363523, 35.766957, 0);
	CCoordinate destinationCoordinate	(51.364110, 35.767557, 0);
	m_shortestPath = m_dijkstra.FindShortestPath(
		sourceCoordinate,
		destinationCoordinate,
		pathDistance);

    m_waysCount = m_dijkstra.m_ways.size();

    {
        //Shortest Path
        m_shortestPathCount = (m_shortestPath.size() - 1) * 2;
        m_shortestPathBuffer = new GLdouble[m_shortestPathCount * 3];
        int32_t coordinateCounter = 0;
        for (int32_t index = 0; index <= (m_shortestPathCount * 3) - 3 * 2; index += 6){
            //Start Coordinate
            m_shortestPathBuffer[index + 0] = m_shortestPath[coordinateCounter].m_x;
            m_shortestPathBuffer[index + 1] = m_shortestPath[coordinateCounter].m_y;
            m_shortestPathBuffer[index + 2] = m_shortestPath[coordinateCounter].m_z;
            //End Coordinate
            m_shortestPathBuffer[index + 3] = m_shortestPath[coordinateCounter + 1].m_x;
            m_shortestPathBuffer[index + 4] = m_shortestPath[coordinateCounter + 1].m_y;
            m_shortestPathBuffer[index + 5] = m_shortestPath[coordinateCounter + 1].m_z;
            coordinateCounter++;
        }

        m_waysCount = m_dijkstra.m_ways.size() * 2;
        m_waysBuffer = new GLdouble[m_waysCount * 3];
        int32_t waysCounter = 0;
        for (int32_t index = 0; index < (m_waysCount * 3) - 3 * 2; index += 6){
            //Start Coordinate
            m_waysBuffer[index + 0] = m_dijkstra.m_ways[waysCounter].m_startCoordinate.m_x;
            m_waysBuffer[index + 1] = m_dijkstra.m_ways[waysCounter].m_startCoordinate.m_y;
            m_waysBuffer[index + 2] = m_dijkstra.m_ways[waysCounter].m_startCoordinate.m_z;
            //End Coordinate
            m_waysBuffer[index + 3] = m_dijkstra.m_ways[waysCounter + 1].m_startCoordinate.m_x;
            m_waysBuffer[index + 4] = m_dijkstra.m_ways[waysCounter + 1].m_startCoordinate.m_y;
            m_waysBuffer[index + 5] = m_dijkstra.m_ways[waysCounter + 1].m_startCoordinate.m_z;
            waysCounter++;
        }
    }

    //Color
    {
        //    m_coordinatesBuffer = new GLdouble[
        m_shortestPathColors = new GLfloat[m_waysCount * 3];
        for (int32_t index = 0; index < (m_waysCount * 3) - 3; index += 3){
            //Yellow Color
            m_shortestPathColors[index + 0] = 1;
            m_shortestPathColors[index + 1] = 1;
            m_shortestPathColors[index + 2] = 0;
        }

        m_pathColors = new GLfloat[m_waysCount * 3];
        for (int32_t index = 0; index < (m_waysCount * 3) - 3; index += 3){
            //blue Color
            m_pathColors[index + 0] = 1;
            m_pathColors[index + 1] = 1;
            m_pathColors[index + 2] = 1;
        }
    }

}

void OpenGLWindow::render()
{
    const qreal retinaScale = devicePixelRatio();
        glViewport(0, 0, width() * retinaScale, height() * retinaScale);

        glClear(GL_COLOR_BUFFER_BIT);

        m_program->bind();

        QMatrix4x4 matrix;
        matrix.perspective(60.0f, 4.0f/3.0f, 0.1, 100);
        matrix.translate(-3, -3, -18);
        matrix.rotate(15, 1, 0, 0);
        m_program->setUniformValue(m_matrixUniform, matrix);

        //Draw Axises
        {
            glVertexAttribPointer(m_positionAttribute, 3, GL_DOUBLE, GL_FALSE, 0, axisesVertices);
            glVertexAttribPointer(m_colorAttribute, 3, GL_FLOAT, GL_FALSE, 0, axisesColors);
            glEnableVertexAttribArray(m_positionAttribute);
            glEnableVertexAttribArray(m_colorAttribute);
            glLineWidth(1.2);
            glDrawArrays(GL_LINES, 0, 6);
            glDisableVertexAttribArray(m_colorAttribute);
            glDisableVertexAttribArray(m_positionAttribute);
        }

//        //Draw area of coordinates
//        {
//            GLdouble area_vertices[] = {
//                //Top-Left
//                m_dijkstra.m_area.m_left, m_dijkstra.m_area.m_bottom, m_dijkstra.m_area.m_top,
//                //Bottom-Left
//                m_dijkstra.m_area.m_left, m_dijkstra.m_area.m_bottom, m_dijkstra.m_area.m_bottom,
//                //Bottom-Right
//                m_dijkstra.m_area.m_right, m_dijkstra.m_area.m_bottom, m_dijkstra.m_area.m_bottom,
//                //Top-Right
//                m_dijkstra.m_area.m_right, m_dijkstra.m_area.m_bottom, m_dijkstra.m_area.m_top
//            };
//            glVertexAttribPointer(m_positionAttribute, 3, GL_DOUBLE, GL_FALSE, 0, area_vertices);
//            glVertexAttribPointer(m_colorAttribute, 3, GL_FLOAT, GL_FALSE, 0, areaColors);
//            glEnableVertexAttribArray(m_positionAttribute);
//            glEnableVertexAttribArray(m_colorAttribute);
//            glLineWidth(1);
//            glDrawArrays(GL_QUADS, 0, 4);
//            glDisableVertexAttribArray(m_colorAttribute);
//            glDisableVertexAttribArray(m_positionAttribute);
//        }

        //Draw Ways
        {
            glVertexAttribPointer(m_positionAttribute, 3, GL_DOUBLE, GL_FALSE, 0, m_waysBuffer);
            glVertexAttribPointer(m_colorAttribute, 3, GL_FLOAT, GL_FALSE, 0, m_pathColors);
            glEnableVertexAttribArray(m_positionAttribute);
            glEnableVertexAttribArray(m_colorAttribute);
            glLineWidth(1);
            glDrawArrays(GL_LINES, 0, m_waysCount);
            glDisableVertexAttribArray(m_colorAttribute);
            glDisableVertexAttribArray(m_positionAttribute);
        }

        //Draw Shortest Path
        {
            glVertexAttribPointer(m_positionAttribute, 3, GL_DOUBLE, GL_FALSE, 0, m_shortestPathBuffer);
            glVertexAttribPointer(m_colorAttribute, 3, GL_FLOAT, GL_FALSE, 0, m_shortestPathColors);
            glEnableVertexAttribArray(m_positionAttribute);
            glEnableVertexAttribArray(m_colorAttribute);
            glLineWidth(4);
            glDrawArrays(GL_LINES, 0, m_shortestPathCount);
            glDisableVertexAttribArray(m_colorAttribute);
            glDisableVertexAttribArray(m_positionAttribute);
        }

        m_program->release();
        ++m_frame;
}

void OpenGLWindow::renderLater()
{
    requestUpdate();
}

bool OpenGLWindow::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::UpdateRequest:
        renderNow();
        return true;
    default:
        return QWindow::event(event);
    }
}

void OpenGLWindow::exposeEvent(QExposeEvent *event)
{
    Q_UNUSED(event);

    if (isExposed())
        renderNow();
}

void OpenGLWindow::renderNow()
{
    if (!isExposed())
        return;

    bool needsInitialize = false;

    if (!m_context) {
        m_context = new QOpenGLContext(this);
        m_context->setFormat(requestedFormat());
        m_context->create();

        needsInitialize = true;
    }

    m_context->makeCurrent(this);

    if (needsInitialize) {
        initializeOpenGLFunctions();
        initialize();
    }

    render();

    m_context->swapBuffers(this);

    if (m_animating)
        renderLater();
}

void OpenGLWindow::setAnimating(bool animating)
{
    m_animating = animating;

    if (animating)
        renderLater();
}