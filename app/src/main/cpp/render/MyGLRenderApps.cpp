/**
 *
 * Created by 公众号：字节流动 on 2021/3/12.
 * https://github.com/githubhaohao/LearnFFmpeg
 * 最新文章首发于公众号：字节流动，有疑问或者技术交流可以添加微信 Byte-Flow ,领取视频教程, 拉你进技术交流群
 *
 * */

#include "MyGLRenderApps.h"
#include <GLUtils.h>
#include <gtc/matrix_transform.hpp>

MyGLRenderApps* MyGLRenderApps::s_Instance = nullptr;
std::mutex MyGLRenderApps::m_Mutex;

static char vShaderStr[] =
        "#version 300 es\n"
        "layout(location = 0) in vec4 a_position;\n"
        "layout(location = 1) in vec2 a_texCoord;\n"
        "out vec2 v_texCoord;\n"
        "void main()\n"
        "{\n"
        "    gl_Position = a_position;\n"
        "    v_texCoord =  a_texCoord;\n"
        "}";

static char fShaderStr[] =
        "#version 300 es\n"
        "precision highp float;\n"
        "in vec2 v_texCoord;\n"
        "layout(location = 0) out vec4 outColor;\n"
        "uniform sampler2D s_texture0;\n"
        "uniform sampler2D s_texture1;\n"
        "uniform sampler2D s_texture2;\n"
        "uniform int u_nImgType;// 1:RGBA, 2:NV21, 3:NV12, 4:I420\n"
        "\n"
        "void main()\n"
        "{\n"
        "\n"
        "    if(u_nImgType == 1) //RGBA\n"
        "    {\n"
        "        outColor = texture(s_texture0, v_texCoord);\n"
        "    }\n"
        "    else if(u_nImgType == 2) //NV21\n"
        "    {\n"
        "        vec3 yuv;\n"
        "        yuv.x = texture(s_texture0, v_texCoord).r;\n"
        "        yuv.y = texture(s_texture1, v_texCoord).a - 0.5;\n"
        "        yuv.z = texture(s_texture1, v_texCoord).r - 0.5;\n"
        "        highp vec3 rgb = mat3(1.0,       1.0,     1.0,\n"
        "        0.0, \t-0.344, \t1.770,\n"
        "        1.403,  -0.714,     0.0) * yuv;\n"
        "        outColor = vec4(rgb, 1.0);\n"
        "\n"
        "    }\n"
        "    else if(u_nImgType == 3) //NV12\n"
        "    {\n"
        "        vec3 yuv;\n"
        "        yuv.x = texture(s_texture0, v_texCoord).r;\n"
        "        yuv.y = texture(s_texture1, v_texCoord).r - 0.5;\n"
        "        yuv.z = texture(s_texture1, v_texCoord).a - 0.5;\n"
        "        highp vec3 rgb = mat3(1.0,       1.0,     1.0,\n"
        "        0.0, \t-0.344, \t1.770,\n"
        "        1.403,  -0.714,     0.0) * yuv;\n"
        "        outColor = vec4(rgb, 1.0);\n"
        "    }\n"
        "    else if(u_nImgType == 4) //I420\n"
        "    {\n"
        "        vec3 yuv;\n"
        "        yuv.x = texture(s_texture0, v_texCoord).r;\n"
        "        yuv.y = texture(s_texture1, v_texCoord).r - 0.5;\n"
        "        yuv.z = texture(s_texture2, v_texCoord).r - 0.5;\n"
        "        highp vec3 rgb = mat3(1.0,       1.0,     1.0,\n"
        "                              0.0, \t-0.344, \t1.770,\n"
        "                              1.403,  -0.714,     0.0) * yuv;\n"
        "        outColor = vec4(rgb, 1.0);\n"
        "    }\n"
        "    else\n"
        "    {\n"
        "        outColor = vec4(1.0);\n"
        "    }\n"
        "}";

static GLfloat verticesCoords[] = {
        -1.0f,  1.0f, 0.0f,  // Position 0
        -1.0f, -1.0f, 0.0f,  // Position 1
        1.0f,  -1.0f, 0.0f,  // Position 2
        1.0f,   1.0f, 0.0f,  // Position 3
};
//FBO文件 默认为左下角坐标系
static GLfloat fboTextureCoords[] = {
        0.0f,  1.0f,        // TexCoord 0
        0.0f,  0.0f,        // TexCoord 1
        1.0f,  0.0f,        // TexCoord 2
        1.0f,  1.0f         // TexCoord 3
};
static GLfloat textureCoords[] = {
        0.0f,  0.0f,        // TexCoord 0
        0.0f,  1.0f,        // TexCoord 1
        1.0f,  1.0f,        // TexCoord 2
        1.0f,  0.0f         // TexCoord 3
};

