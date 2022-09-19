#pragma once
#include <condition_variable>
#include "GLRenderLooper.h"
#include "GLSampleBase.h"

class MyRender : public GLSampleBase
{
public:
	MyRender();
	~MyRender();
    virtual void Init();
    virtual void Destroy();
    virtual void Draw(int screenW, int screenH);

	static void OnAsyncRenderDone(void* callback, int fboTexId);

private:
	void frameGetThd();
	void DownloadPixels() ;
private:
    unsigned char* m_buffer;

	GLuint m_FboTextureId;
	GLuint m_VaoId;
	GLuint m_VboIds[4]{};

	GLuint m_ProgramObj;
	GLuint m_FboMainProgram;
	GLuint m_FboRgb2YuvProgram;

	GLuint m_yTextureId;
	GLuint m_uvTextureId;

	GLEnv m_GLEnv;

    int m_nScreenW;
    int m_nScreenH;

    mutex m_Mutex;
    condition_variable m_Cond;

};

