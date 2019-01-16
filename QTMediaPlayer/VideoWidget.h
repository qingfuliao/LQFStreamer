#pragma once

#include <QtWidgets/qwidget.h>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShader>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>

#define BUF_SIZE 2

class VideoWidget :	public QOpenGLWidget, protected QOpenGLFunctions
{
	Q_OBJECT
public:
	VideoWidget(QWidget *parent = NULL);
	~VideoWidget();
	void UpdateData(unsigned char**);
	void InitShader(int w, int h);
public slots:
	void setTransparent(bool transparent);

protected:
	void resizeGL(int w, int h) Q_DECL_OVERRIDE;
	void paintGL() Q_DECL_OVERRIDE;
	void initializeGL() Q_DECL_OVERRIDE;

private:
	void initializeTexture(int id, int width, int height);

	QOpenGLShader *vshaderA_;
	QOpenGLShader *fshaderA_;
	QOpenGLShaderProgram *programA_;
	QOpenGLBuffer vboA_;
	QOpenGLTexture *texture_;

	bool transparent_;

	QColor background_;

	bool is_shader_inited_;
	int width_;
	int height_;
	quint8 *buffer_[BUF_SIZE];
	int buf_index_;

	GLuint  texture_ids_[3];
};