static GLushort indices[] = { 0, 1, 2, 0, 2, 3 };

MyGLRenderApps *MyGLRenderApps::GetInstance() {
    if(s_Instance == nullptr)
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        if(s_Instance == nullptr)
        {
            s_Instance = new MyGLRenderApps();
        }
    }
    return s_Instance;
}

void MyGLRenderApps::ReleaseInstance() {
    if(s_Instance != nullptr)
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        if(s_Instance != nullptr)
        {
            delete s_Instance;
            s_Instance = nullptr;
        }
    }
}

MyGLRenderApps::MyGLRenderApps(): VideoRender(VIDEO_RENDER_OPENGL) {
}

MyGLRenderApps::~MyGLRenderApps() {
    NativeImageUtil::FreeNativeImage(&m_RenderImage);
}



void MyGLRenderApps::RenderVideoFrame(NativeImage *pImage) {
    LOGCATE("MyGLRenderApps::RenderVideoFrame pImage=%p", pImage);
    if(pImage == nullptr || pImage->ppPlane[0] == nullptr)
        return;
    std::unique_lock<std::mutex> lock(m_Mutex);
    if (pImage->width != m_RenderImage.width || pImage->height != m_RenderImage.height) {
        if (m_RenderImage.ppPlane[0] != nullptr) {
            NativeImageUtil::FreeNativeImage(&m_RenderImage);
        }
        memset(&m_RenderImage, 0, sizeof(NativeImage));
        m_RenderImage.format = pImage->format;
        m_RenderImage.width = pImage->width;
        m_RenderImage.height = pImage->height;
        NativeImageUtil::AllocNativeImage(&m_RenderImage);
    }

    NativeImageUtil::CopyNativeImage(pImage, &m_RenderImage);
    //NativeImageUtil::DumpNativeImage(&m_RenderImage, "/sdcard", "camera");
}

void MyGLRenderApps::UnInit() {
    NativeImageUtil::FreeNativeImage(&m_ExtImage);

    if(m_pFragShaderBuffer != nullptr) {
        free(m_pFragShaderBuffer);
        m_pFragShaderBuffer = nullptr;
    }

}
//glsurfaceView 由app层创建
void MyGLRenderApps::OnSurfaceCreated() {
    LOGCATE("MyGLRenderApps::OnSurfaceCreated");
    m_ProgramObj = GLUtils::CreateProgram(vShaderStr, fShaderStr);
    m_FboProgramObj = GLUtils::CreateProgram(vShaderStr, fShaderStr);
    if (!m_ProgramObj || !m_FboProgramObj)
    {
        LOGCATE("MyGLRenderApps::OnSurfaceCreated create program fail");
        return;
    }
    //生成纹理
    glGenTextures(TEXTURE_NUM, m_TextureIds);
    for (int i = 0; i < TEXTURE_NUM ; ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, m_TextureIds[i]);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, GL_NONE);
    }

    // Generate VBO Ids and load the VBOs with data
    glGenBuffers(4, m_VboIds);
    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesCoords), verticesCoords, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoords), textureCoords, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(fboTextureCoords), fboTextureCoords, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VboIds[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Generate VAO Id
    glGenVertexArrays(2, m_VaoId);
    glBindVertexArray(m_VaoId[0]);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (const void *)0);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[1]);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (const void *)0);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VboIds[3]);

    glBindVertexArray(GL_NONE);

    //绑定VAO1
    glBindVertexArray(m_VaoId[1]);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (const void *)0);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[2]);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (const void *)0);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VboIds[3]);

    glBindVertexArray(GL_NONE);
}

