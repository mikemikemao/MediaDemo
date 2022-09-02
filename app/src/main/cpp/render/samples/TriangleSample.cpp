//
// Created by ByteFlow on 2019/7/9.
//

#include "TriangleSample.h"


TriangleSample::TriangleSample()
{

}

TriangleSample::~TriangleSample()
{
}


void TriangleSample::LoadImage(NativeImage *pImage)
{
	//null implement

}

void TriangleSample::Init()
{
	if(m_ProgramObj != 0)
		return;
	char vShaderStr[] =
			"#version 300 es                          \n"//版本
			"layout(location = 0) in vec4 vPosition;  \n"//使用in关键字，在顶点着色器中声明所有的输入顶点属性(Input Vertex Attribute)
            "layout (location = 1) in vec3 aColor;    \n"
			"out vec3 ourColor;                       \n"
			"void main()                              \n"
			"{                                        \n"
			"   gl_Position = vPosition;              \n"
			"   ourColor    = aColor;                 \n"
			"}                                        \n";

	char fShaderStr[] =
			"#version 300 es                              \n"
			"precision mediump float;                     \n"
			"out vec4 fragColor;                          \n"
			"in vec3 ourColor;                           \n"
			"void main()                                  \n"
			"{                                            \n"
			"   fragColor = vec4 ( ourColor, 1.0 );       \n"
			"}                                            \n";

	m_ProgramObj = GLUtils::CreateProgram(vShaderStr, fShaderStr, m_VertexShader, m_FragmentShader);

}

void TriangleSample::Draw(int screenW, int screenH)
{
	LOGCATE("TriangleSample::Draw");
	if(m_ProgramObj == 0)
		return;
	// ------------------------------------------------------------------
	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
//	float vertices[] = {
//			0.5f,  0.5f, 0.0f,  // top right
//			0.5f, -0.5f, 0.0f,  // bottom right
//			-0.5f, -0.5f, 0.0f,  // bottom left
//			-0.5f,  0.5f, 0.0f   // top left
//	};
	float vertices[] = {
			// 位置              // 颜色
			0.5f, 0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // 右下
			0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // 左下
			-0.5f,  -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,    // 顶部
			-0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 4.0f    // 顶部

	};
	unsigned int indices[] = {  // note that we start from 0!
			0, 1, 2,  // first Triangle
			0, 2, 3   // second Triangle
	};
	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3* sizeof(float)));
	glEnableVertexAttribArray(1);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0);


	// uncomment this call to draw in wireframe polygons.
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// render
	// ------
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// draw our first triangle
	glUseProgram(m_ProgramObj);
	glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
	//glDrawArrays(GL_TRIANGLES, 0, 6);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	//glDrawArrays(GL_TRIANGLES, 0, 3);
	// glBindVertexArray(0); // no need to unbind it every time

	// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
	// -------------------------------------------------------------------------------


	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteProgram(m_ProgramObj);
	m_ProgramObj = 0 ;

}

void TriangleSample::Destroy()
{
	if (m_ProgramObj)
	{
		glDeleteProgram(m_ProgramObj);
		m_ProgramObj = GL_NONE;
	}

}
