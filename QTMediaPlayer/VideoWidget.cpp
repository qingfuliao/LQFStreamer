#include "VideoWidget.h"

static const char s_indices[] = { 0, 3, 2, 0, 2, 1 };
static const char s_vertext_shader[] = 
{ 
"attribute vec4 aPosition;\n"		// 属性，顶点矩阵支持的Per_vertex数据
"attribute vec2 aTextureCoord;\n"	
"varying vec2 vTextureCoord;\n"	//顶点着色器的输出叫做varying变量
"void main() {\n"
"  gl_Position = aPosition;\n"
"  vTextureCoord = aTextureCoord;\n"
"}\n" 
};
static const char s_fragment_shader[] =
{
	"uniform sampler2D Ytex,Utex,Vtex;\n"
	"varying vec2 vTextureCoord;\n"
	"void main(void) {\n"
	"vec3 yuv;\n"
	"vec3 rgb;\n"
	"yuv.x = texture2D(Ytex, vTextureCoord).r;\n"
	"yuv.y = texture2D(Utex, vTextureCoord).r - 0.5;\n"
	"yuv.z = texture2D(Vtex, vTextureCoord).r - 0.5;\n"
	"rgb = mat3(1,1,1,\n"
	"0,-0.39465,2.03211,\n"
	"1.13983, -0.58060,  0) * yuv;\n"
	"gl_FragColor = vec4(rgb, 1);\n"
	"}\n" 
};
static GLfloat s_verticesA[20] = 
{ 1, 1, 0, 1, 0, -1, 1, 0, 0, 0, -1, -1, 0, 0,1, 1, -1, 0, 1, 1, };

void VideoWidget::initializeTexture(int id, int width, int height) 
{
	glBindTexture(GL_TEXTURE_2D, id);	// 将一个指定的纹理ID绑定到一个纹理目标上
	//设置纹理贴图的参数属性
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width, height, 0, GL_LUMINANCE,
		GL_UNSIGNED_BYTE, NULL);	// 指定一个二维纹理图像
}

VideoWidget::VideoWidget(QWidget *parent) 
	:QOpenGLWidget(parent),
	texture_(0),
	transparent_(false),
	background_(Qt::white),
	width_(0),
	height_(0),
	buf_index_(0)
{
	memset(buffer_, 0, sizeof(buffer_));
	memset(texture_ids_, 0, sizeof(texture_ids_));
	is_shader_inited_ = false;
}

VideoWidget::~VideoWidget()
{
	// And now release all OpenGL resources.
	makeCurrent();

	doneCurrent();
}

void VideoWidget::initializeGL()
{
	//Initializes OpenGL function resolution for the current context.
	initializeOpenGLFunctions();
	// 纹理过滤函数
	// GL_TEXTURE_2D: 操作2D纹理.
	// GL_LINEAR: 线性过滤, 使用距离当前渲染像素中心最近的4个纹素加权平均值.
	// GL_TEXTURE_MIN_FILTER: 缩小过滤
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// GL_TEXTURE_MAG_FILTER: 放大过滤
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glEnable(GL_TEXTURE_2D);	// 启用2D渲染
	// 色彩值的范围从0.0到1.0。0.0代表最黑的情况，1.0就是最亮的情况
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);	// 将背景清为黑色

	// GL_LEQUAL,如果输入的深度值小于或等于参考值，则通过
	glDepthFunc(GL_LEQUAL);
	// 真正精细的透视修正。这一行告诉OpenGL我们希望进行最好的透视修正。
	// 这会十分轻微的影响性能。但使得透视图看起来好一点。
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	vshaderA_ = new QOpenGLShader(QOpenGLShader::Vertex);	// 顶点着色器
	vshaderA_->compileSourceCode(s_vertext_shader);
	fshaderA_ = new QOpenGLShader(QOpenGLShader::Fragment);	// 片元着色器
	fshaderA_->compileSourceCode(s_fragment_shader);
	programA_ = new QOpenGLShaderProgram;
	programA_->addShader(vshaderA_);
	programA_->addShader(fshaderA_);
	programA_->link();

	int positionHandle = programA_->attributeLocation("aPosition");
	int textureHandle = programA_->attributeLocation("aTextureCoord");

	glVertexAttribPointer(positionHandle, 3, GL_FLOAT, false,
		5 * sizeof(GLfloat), s_verticesA);

	glEnableVertexAttribArray(positionHandle);

	glVertexAttribPointer(textureHandle, 2, GL_FLOAT, false,
		5 * sizeof(GLfloat), &s_verticesA[3]);

	glEnableVertexAttribArray(textureHandle);

	int i = programA_->uniformLocation("Ytex");
	glUniform1i(i, 0);
	i = programA_->uniformLocation("Utex");
	glUniform1i(i, 1);
	i = programA_->uniformLocation("Vtex");
	glUniform1i(i, 2);
}


void VideoWidget::InitShader(int width, int height)
{
	if (width_ != width || height_ != height)
	{
		width_ = width;
		height_ = height;
		int size = width_*height_ * 3 / 2;

		for (int i = 0; i<BUF_SIZE; i++)
		{
			buffer_[i] = (unsigned char*)realloc(buffer_[i], size);
		}
		is_shader_inited_ = false;
	}
}

void VideoWidget::UpdateData(unsigned char**data)
{
	unsigned char *tmp = buffer_[buf_index_];

	memcpy(tmp, data[0], width_*height_);
	tmp += width_*height_;

	memcpy(tmp, data[1], width_*height_ / 4);

	tmp += width_*height_ / 4;
	memcpy(tmp, data[2], width_*height_ / 4);

	this->update();
}

void VideoWidget::paintGL()
{
	if (!is_shader_inited_)
	{
		is_shader_inited_ = true;
		glDeleteTextures(3, texture_ids_);

		glGenTextures(3, texture_ids_);

		glActiveTexture(GL_TEXTURE0);
		initializeTexture(texture_ids_[0], width_, height_);
		glActiveTexture(GL_TEXTURE1);
		initializeTexture(texture_ids_[1], width_ / 2, height_ / 2);
		glActiveTexture(GL_TEXTURE2);
		initializeTexture(texture_ids_[2], width_ / 2, height_ / 2);
	}

	glClearColor(background_.redF(), background_.greenF(), background_.blueF(), transparent_ ? 0.0f : 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (width_ == 0 || height_ == 0)
		return;
	programA_->bind();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_ids_[0]);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width_, height_, GL_LUMINANCE,
		GL_UNSIGNED_BYTE, buffer_[buf_index_]);

	int i = programA_->uniformLocation("Ytex");
	glUniform1i(i, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture_ids_[1]);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width_ / 2, height_ / 2, GL_LUMINANCE,
		GL_UNSIGNED_BYTE, (char *)buffer_[buf_index_] + width_ * height_);

	i = programA_->uniformLocation("Utex");
	glUniform1i(i, 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, texture_ids_[2]);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width_ / 2, height_ / 2, GL_LUMINANCE,
		GL_UNSIGNED_BYTE, (char *)buffer_[buf_index_] + width_ * height_ * 5 / 4);

	i = programA_->uniformLocation("Vtex");
	glUniform1i(i, 2);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, s_indices);

	programA_->release();

	buf_index_++;
	if (buf_index_ >= BUF_SIZE)
		buf_index_ = 0;
}

void VideoWidget::setTransparent(bool transparent)
{
	setAttribute(Qt::WA_AlwaysStackOnTop, transparent);
	transparent_ = transparent;
	window()->update();
}

void VideoWidget::resizeGL(int w, int h)
{

}