void MyGLRenderApps::OnSurfaceChanged(int w, int h) {
    LOGCATE("MyGLRenderApps::OnSurfaceChanged [w, h]=[%d, %d]", w, h);
    m_ScreenSize.x = w;
    m_ScreenSize.y = h;
    glViewport(0, 0, w, h);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void MyGLRenderApps::OnDrawFrame() {
    glClear(GL_COLOR_BUFFER_BIT);
    if(m_ProgramObj == GL_NONE || m_RenderImage.ppPlane[0] == nullptr) return;
    if(m_SrcFboId == GL_NONE && CreateFrameBufferObj()) {
        LOGCATE("MyGLRenderApps::OnDrawFrame CreateFrameBufferObj fail");
        return;
    }
    LOGCATE("MyGLRenderApps::OnDrawFrame [w, h]=[%d, %d], format=%d", m_RenderImage.width, m_RenderImage.height, m_RenderImage.format);

    std::unique_lock<std::mutex> lock(m_Mutex);
    // 渲染到 FBO
    glBindFramebuffer(GL_FRAMEBUFFER, m_SrcFboId);
    glViewport(0, 0, m_RenderImage.height, m_RenderImage.width); //相机的宽和高反了
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(m_FboProgramObj);
    // upload image data

    glBindTexture(GL_TEXTURE_2D, m_SrcFboTextureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_RenderImage.height, m_RenderImage.width, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    //upload Y plane data
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_TextureIds[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, m_RenderImage.width,
                 m_RenderImage.height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE,
                 m_RenderImage.ppPlane[0]);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    //update U plane data
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_TextureIds[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, m_RenderImage.width >> 1,
                 m_RenderImage.height >> 1, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE,
                 m_RenderImage.ppPlane[1]);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    //update V plane data
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_TextureIds[2]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, m_RenderImage.width >> 1,
                 m_RenderImage.height >> 1, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE,
                 m_RenderImage.ppPlane[2]);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    glBindVertexArray(m_VaoId[1]);

    for (int i = 0; i < TEXTURE_NUM; ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, m_TextureIds[i]);
        char samplerName[64] = {0};
        sprintf(samplerName, "s_texture%d", i);
        GLUtils::setInt(m_FboProgramObj, samplerName, i);
    }

    GLUtils::setInt(m_FboProgramObj, "u_nImgType", m_RenderImage.format);


    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (const void *)0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindVertexArray(GL_NONE);
    //m_OffscreenSurface->swapBuffers();
    lock.unlock();

    // 渲染到屏幕
    glViewport(0, 0, m_ScreenSize.x, m_ScreenSize.y);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(m_VaoId[0]);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_SrcFboTextureId);
    GLUtils::setInt(m_ProgramObj, "s_texture0", 0);

    GLUtils::setInt(m_ProgramObj, "u_nImgType", IMAGE_FORMAT_RGBA);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (const void *)0);
}

bool MyGLRenderApps::CreateFrameBufferObj() {
    // 创建并初始化 FBO 纹理
    if(m_SrcFboTextureId == GL_NONE) {
        glGenTextures(1, &m_SrcFboTextureId);
        glBindTexture(GL_TEXTURE_2D, m_SrcFboTextureId);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, GL_NONE);
    }

    // 创建并初始化 FBO
    if(m_SrcFboId == GL_NONE) {
        glGenFramebuffers(1, &m_SrcFboId);
        glBindFramebuffer(GL_FRAMEBUFFER, m_SrcFboId);
        glBindTexture(GL_TEXTURE_2D, m_SrcFboTextureId);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_SrcFboTextureId, 0);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_RenderImage.height, m_RenderImage.width, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER)!= GL_FRAMEBUFFER_COMPLETE) {
            LOGCATE("MyGLRenderApps::CreateFrameBufferObj glCheckFramebufferStatus status != GL_FRAMEBUFFER_COMPLETE");
            if(m_SrcFboTextureId != GL_NONE) {
                glDeleteTextures(1, &m_SrcFboTextureId);
                m_SrcFboTextureId = GL_NONE;
            }

            if(m_SrcFboId != GL_NONE) {
                glDeleteFramebuffers(1, &m_SrcFboId);
                m_SrcFboId = GL_NONE;
            }
            glBindTexture(GL_TEXTURE_2D, GL_NONE);
            glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
            return false;
        }
    }

    glBindTexture(GL_TEXTURE_2D, GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
    return true;
}

void MyGLRenderApps::GetRenderFrameFromFBO() {
    LOGCATE("MyGLRenderApps::GetRenderFrameFromFBO m_RenderFrameCallback=%p", m_RenderFrameCallback);
    if(m_RenderFrameCallback != nullptr) {
        uint8_t *pBuffer = new uint8_t[m_RenderImage.width * m_RenderImage.height * 4];
        NativeImage nativeImage = m_RenderImage;
        nativeImage.format = IMAGE_FORMAT_RGBA;
        nativeImage.width = m_RenderImage.width;
        nativeImage.height = m_RenderImage.height;
        nativeImage.pLineSize[0] = nativeImage.width * 4;
        nativeImage.ppPlane[0] = pBuffer;
        glReadPixels(0, 0, nativeImage.width, nativeImage.height, GL_RGBA, GL_UNSIGNED_BYTE, pBuffer);
        m_RenderFrameCallback(m_CallbackContext, &nativeImage);
        delete []pBuffer;
    }
}


