#include "GLUtils.h"
#include "LogUtil.h"
#include <stdlib.h>
#include <cstring>
#include <GLES2/gl2ext.h>

GLuint GLUtils::LoadShader(GLenum shaderType, const char *pSource)
{
    GLuint shader = 0;
	FUN_BEGIN_TIME("GLUtils::LoadShader")
	    //创建着色器
        shader = glCreateShader(shaderType); //1.我们首先要做的是创建一个着色器对象，注意还是用ID来引用的。所以我们储存这个顶点着色器为unsigned int，然后用glCreateShader创建这个着色器：
        if (shader)
        {
            //2.把着色器源码附加到着色器上 然后编译
            glShaderSource(shader, 1, &pSource, NULL);
            glCompileShader(shader);
            GLint compiled = 0;
            //检测是否编译成功
            glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
            if (!compiled)
            {
                GLint infoLen = 0;
                //查看错误信息
                glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
                if (infoLen)
                {
                    char* buf = (char*) malloc((size_t)infoLen);
                    if (buf)
                    {
                        glGetShaderInfoLog(shader, infoLen, NULL, buf);
                        LOGCATE("GLUtils::LoadShader Could not compile shader %d:\n%s\n", shaderType, buf);
                        free(buf);
                    }
                    //删除着色器 回收资源
                    glDeleteShader(shader);
                    shader = 0;
                }
            }
        }
	FUN_END_TIME("GLUtils::LoadShader")
	return shader;
}

GLuint GLUtils::CreateProgram(const char *pVertexShaderSource, const char *pFragShaderSource, GLuint &vertexShaderHandle, GLuint &fragShaderHandle)
{
    GLuint program = 0;
    FUN_BEGIN_TIME("GLUtils::CreateProgram")
        //顶点着色器 读取并编译着色器 得到一个对象
        vertexShaderHandle = LoadShader(GL_VERTEX_SHADER, pVertexShaderSource);
        if (!vertexShaderHandle) return program;
        //fragment shader
        fragShaderHandle = LoadShader(GL_FRAGMENT_SHADER, pFragShaderSource);
        if (!fragShaderHandle) return program;
        //创建着色器程序
        program = glCreateProgram();
        if (program)
        {
            //附加顶点着色器
            glAttachShader(program, vertexShaderHandle);
            CheckGLError("glAttachShader");
            //附加片段着色器
            glAttachShader(program, fragShaderHandle);
            CheckGLError("glAttachShader");
            //链接
            glLinkProgram(program);
            GLint linkStatus = GL_FALSE;
            //检测出错
            glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
            //连接完后 需把对象删掉 不需要了
            glDetachShader(program, vertexShaderHandle);
            glDeleteShader(vertexShaderHandle);
            vertexShaderHandle = 0;
            glDetachShader(program, fragShaderHandle);
            glDeleteShader(fragShaderHandle);
            fragShaderHandle = 0;
            if (linkStatus != GL_TRUE)
            {
                GLint bufLength = 0;
                glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
                if (bufLength)
                {
                    char* buf = (char*) malloc((size_t)bufLength);
                    if (buf)
                    {
                        glGetProgramInfoLog(program, bufLength, NULL, buf);
                        LOGCATE("GLUtils::CreateProgram Could not link program:\n%s\n", buf);
                        free(buf);
                    }
                }
                glDeleteProgram(program);
                program = 0;
            }
        }
    FUN_END_TIME("GLUtils::CreateProgram")
    LOGCATE("GLUtils::CreateProgram program = %d", program);
	return program;
}

GLuint GLUtils::CreateProgramWithFeedback(const char *pVertexShaderSource, const char *pFragShaderSource, GLuint &vertexShaderHandle, GLuint &fragShaderHandle, GLchar const **varying, int varyingCount)
{
    GLuint program = 0;
    FUN_BEGIN_TIME("GLUtils::CreateProgramWithFeedback")
        vertexShaderHandle = LoadShader(GL_VERTEX_SHADER, pVertexShaderSource);
        if (!vertexShaderHandle) return program;

        fragShaderHandle = LoadShader(GL_FRAGMENT_SHADER, pFragShaderSource);
        if (!fragShaderHandle) return program;

        program = glCreateProgram();
        if (program)
        {
            glAttachShader(program, vertexShaderHandle);
            CheckGLError("glAttachShader");
            glAttachShader(program, fragShaderHandle);
            CheckGLError("glAttachShader");

            //transform feedback
            glTransformFeedbackVaryings(program, varyingCount, varying, GL_INTERLEAVED_ATTRIBS);
            GO_CHECK_GL_ERROR();

            glLinkProgram(program);
            GLint linkStatus = GL_FALSE;
            glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);

            glDetachShader(program, vertexShaderHandle);
            glDeleteShader(vertexShaderHandle);
            vertexShaderHandle = 0;
            glDetachShader(program, fragShaderHandle);
            glDeleteShader(fragShaderHandle);
            fragShaderHandle = 0;
            if (linkStatus != GL_TRUE)
            {
                GLint bufLength = 0;
                glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
                if (bufLength)
                {
                    char* buf = (char*) malloc((size_t)bufLength);
                    if (buf)
                    {
                        glGetProgramInfoLog(program, bufLength, NULL, buf);
                        LOGCATE("GLUtils::CreateProgramWithFeedback Could not link program:\n%s\n", buf);
                        free(buf);
                    }
                }
                glDeleteProgram(program);
                program = 0;
            }
        }
    FUN_END_TIME("GLUtils::CreateProgramWithFeedback")
    LOGCATE("GLUtils::CreateProgramWithFeedback program = %d", program);
    return program;
}

void GLUtils::DeleteProgram(GLuint &program)
{
    LOGCATE("GLUtils::DeleteProgram");
    if (program)
    {
        glUseProgram(0);
        glDeleteProgram(program);
        program = 0;
    }
}

void GLUtils::CheckGLError(const char *pGLOperation)
{
    for (GLint error = glGetError(); error; error = glGetError())
    {
        LOGCATE("GLUtils::CheckGLError GL Operation %s() glError (0x%x)\n", pGLOperation, error);
    }

}

GLuint GLUtils::CreateProgram(const char *pVertexShaderSource, const char *pFragShaderSource) {
    GLuint vertexShaderHandle, fragShaderHandle;
    return CreateProgram(pVertexShaderSource, pFragShaderSource, vertexShaderHandle, fragShaderHandle);
}
