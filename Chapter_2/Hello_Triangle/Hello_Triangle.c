// The MIT License (MIT)
//
// Copyright (c) 2013 Dan Ginsburg, Budirijanto Purnomo
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

//
// Book:      OpenGL(R) ES 3.0 Programming Guide, 2nd Edition
// Authors:   Dan Ginsburg, Budirijanto Purnomo, Dave Shreiner, Aaftab Munshi
// ISBN-10:   0-321-93388-5
// ISBN-13:   978-0-321-93388-1
// Publisher: Addison-Wesley Professional
// URLs:      http://www.opengles-book.com
//            http://my.safaribooksonline.com/book/animation-and-3d/9780133440133
//
// Hello_Triangle.c
//
//    This is a simple example that draws a single triangle with
//    a minimal vertex/fragment shader.  The purpose of this
//    example is to demonstrate the basic concepts of
//    OpenGL ES 3.0 rendering.
#include "esUtil.h"

typedef struct
{
   // Handle to a program object
   GLuint programObject;

} UserData;

///
// Create a shader object, load the shader source, and
// compile the shader.
//
GLuint LoadShader ( GLenum type, const char *shaderSrc )
{
   GLuint shader;
   GLint compiled;

   //1. Create the shader object
   shader = glCreateShader ( type );

   if ( shader == 0 )
   {
      return 0;
   }

   //2. Load the shader source，参数1表示着色器源字符串数量，着色器可以由多个源字符串组成，但只能有一个main函数。
   glShaderSource ( shader, 1, &shaderSrc, NULL );

   //3.Compile the shader
   glCompileShader ( shader );

   // Check the compile status
   glGetShaderiv ( shader, GL_COMPILE_STATUS, &compiled );

   if ( !compiled )
   {
      GLint infoLen = 0;

      glGetShaderiv ( shader, GL_INFO_LOG_LENGTH, &infoLen );

      if ( infoLen > 1 )
      {
         char *infoLog = malloc ( sizeof ( char ) * infoLen );
          //第三个参数为写入日志的长度（减去null终止符），NULL表示不需要知道长度
         glGetShaderInfoLog ( shader, infoLen, NULL, infoLog );
         esLogMessage ( "Error compiling shader:\n%s\n", infoLog );

         free ( infoLog );
      }
       //当一个着色器连接到程序对象后，调用该函数不会立刻删除着色器，而是将其标记为删除，在着色器不再连接到任何程序对象时，释放它的内存。
      glDeleteShader ( shader );
      return 0;
   }

   return shader;

}

