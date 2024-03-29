/**
 *
 * Created by 公众号：字节流动 on 2021/3/12.
 * https://github.com/githubhaohao/LearnFFmpeg
 * 最新文章首发于公众号：字节流动，有疑问或者技术交流可以添加微信 Byte-Flow ,领取视频教程, 拉你进技术交流群
 *
 * */

#ifndef LEARNFFMPEG_MASTER_GLCAMERARENDER_H
#define LEARNFFMPEG_MASTER_GLCAMERARENDER_H
#include <thread>
#include <ImageDef.h>
#include "VideoRender.h"
#include <GLES3/gl3.h>
#include <detail/type_mat.hpp>
#include <detail/type_mat4x4.hpp>
#include <vec2.hpp>
#include <render/BaseGLRender.h>
#include <vector>

#include "GLRenderLooper.h"

using namespace glm;
using namespace std;

#define TEXTURE_NUM 3

#define IMG_WIDTH 1600
#define IMG_HEIGHT 1200


typedef void (*OnRenderFrameCallback)(void*, NativeImage*);

class MyGLRenderApps: public VideoRender, public BaseGLRender{
public:
    //单例模式
    static MyGLRenderApps *GetInstance();
    static void ReleaseInstance();
    //添加好滤镜之后，视频帧的回调，然后将带有滤镜的视频帧放入编码队列
    void SetRenderCallback(void *ctx, OnRenderFrameCallback callback) {
        m_CallbackContext = ctx;
        m_RenderFrameCallback = callback;
    }
    //渲染一帧视频
    virtual void RenderVideoFrame(NativeImage *pImage);
    virtual void UnInit();
    //GLSurfaceView 的三个回调
    virtual void OnSurfaceCreated();
    virtual void OnSurfaceChanged(int w, int h);
    virtual void OnDrawFrame();
    //使用egl实现离屏幕渲染
    void eglInit();
    void OnEglDrawFrame();
    static void OnAsyncRenderDone(void *callback, int fboTexId);
private:
    MyGLRenderApps();
    virtual ~MyGLRenderApps();
    bool CreateFrameBufferObj();
    void GetRenderFrameFromFBO();

private:
    static std::mutex m_Mutex;
    static MyGLRenderApps* s_Instance;
    GLuint m_ProgramObj = GL_NONE;
    GLuint m_FboProgramObj = GL_NONE;
    GLuint m_TextureIds[TEXTURE_NUM];
    GLuint m_VaoId[2];
    GLuint m_VboIds[4];
    GLuint m_SrcFboTextureId = GL_NONE;
    GLuint m_SrcFboId = GL_NONE;
    NativeImage m_RenderImage;
    vec2 m_ScreenSize;
    OnRenderFrameCallback m_RenderFrameCallback = nullptr;
    void *m_CallbackContext = nullptr;
    //支持滑动选择滤镜功能
    char * m_pFragShaderBuffer = nullptr;
    NativeImage m_ExtImage;

    //egl
    GLEnv m_GLEnv;
    EglCore *m_EglCore = nullptr;
    OffscreenSurface *m_OffscreenSurface = nullptr;
    condition_variable m_Cond;
};


#endif //LEARNFFMPEG_MASTER_GLCAMERARENDER_H
