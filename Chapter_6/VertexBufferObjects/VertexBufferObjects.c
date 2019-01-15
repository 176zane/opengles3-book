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
// VertexBufferObjects.c
//
//    This example demonstrates drawing a primitive with
//    and without Vertex Buffer Objects (VBOs)
//
#include "esUtil.h"

typedef struct
{
   // Handle to a program object
   GLuint programObject;

   // VertexBufferObject Ids
   GLuint vboIds[2];

   // x-offset uniform location
   GLuint offsetLoc;

} UserData;


#define VERTEX_POS_SIZE       3 // x, y and z
#define VERTEX_COLOR_SIZE     4 // r, g, b, and a

#define VERTEX_POS_INDX       0
#define VERTEX_COLOR_INDX     1


int Init ( ESContext *esContext )
{
   UserData *userData = esContext->userData;
   const char vShaderStr[] =
      "#version 300 es                            \n"
      "layout(location = 0) in vec4 a_position;   \n"
      "layout(location = 1) in vec4 a_color;      \n"
      "uniform float u_offset;                    \n"
      "out vec4 v_color;                          \n"
      "void main()                                \n"
      "{                                          \n"
      "    v_color = a_color;                     \n"
      "    gl_Position = a_position;              \n"
      "    gl_Position.x += u_offset;             \n"
      "}";


   const char fShaderStr[] =
      "#version 300 es            \n"
      "precision mediump float;   \n"
      "in vec4 v_color;           \n"
      "out vec4 o_fragColor;      \n"
      "void main()                \n"
      "{                          \n"
      "    o_fragColor = v_color; \n"
      "}" ;

   GLuint programObject;

   // Create the program object
   programObject = esLoadProgram ( vShaderStr, fShaderStr );
    //获取由统一变量名称name指定的统一变量的位置
   userData->offsetLoc = glGetUniformLocation ( programObject, "u_offset" );

   if ( programObject == 0 )
   {
      return GL_FALSE;
   }

   // Store the program object
   userData->programObject = programObject;
   userData->vboIds[0] = 0;
   userData->vboIds[1] = 0;

   glClearColor ( 1.0f, 1.0f, 1.0f, 0.0f );
   return GL_TRUE;
}

//
// vertices   - pointer to a buffer that contains vertex
//              attribute data
// vtxStride  - stride of attribute data / vertex in bytes
// numIndices - number of indices that make up primitive
//              drawn as triangles
// indices    - pointer to element index buffer.
//
//不使用顶点缓冲区对象进行图元绘制
void DrawPrimitiveWithoutVBOs ( GLfloat *vertices,
                                GLint vtxStride,
                                GLint numIndices,
                                GLushort *indices )
{
    //使用顶点数组，vtxBuf表示保存顶点属性数据的缓冲区的指针。
   GLfloat   *vtxBuf = vertices;
//???这里应该是使用该函数指定一个未使用的缓冲区对象。
   glBindBuffer ( GL_ARRAY_BUFFER, 0 );
   glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, 0 );

   glEnableVertexAttribArray ( VERTEX_POS_INDX );
   glEnableVertexAttribArray ( VERTEX_COLOR_INDX );

   glVertexAttribPointer ( VERTEX_POS_INDX, VERTEX_POS_SIZE,
                           GL_FLOAT, GL_FALSE, vtxStride,
                           vtxBuf );
   vtxBuf += VERTEX_POS_SIZE;

   glVertexAttribPointer ( VERTEX_COLOR_INDX,
                           VERTEX_COLOR_SIZE, GL_FLOAT,
                           GL_FALSE, vtxStride, vtxBuf );

   glDrawElements ( GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT,
                    indices );

   glDisableVertexAttribArray ( VERTEX_POS_INDX );
   glDisableVertexAttribArray ( VERTEX_COLOR_INDX );

}

