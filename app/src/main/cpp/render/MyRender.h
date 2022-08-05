//
// Created by pirate on 2022/8/4.
//

#ifndef MEDIADEMO_MYRENDER_H
#define MEDIADEMO_MYRENDER_H

#include <condition_variable>
#include <GLES3/gl3.h>
using namespace std;

class MyRender
{
public:
    MyRender();
    ~MyRender();


    int init(int width, int height);

    int destroy();

    int draw(unsigned char* buffer, int w, int h);

    static void OnAsyncRenderDone(void* callback, int fboTexId);

    bool CreateFrameBufferObj(GLuint& fboId, GLuint& textureId, int w, int h);

private:

    GLuint m_FboTextureId;
    GLuint m_VaoId;
    GLuint m_VboIds[4]{};

    GLuint m_ProgramObj;
    GLuint m_FboMainProgram;
    GLuint m_FboRgb2YuvProgram;

    GLuint m_yTextureId;
    GLuint m_uvTextureId;
    //GLSL对应的uniform 全局变量
    GLint m_ySamplerLoc;
    GLint m_uvSamplerLoc;
    GLint m_transSamplerLoc;
    GLint m_transUoffsetLoc;
    //FBO
    GLuint m_dwFboId;
    GLuint m_dwFboId2;


    int m_nScreenW;
    int m_nScreenH;

    mutex m_Mutex;
    condition_variable m_Cond;

};



#endif //MEDIADEMO_MYRENDER_H
