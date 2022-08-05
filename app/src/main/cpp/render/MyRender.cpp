//
// Created by pirate on 2022/8/4.
//

#include <EGL/egl.h>
#include "MyRender.h"
#include "GLUtils.h"
#include "LogUtil.h"

#define VERTEX_POS_INDX  0
#define TEXTURE_POS_INDX 1

#define REND_SURF_MAX_WIDTH		1920
#define REND_SURF_MAX_HEIGHT	1080


MyRender::MyRender()
{
    m_FboTextureId	= 0;
    m_VaoId			= 0;
    m_ProgramObj		= GL_NONE;
    m_FboMainProgram	= GL_NONE;
    m_FboRgb2YuvProgram = GL_NONE;

    m_yTextureId	= 0;
    m_uvTextureId	= 0;

    m_nScreenW = 0;
    m_nScreenH = 0;
}

MyRender::~MyRender()
{

}

int MyRender::init(int width, int height)
{
    if (m_ProgramObj != GL_NONE) return 0;

    //顶点坐标
    GLfloat vVertices[] = {
            -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            -1.0f,  1.0f, 0.0f,
            1.0f,  1.0f, 0.0f,
    };

    //正常纹理坐标
    GLfloat vTexCoors[] = {
            0.0f, 1.0f,
            1.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 0.0f,
    };

    //fbo 纹理坐标与正常纹理方向不同，原点位于左下角
    GLfloat vFboTexCoors[] = {
            0.0f, 0.0f,
            1.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 1.0f,
    };

    GLushort indices[] = { 0, 1, 2, 1, 3, 2 };


    char vShaderStr[] =
            "#version 300 es                            \n"
            "layout(location = 0) in vec4 a_position;   \n"
            "layout(location = 1) in vec2 a_texCoord;   \n"
            "out vec2 v_texCoord;                       \n"
            "void main()                                \n"
            "{                                          \n"
            "   gl_Position = a_position;               \n"
            "   v_texCoord = a_texCoord;                \n"
            "}                                          \n";

    /* just copy */
    char fShaderStr[] =
            "#version 300 es\n"
            "precision mediump float;\n"
            "in vec2 v_texCoord;\n"
            "layout(location = 0) out vec4 outColor;\n"
            "uniform sampler2D s_TextureMap;\n"
            "void main()\n"
            "{\n"
            "    outColor = texture(s_TextureMap, v_texCoord);\n"
            "}";

    /* fbo main proc shader */
    char fFboShaderStr[] =
            "#version 300 es                                    \n"
            "precision mediump float;                           \n"
            "in vec2 v_texCoord;                                \n"
            "layout(location = 0) out vec4 outColor;            \n"
            "uniform sampler2D y_texture;                       \n"
            "uniform sampler2D uv_texture;                      \n"
            "void main()                                        \n"
            "{                                                  \n"
            "	vec3 yuv;										\n"
            "   yuv.x = texture(y_texture, v_texCoord).r;  		\n"
            "   yuv.y = texture(uv_texture, v_texCoord).a-0.5;	\n"
            "   yuv.z = texture(uv_texture, v_texCoord).r-0.5;	\n"
            "	highp vec3 rgb = mat3( 1,       1,       	1,	\n"
            "               0, 		-0.344, 	1.770,			\n"
            "               1.403,  -0.714,       0) * yuv;		\n"
            "	outColor = vec4(rgb, 1);						\n"
            "}                                                  \n";

    /* fbo rgb 2 nv21 shader */
    char fFboRgb2YuvShaderStr[] =
            "#version 300 es\n"
            "precision mediump float;\n"
            "in vec2 v_texCoord;\n"
            "layout(location = 0) out vec4 outColor;\n"
            "uniform sampler2D s_TextureMap;\n"
            "uniform float u_Offset;\n"
            "//Y =  0.299R + 0.587G + 0.114B\n"
            "//U = -0.147R - 0.289G + 0.436B\n"
            "//V =  0.615R - 0.515G - 0.100B\n"
            "const vec3 COEF_Y = vec3( 0.299,  0.587,  0.114);\n"
            "const vec3 COEF_U = vec3(-0.147, -0.289,  0.436);\n"
            "const vec3 COEF_V = vec3( 0.615, -0.515, -0.100);\n"
            "const float UV_DIVIDE_LINE = 2.0 / 3.0;\n"
            "void main()\n"
            "{\n"
            "    vec2 texelOffset = vec2(u_Offset, 0.0);\n"
            "    if(v_texCoord.y <= UV_DIVIDE_LINE) {\n"
            "        vec2 texCoord = vec2(v_texCoord.x, v_texCoord.y * 3.0 / 2.0);\n"
            "        vec4 color0 = texture(s_TextureMap, texCoord);\n"
            "        vec4 color1 = texture(s_TextureMap, texCoord + texelOffset);\n"
            "        vec4 color2 = texture(s_TextureMap, texCoord + texelOffset * 2.0);\n"
            "        vec4 color3 = texture(s_TextureMap, texCoord + texelOffset * 3.0);\n"
            "\n"
            "        float y0 = dot(color0.rgb, COEF_Y);\n"
            "        float y1 = dot(color1.rgb, COEF_Y);\n"
            "        float y2 = dot(color2.rgb, COEF_Y);\n"
            "        float y3 = dot(color3.rgb, COEF_Y);\n"
            "        outColor = vec4(y0, y1, y2, y3);\n"
            "    }\n"
            "    else {\n"
            "        vec2 texCoord = vec2(v_texCoord.x, (v_texCoord.y - UV_DIVIDE_LINE) * 3.0);\n"
            "        vec4 color0 = texture(s_TextureMap, texCoord);\n"
            "        vec4 color1 = texture(s_TextureMap, texCoord + texelOffset);\n"
            "        vec4 color2 = texture(s_TextureMap, texCoord + texelOffset * 2.0);\n"
            "        vec4 color3 = texture(s_TextureMap, texCoord + texelOffset * 3.0);\n"
            "\n"
            "        float v0 = dot(color0.rgb, COEF_V) + 0.5;\n"
            "        float u0 = dot(color1.rgb, COEF_U) + 0.5;\n"
            "        float v1 = dot(color2.rgb, COEF_V) + 0.5;\n"
            "        float u1 = dot(color3.rgb, COEF_U) + 0.5;\n"
            "        outColor = vec4(v0, u0, v1, u1);\n"
            "    }\n"
            "}";




    /* 编译链接用于普通渲染的着色器程序 */
    m_ProgramObj = GLUtils::CreateProgram(vShaderStr, fShaderStr);

    /* 编译链接用于离屏渲染的着色器程序 */
    m_FboMainProgram = GLUtils::CreateProgram(vShaderStr, fFboShaderStr);

    /* 编译链接用于离屏渲染的rgb2nv21着色器程序 */
    m_FboRgb2YuvProgram = GLUtils::CreateProgram(vShaderStr, fFboRgb2YuvShaderStr);

    if (m_ProgramObj == GL_NONE || m_FboMainProgram == GL_NONE || m_FboRgb2YuvProgram == GL_NONE)
    {
        LOGCATE("MyRender::Init m_ProgramObj == GL_NONE");
        return -1;
    }

    m_nScreenW = width;
    m_nScreenH = height;

    // 生成 VBO ，加载顶点数据和索引数据
    glGenBuffers(4, m_VboIds);
    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vVertices), vVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vTexCoors), vTexCoors, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vFboTexCoors), vFboTexCoors, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VboIds[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


    glGenVertexArrays(1, &m_VaoId);
    // 初始化用于普通渲染的 VAO
    glBindVertexArray(m_VaoId);
    //关联VAO 与 VBO
    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
    glEnableVertexAttribArray(VERTEX_POS_INDX);
    glVertexAttribPointer(VERTEX_POS_INDX, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (const void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[1]);
    glEnableVertexAttribArray(TEXTURE_POS_INDX);
    glVertexAttribPointer(TEXTURE_POS_INDX, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (const void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VboIds[3]);
    GLenum glerr= glGetError();
    if (glerr!=0){
        LOGCATE("MyRender::glGetError glerr =%d",glGetError);
        return  glerr;
    }
    glBindVertexArray(GL_NONE);

    /* 创建yuv纹理 */
    m_yTextureId	= GLUtils::CreateTexture(width, height, GL_LUMINANCE);
    m_uvTextureId	= GLUtils::CreateTexture(width >> 1, height >> 1, GL_LUMINANCE_ALPHA);

    /* Get the sampler location */
    m_ySamplerLoc  = glGetUniformLocation(m_FboMainProgram, "y_texture");
    m_uvSamplerLoc = glGetUniformLocation(m_FboMainProgram, "uv_texture");

    m_transSamplerLoc = glGetUniformLocation(m_FboRgb2YuvProgram, "s_TextureMap");
    m_transUoffsetLoc = glGetUniformLocation(m_FboRgb2YuvProgram, "u_Offset");

    //创建FBO
    if (!CreateFrameBufferObj(m_dwFboId, m_yTextureId, width, height))
    {
        LOGCATE("MyRender::CreateFrameBufferObj fail");
    }
    if (!CreateFrameBufferObj(m_dwFboId2, m_uvTextureId, width / 4 , height * 1.5))
    {
        LOGCATE("MyRender::OnSurfaceCreated CreateFrameBufferObj fail");
    }
}

int MyRender::destroy()
{
    if (m_ProgramObj)
    {
        glDeleteProgram(m_ProgramObj);
    }

    if (m_FboMainProgram)
    {
        glDeleteProgram(m_FboMainProgram);
    }
    if (m_FboRgb2YuvProgram)
    {
        glDeleteProgram(m_FboRgb2YuvProgram);
    }
    if (m_yTextureId)
    {
        glDeleteTextures(1, &m_yTextureId);
    }
    if (m_uvTextureId)
    {
        glDeleteTextures(1, &m_uvTextureId);
    }

    if (m_VboIds[0])
    {
        glDeleteBuffers(4, m_VboIds);
    }

    if (m_VaoId)
    {
        glDeleteVertexArrays(1, &m_VaoId);
    }
    return 0;
}

int MyRender::draw(unsigned char* buffer, int w, int h)
{
    LOGCATE("MyRender::Draw");
    /* 更新纹理GL_TEXTURE0, 对应Y分量数据 */
    glBindTexture(GL_TEXTURE_2D, m_yTextureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, w, h,
                 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, buffer);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    /* 更新纹理GL_TEXTURE1, 对应UV分量数据 */
    glBindTexture(GL_TEXTURE_2D, m_uvTextureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, w >> 1, h >> 1,
                 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, buffer + w * h);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    {
        unique_lock<mutex> lock(m_Mutex);
        //GLRenderLooper::GetInstance()->postMessage(MSG_DrawFrame);
        m_Cond.wait(lock);
    }

    glViewport(0, 0, m_nScreenW, m_nScreenH);
    glUseProgram(m_ProgramObj);
    //CHECK_GL_ERROR();
    glBindVertexArray(m_VaoId);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_FboTextureId);
    GLUtils::setInt(m_ProgramObj, "s_TextureMap", 0);
    //CHECK_GL_ERROR();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (const void*)0);
    //CHECK_GL_ERROR();
    glBindTexture(GL_TEXTURE_2D, GL_NONE);
    glBindVertexArray(GL_NONE);

    return 0;
}

void MyRender::OnAsyncRenderDone(void* callback, int fboTexId)
{
    LOGCATD("MyRender::OnAsyncRenderDone callback=%p, fboTexId=%d", callback, fboTexId);
    MyRender* ctx = static_cast<MyRender*>(callback);
    unique_lock<mutex> lock(ctx->m_Mutex);
    ctx->m_FboTextureId = fboTexId;
    ctx->m_Cond.notify_all();
}

bool MyRender::CreateFrameBufferObj(GLuint& fboId, GLuint& textureId, int w, int h) {
    // 创建并初始化 FBO 纹理
    bool ret = true;
    do
    {
        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, GL_NONE);

        // 创建并初始化 FBO
        glGenFramebuffers(1, &fboId);                   /* 创建了fbo name */
        glBindFramebuffer(GL_FRAMEBUFFER, fboId);       /* 真正创建了fbo，关联到fboId这个name上 */
        glBindTexture(GL_TEXTURE_2D, textureId);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureId, 0);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            LOGCATE("MyRender:: glCheckFramebufferStatus status != GL_FRAMEBUFFER_COMPLETE");
            ret = false;
            break;
        }
    } while (0);

    glBindTexture(GL_TEXTURE_2D, GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
    return ret;
}