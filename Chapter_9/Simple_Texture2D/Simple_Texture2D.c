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
// Simple_Texture2D.c
//
//    This is a simple example that draws a quad with a 2D
//    texture image. The purpose of this example is to demonstrate
//    the basics of 2D texturing
//
#include <stdlib.h>
#include "esUtil.h"

typedef struct
{
   // Handle to a program object
   GLuint programObject;

   // Sampler location
   GLint samplerLoc;

   // Texture handle
   GLuint textureId;

} UserData;

///
// Create a simple 2x2 texture image with four different colors
//
GLuint CreateSimpleTexture2D( )
{
   // Texture object handle（纹理对象用一个无符号整数表示，该整数是纹理对象的一个句柄，纹理对象时一个容器对象，保存渲染所需的纹理数据，如图像数据、过滤模式和包装模式）
   GLuint textureId;

   // 2x2 Image, 3 bytes per pixel (R, G, B)
   GLubyte pixels[4 * 3] =
   {
      255,   0,   0, // Red
        0, 255,   0, // Green
        0,   0, 255, // Blue
      255, 255,   0  // Yellow
   };

   // 设置打包或解包选项：pname指定设置的像素存储类型，有影响glTextImage2D等的数据从内存中解包的方式和glReadPixels时数据打包到内存中的方式；paramd指定打包或解包选项的整数值。此处将解包对齐设置为1，因为着像素行被认定为从1个字节便捷开始，也就是数据被紧密打包。
   glPixelStorei ( GL_UNPACK_ALIGNMENT, 1 );

   // 生成纹理对象：n指定要生成的纹理对象数量；texture是一个保存n个纹理对象ID的无符号整数数组
   glGenTextures ( 1, &textureId );

   // 一旦用glGenTexture生成了纹理对象ID，应用程序就必须绑定纹理对象进行操作
   glBindTexture ( GL_TEXTURE_2D, textureId );

    // 将图像数据加载到纹理对象：target指定纹理绑定的目标；level指定要加载的mip级别，第一个为0，后续递增；internalFarmat是纹理存储的内部格式；width图像的像素宽度；height图像的像素高度；border保留的与桌面OpenGL兼容的参数，直接设置为0；iformat输入的纹理格式数据；type输入像素数据的类型；pixels包含图像的实际像素数据
   glTexImage2D ( GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels );

   // 设置过滤模式
   glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
   glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

   return textureId;

}


///
// Initialize the shader and program object
//
int Init ( ESContext *esContext )
{
   UserData *userData = esContext->userData;
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

   char fShaderStr[] =
      "#version 300 es                                     \n"
      "precision mediump float;                            \n"
      "in vec2 v_texCoord;                                 \n"
      "layout(location = 0) out vec4 outColor;             \n"
      "uniform sampler2D s_texture;                        \n"
      "void main()                                         \n"
      "{                                                   \n"
      "  outColor = texture( s_texture, v_texCoord );      \n"
      "}                                                   \n";

   // Load the shaders and get a linked program object
   userData->programObject = esLoadProgram ( vShaderStr, fShaderStr );

   // Get the sampler location
   userData->samplerLoc = glGetUniformLocation ( userData->programObject, "s_texture" );

   // Load the texture
   userData->textureId = CreateSimpleTexture2D ();

   glClearColor ( 1.0f, 1.0f, 1.0f, 0.0f );
   return TRUE;
}

///
// Draw a triangle using the shader pair created in Init()
//
void Draw ( ESContext *esContext )
{
   UserData *userData = esContext->userData;
   GLfloat vVertices[] = { -0.5f,  0.5f, 0.0f,  // Position 0
                            0.0f,  0.0f,        // TexCoord 0 
                           -0.5f, -0.5f, 0.0f,  // Position 1
                            0.0f,  1.0f,        // TexCoord 1
                            0.5f, -0.5f, 0.0f,  // Position 2
                            1.0f,  1.0f,        // TexCoord 2
                            0.5f,  0.5f, 0.0f,  // Position 3
                            1.0f,  0.0f         // TexCoord 3
                         };
   GLushort indices[] = { 0, 1, 2, 0, 2, 3 };

   // Set the viewport
   glViewport ( 0, 0, esContext->width, esContext->height );

   // Clear the color buffer
   glClear ( GL_COLOR_BUFFER_BIT );

   // Use the program object
   glUseProgram ( userData->programObject );

   // Load the vertex position
   glVertexAttribPointer ( 0, 3, GL_FLOAT,
                           GL_FALSE, 5 * sizeof ( GLfloat ), vVertices );
   // Load the texture coordinate
   glVertexAttribPointer ( 1, 2, GL_FLOAT,
                           GL_FALSE, 5 * sizeof ( GLfloat ), &vVertices[3] );

   glEnableVertexAttribArray ( 0 );
   glEnableVertexAttribArray ( 1 );

   // Bind the texture
   glActiveTexture ( GL_TEXTURE0 );
   glBindTexture ( GL_TEXTURE_2D, userData->textureId );

   // Set the sampler texture unit to 0
   glUniform1i ( userData->samplerLoc, 0 );

   glDrawElements ( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices );
}

///
// Cleanup
//
void ShutDown ( ESContext *esContext )
{
   UserData *userData = esContext->userData;

   // Delete texture object
   glDeleteTextures ( 1, &userData->textureId );

   // Delete program object
   glDeleteProgram ( userData->programObject );
}


int esMain ( ESContext *esContext )
{
   esContext->userData = malloc ( sizeof ( UserData ) );

   esCreateWindow ( esContext, "Simple Texture 2D", 320, 240, ES_WINDOW_RGB );

   if ( !Init ( esContext ) )
   {
      return GL_FALSE;
   }

   esRegisterDrawFunc ( esContext, Draw );
   esRegisterShutdownFunc ( esContext, ShutDown );

   return GL_TRUE;
}
