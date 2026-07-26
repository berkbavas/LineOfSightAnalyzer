#include "OpenGLWidget.h"

#include "Util/Logger.h"
#include "Util/Util.h"

#include <QDateTime>

LineOfSightAnalyzer::OpenGLWidget::OpenGLWidget(QWidget *pParent)
    : QOpenGLWidget(pParent)
{
    connect(this, &OpenGLWidget::frameSwapped, this, [this]() { update(); });
    setMouseTracking(true);
}

void LineOfSightAnalyzer::OpenGLWidget::initializeGL()
{
    qDebug() << "OpenGLWidget::initializeGL: QSurfaceFormat:" << QSurfaceFormat::defaultFormat();

    initializeOpenGLFunctions(); // Initialize OpenGL functions for the current context

    mCurrentTime = QDateTime::currentMSecsSinceEpoch();
    mPreviousTime = mCurrentTime;

    emit Initialized();

    CheckGLError();

    qDebug() << "OpenGLWidget::initializeGL: OpenGL version:" << reinterpret_cast<const char *>(glGetString(GL_VERSION));
}

void LineOfSightAnalyzer::OpenGLWidget::resizeGL(int Width, int Height)
{
    emit Resized(Width, Height);

    CheckGLError();
}

void LineOfSightAnalyzer::OpenGLWidget::paintGL()
{
    mCurrentTime = QDateTime::currentMSecsSinceEpoch();
    const float Ifps = (mCurrentTime - mPreviousTime) * 0.001f;
    mPreviousTime = mCurrentTime;

    emit Render(Ifps);

    CheckGLError();
}

void LineOfSightAnalyzer::OpenGLWidget::keyPressEvent(QKeyEvent *pEvent)
{
    emit KeyPressed(pEvent);
}

void LineOfSightAnalyzer::OpenGLWidget::keyReleaseEvent(QKeyEvent *pEvent)
{
    emit KeyReleased(pEvent);
}

void LineOfSightAnalyzer::OpenGLWidget::mousePressEvent(QMouseEvent *pEvent)
{
    emit MousePressed(pEvent);
}

void LineOfSightAnalyzer::OpenGLWidget::mouseReleaseEvent(QMouseEvent *pEvent)
{
    emit MouseReleased(pEvent);
}

void LineOfSightAnalyzer::OpenGLWidget::mouseMoveEvent(QMouseEvent *pEvent)
{
    emit MouseMoved(pEvent);
}

void LineOfSightAnalyzer::OpenGLWidget::wheelEvent(QWheelEvent *pEvent)
{
    emit WheelMoved(pEvent);
}

void LineOfSightAnalyzer::OpenGLWidget::closeEvent(QCloseEvent *pEvent)
{
    QOpenGLWidget::closeEvent(pEvent);

    emit WindowClosed();
}

void LineOfSightAnalyzer::OpenGLWidget::leaveEvent(QEvent *pEvent)
{
    emit LeaveEvent(pEvent);
}

void LineOfSightAnalyzer::OpenGLWidget::focusOutEvent(QFocusEvent* pEvent)
{
    emit FocusOutEvent(pEvent);
}

void LineOfSightAnalyzer::OpenGLWidget::CheckGLError()
{
    GLenum Error;
    while ((Error = glGetError()) != GL_NO_ERROR) // Loop until all OpenGL errors are retrieved
    {
        LOS_EXIT_FAILURE("OpenGLWidget::CheckGLError: OpenGL error detected: '{}'", LineOfSightAnalyzer::Util::GetGlErrorString(Error));
    }
}