void DrawPrimitiveWithVBOs ( ESContext *esContext,
                             GLint numVertices, GLfloat *vtxBuf,
                             GLint vtxStride, GLint numIndices,
                             GLushort *indices )
{
   UserData *userData = esContext->userData;
   GLuint   offset = 0;

   // vboIds[0] - 用于存储顶点属性数据
   // vboIds[l] - 用于存储图元索引
   if ( userData->vboIds[0] == 0 && userData->vboIds[1] == 0 )
   {// Only allocate on the first draw
       //分配n个缓冲区对象名称，并在buffer中返回它们。
      glGenBuffers ( 2, userData->vboIds );
       //用于指定当前缓冲区对象，第一次通过该函数绑定缓冲区对象名称时，缓冲区对象以默认状态分配。
      glBindBuffer ( GL_ARRAY_BUFFER, userData->vboIds[0] );
       //创建和初始化或更新缓冲区对象数据存储。size表示缓冲区数据存储大小，以字节数表示。data参数可以为NULL，表示保留的数据存储不进行初始化，若为有效指针，则其内容被复制到分配的数据存储。对于静态几何形状，调用该函数后，客户数据存储将不再需要，可以释放。
      glBufferData ( GL_ARRAY_BUFFER, vtxStride * numVertices,
                     vtxBuf, GL_STATIC_DRAW );
      glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, userData->vboIds[1] );
      glBufferData ( GL_ELEMENT_ARRAY_BUFFER,
                     sizeof ( GLushort ) * numIndices,
                     indices, GL_STATIC_DRAW );
   }

   glBindBuffer ( GL_ARRAY_BUFFER, userData->vboIds[0] );
   glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, userData->vboIds[1] );

   glEnableVertexAttribArray ( VERTEX_POS_INDX );
   glEnableVertexAttribArray ( VERTEX_COLOR_INDX );
    //加载顶点数据，indx参数指定通用顶点属性索引，size参数表示顶点数组中微索引引用的顶点属性所指定的分量数量，type参数表示数据格式，normalized参数表示非浮点数据类型转换为浮点值时是否应该规范化，stride参数指定顶点索引I和I+1表示的顶点数据之间的位移，ptr参数在使用客户端顶点数组时表示保存顶点属性数据的缓冲区指针，在使用顶点缓冲区对象时，表示该缓冲区内的偏移量
   glVertexAttribPointer ( VERTEX_POS_INDX, VERTEX_POS_SIZE,
                           GL_FLOAT, GL_FALSE, vtxStride,
                           ( const void * ) offset );

   offset += VERTEX_POS_SIZE * sizeof ( GLfloat );
   glVertexAttribPointer ( VERTEX_COLOR_INDX,
                           VERTEX_COLOR_SIZE,
                           GL_FLOAT, GL_FALSE, vtxStride,
                           ( const void * ) offset );

   glDrawElements ( GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT,
                    0 );

   glDisableVertexAttribArray ( VERTEX_POS_INDX );
   glDisableVertexAttribArray ( VERTEX_COLOR_INDX );

   glBindBuffer ( GL_ARRAY_BUFFER, 0 );
   glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, 0 );
}

void Draw ( ESContext *esContext )
{
   UserData *userData = esContext->userData;

   // 3 vertices, with (x,y,z) ,(r, g, b, a) per-vertex
   GLfloat vertices[3 * ( VERTEX_POS_SIZE + VERTEX_COLOR_SIZE )] =
   {
      -0.5f,  0.5f, 0.0f,        // v0
      1.0f,  0.0f, 0.0f, 1.0f,  // c0
      -1.0f, -0.5f, 0.0f,        // v1
      0.0f,  1.0f, 0.0f, 1.0f,  // c1
      0.0f, -0.5f, 0.0f,        // v2
      0.0f,  0.0f, 1.0f, 1.0f,  // c2
   };
   // Index buffer data
   GLushort indices[3] = { 0, 1, 2 };

   glViewport ( 0, 0, esContext->width, esContext->height );
   glClear ( GL_COLOR_BUFFER_BIT );
   glUseProgram ( userData->programObject );
   glUniform1f ( userData->offsetLoc, 0.0f );

   DrawPrimitiveWithoutVBOs ( vertices,
                              sizeof ( GLfloat ) * ( VERTEX_POS_SIZE + VERTEX_COLOR_SIZE ),
                              3, indices );

   // Offset the vertex positions so both can be seen
   glUniform1f ( userData->offsetLoc, 1.0f );

   DrawPrimitiveWithVBOs ( esContext, 3, vertices,
                           sizeof ( GLfloat ) * ( VERTEX_POS_SIZE + VERTEX_COLOR_SIZE ),
                           3, indices );
}

void Shutdown ( ESContext *esContext )
{
   UserData *userData = esContext->userData;

   glDeleteProgram ( userData->programObject );
   glDeleteBuffers ( 2, userData->vboIds );
}

int esMain ( ESContext *esContext )
{
   esContext->userData = malloc ( sizeof ( UserData ) );

   esCreateWindow ( esContext, "VertexBufferObjects", 320, 240, ES_WINDOW_RGB );

   if ( !Init ( esContext ) )
   {
      return GL_FALSE;
   }

   esRegisterShutdownFunc ( esContext, Shutdown );
   esRegisterDrawFunc ( esContext, Draw );

   return GL_TRUE;
}