///
// Initialize the shader and program object
//
int Init ( ESContext *esContext )
{
   UserData *userData = esContext->userData;
   char vShaderStr[] =
    //声明着色器版本
      "#version 300 es                          \n"
    //layout(location = 0)限定符表示这个变量的位置是顶点属性0
      "layout(location = 0) in vec4 vPosition;  \n"
    //着色器中的main函数表示着色器执行的开始
      "void main()                              \n"
      "{                                        \n"
      "   gl_Position = vPosition;              \n"
      "}                                        \n";

   char fShaderStr[] =
      "#version 300 es                              \n"
      "precision mediump float;                     \n"
    //fragColor的值将会被输出到颜色缓冲区
      "out vec4 fragColor;                          \n"
      "void main()                                  \n"
      "{                                            \n"
      "   fragColor = vec4 ( 1.0, 0.0, 0.0, 1.0 );  \n"
      "}                                            \n";

   GLuint vertexShader;
   GLuint fragmentShader;
   GLuint programObject;
   GLint linked;

   //Load the vertex/fragment shaders
   vertexShader = LoadShader ( GL_VERTEX_SHADER, vShaderStr );
   fragmentShader = LoadShader ( GL_FRAGMENT_SHADER, fShaderStr );

   //4.Create the program object，程序对象是一个容器对象，需要将其与着色器连接，并链接一个最终的可执行程序。
   programObject = glCreateProgram ( );

   if ( programObject == 0 )
   {
      return 0;
   }
    //5.不同的着色器编译为一个着色器对象之后，必须连接到一个程序对象并一起链接，才能绘制图形，OPENGL ES中程序对象必须连接一个顶点着色器和一个片段着色器。注意，着色器可以在任何时候连接，在连接到程序之前不一定需要编译，甚至可以没有源代码。
   glAttachShader ( programObject, vertexShader );
   glAttachShader ( programObject, fragmentShader );

   //6.Link the program,链接程序将确保顶点着色器写入片段着色器所使用的所有顶点着色器输出变量，链接程序确保任何在顶点和片段着色器中都声明的统一变量和统一变量缓冲区的类型相符，链接程序确保最终程序符合具体实现的限制。
   glLinkProgram ( programObject );

   // Check the link status
   glGetProgramiv ( programObject, GL_LINK_STATUS, &linked );

   if ( !linked )
   {
      GLint infoLen = 0;

      glGetProgramiv ( programObject, GL_INFO_LOG_LENGTH, &infoLen );

      if ( infoLen > 1 )
      {
         char *infoLog = malloc ( sizeof ( char ) * infoLen );

         glGetProgramInfoLog ( programObject, infoLen, NULL, infoLog );
         esLogMessage ( "Error linking program:\n%s\n", infoLog );

         free ( infoLog );
      }

       
      glDeleteProgram ( programObject );
      return FALSE;
   }

   // Store the program object
   userData->programObject = programObject;
    //设置颜色缓冲区的清除颜色为白色
   glClearColor ( 1.0f, 1.0f, 1.0f, 0.0f );
   return TRUE;
}

///
// Draw a triangle using the shader pair created in Init()
//
void Draw ( ESContext *esContext )
{
   UserData *userData = esContext->userData;
   GLfloat vVertices[] = {  0.0f,  0.5f, 0.0f,
                            -0.5f, -0.5f, 0.0f,
                            0.5f, -0.5f, 0.0f
                         };

   // Set the viewport
   glViewport ( 0, 0, esContext->width, esContext->height );

   // Clear the color buffer
   glClear ( GL_COLOR_BUFFER_BIT );

   // 将程序设置为活动程序
   glUseProgram ( userData->programObject );

   // 加载顶点数据，indx参数指定通用顶点属性索引，size参数表示顶点数组中微索引引用的顶点属性所指定的分量数量，type参数表示数据格式，normalized参数表示非浮点数据类型转换为浮点值时是否应该规范化，stride参数指定顶点索引I和I+1表示的顶点数据之间的位移，ptr参数在使用客户端顶点数组时表示保存顶点属性数据的缓冲区指针，在使用顶点缓冲区对象时，表示该缓冲区内的偏移量。
   glVertexAttribPointer ( 0, 3, GL_FLOAT, GL_FALSE, 0, vVertices );
   glEnableVertexAttribArray ( 0 );
    //用元素索引为first到first+count-1的元素指定的顶点绘制mode指定的图元
   glDrawArrays ( GL_TRIANGLES, 0, 3 );
}

void Shutdown ( ESContext *esContext )
{
   UserData *userData = esContext->userData;

   glDeleteProgram ( userData->programObject );
}

int esMain ( ESContext *esContext )
{
    //应用程序所需的数据保存在userData中
   esContext->userData = malloc ( sizeof ( UserData ) );
    //使用EGl API创建渲染表面和渲染上下文，ES_WINDOW_RGB参数指定创建一个RGB帧缓冲区,iOS可不调用该函数
   esCreateWindow ( esContext, "Hello Triangle", 320, 240, ES_WINDOW_RGB );

   if ( !Init ( esContext ) )
   {
      return GL_FALSE;
   }
    //注册回调函数
   esRegisterShutdownFunc ( esContext, Shutdown );
   esRegisterDrawFunc ( esContext, Draw );

   return GL_TRUE;
}