void MyGLRenderApps::OnAsyncRenderDone(void *callback, int fboTexId) {
    LOGCATE("MyGLRenderApps::OnAsyncRenderDone callback=%p, fboTexId=%d", callback, fboTexId);
    MyGLRenderApps *ctx = static_cast<MyGLRenderApps *>(callback);
    unique_lock<mutex> lock(ctx->m_Mutex);
    ctx->m_SrcFboTextureId = fboTexId;
    ctx->m_Cond.notify_all();
}


//eglInit
void MyGLRenderApps::eglInit() {
    LOGCATE("MyGLRenderApps::OnSurfaceCreated");
    m_ProgramObj = GLUtils::CreateProgram(vShaderStr, fShaderStr);
    m_FboProgramObj = GLUtils::CreateProgram(vShaderStr, fShaderStr);
    if (!m_ProgramObj || !m_FboProgramObj)
    {
        LOGCATE("MyGLRenderApps::OnSurfaceCreated create program fail");
        return;
    }
    //生成纹理
    glGenTextures(TEXTURE_NUM, m_TextureIds);
    for (int i = 0; i < TEXTURE_NUM ; ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, m_TextureIds[i]);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, GL_NONE);
    }

    // Generate VBO Ids and load the VBOs with data
    glGenBuffers(4, m_VboIds);
    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesCoords), verticesCoords, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoords), textureCoords, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(fboTextureCoords), fboTextureCoords, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VboIds[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Generate VAO Id
    glGenVertexArrays(2, m_VaoId);
    glBindVertexArray(m_VaoId[0]);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (const void *)0);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[1]);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (const void *)0);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VboIds[3]);

    glBindVertexArray(GL_NONE);

    //绑定VAO1
    glBindVertexArray(m_VaoId[1]);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (const void *)0);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[2]);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (const void *)0);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VboIds[3]);

    glBindVertexArray(GL_NONE);

//    m_GLEnv.sharedCtx     = EGL_NO_CONTEXT;
//    m_GLEnv.program       = m_FboProgramObj;
//    m_GLEnv.textureIds[0] = m_TextureIds[0];
//    m_GLEnv.textureIds[1] = m_TextureIds[1];
//    m_GLEnv.textureIds[2] = m_TextureIds[2];
//    m_GLEnv.vboIds[0]     = m_VboIds[0];
//    m_GLEnv.vboIds[1]     = m_VboIds[2];
//    m_GLEnv.vboIds[2]     = m_VboIds[3];
//    m_GLEnv.imgWidth      = IMG_WIDTH;
//    m_GLEnv.imgHeight     = IMG_HEIGHT;
//    m_GLEnv.imgFormat      = m_RenderImage.format;
//    m_GLEnv.renderDone    = OnAsyncRenderDone;
//    m_GLEnv.callbackCtx   = this;
//    LOGCATE("SharedEGLContextSample::Init sharedCtx=%p", m_GLEnv.sharedCtx);
//
//    GLRenderLooper::GetInstance()->postMessage(MSG_SurfaceCreated, &m_GLEnv);
//
//    GLRenderLooper::GetInstance()->postMessage(MSG_SurfaceChanged, m_RenderImage.width, m_RenderImage.height);

}


void MyGLRenderApps::OnEglDrawFrame() {
    glClear(GL_COLOR_BUFFER_BIT);
    if(m_ProgramObj == GL_NONE || m_RenderImage.ppPlane[0] == nullptr) return;

    LOGCATE("MyGLRenderApps::OnDrawFrame [w, h]=[%d, %d], format=%d", m_RenderImage.width, m_RenderImage.height, m_RenderImage.format);

    std::unique_lock<std::mutex> lock(m_Mutex);

    //upload Y plane data
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_TextureIds[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, m_RenderImage.width,
                 m_RenderImage.height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE,
                 m_RenderImage.ppPlane[0]);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    //update U plane data
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_TextureIds[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, m_RenderImage.width >> 1,
                 m_RenderImage.height >> 1, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE,
                 m_RenderImage.ppPlane[1]);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    //update V plane data
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_TextureIds[2]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, m_RenderImage.width >> 1,
                 m_RenderImage.height >> 1, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE,
                 m_RenderImage.ppPlane[2]);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    lock.unlock();

    {
        unique_lock<mutex> lock(m_Mutex);
        GLRenderLooper::GetInstance()->postMessage(MSG_DrawFrame);
        m_Cond.wait(lock);
    }


    // 渲染到屏幕
    glViewport(0, 0, m_ScreenSize.x, m_ScreenSize.y);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(m_VaoId[0]);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_SrcFboTextureId);
    GLUtils::setInt(m_ProgramObj, "s_texture0", 0);

    GLUtils::setInt(m_ProgramObj, "u_nImgType", IMAGE_FORMAT_RGBA);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (const void *)0);
}