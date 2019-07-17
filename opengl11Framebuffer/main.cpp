#include <stdio.h>
#include <iostream>
#include <string>
#include <X11/Xlib.h>
#include <malloc.h>

#define GLX_CONTEXT_MAJOR_VERSION_ARB		0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB		0x2092


#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glext.h>
#include "/home/ulas/soil/src/SOIL.h"

#include <math.h>
using namespace std;



GLuint loadBMP_custom(const char * imagepath);

void drawText(Display *d,Window w,string text)
{
    XGCValues values;
    GC gc = XCreateGC(d, w, 0, &values);
    XSetForeground(d, gc, 0xff00ff);
    XDrawString(d,w,gc,1,10,text.c_str(),text.length());
    XFreeGC(d, gc);
}

GLXContext CreateGLContext(Display *d,Window w)
{
    static int attributeList[] = { GLX_RGBA, GLX_DOUBLEBUFFER, GLX_RED_SIZE, 1, GLX_GREEN_SIZE, 1, GLX_BLUE_SIZE, 1,GLX_DEPTH_SIZE,16,None };
    XVisualInfo *vi = glXChooseVisual(d, DefaultScreen(d),attributeList);
    GLXContext ctx = glXCreateContext(d, vi, 0, GL_TRUE);
    glXMakeCurrent (d, w, ctx);
    fflush(stdout);fflush(stderr);
}

void DestroyContext(Display *d,GLXContext ctx)
{
    glXMakeCurrent(d, None, NULL);
    glXDestroyContext(d, ctx);
    ctx=NULL/*Nnullptr*/;
}

#define GLSL(src) "#version 300 es \n" #src
void shaderAttach(GLuint program, GLenum type,char *shaderSource=NULL);

//char* vertexShaderSrc = GLSL(
//            in vec3 pos;
//            uniform float Scale;
//void main() {
//    float r=sqrt(pos.x*pos.x+pos.y*pos.y);


//    if(r>0.0)  //this fix was missing for us earlier
//    {
//        float angle= acos(pos.x/r)+Scale;
//        float angle1=asin(pos.y/r)+Scale;

//        gl_Position = vec4(cos(angle)*r,sin(angle1)*r, 0, 1.0);
//    }
//    else
//        gl_Position = vec4(pos, 1.0);
//}
//);

//char* vertexShaderSrc = GLSL(
//            in vec3 pos;
//            uniform float Scale;
//void main() {
//    gl_Position = vec4(pos, 1.0)+vec4(Scale,0,0,0);
//}
//);

char* vertexShaderSrc = GLSL(
in vec3 pos;
in vec2 tex;

out vec2 texOut;

void main() {
    gl_Position.xyz = pos.xyz;
    gl_Position.w=1.0f;
    texOut=tex;
}
);

char* fragmentShaderSrc = GLSL(
precision mediump float;

uniform sampler2D colorMap0;

in vec2 texOut;
out vec4 FragColor;

void main() {
    vec2 texOut1 = texOut;
    vec2 texOut2 = texOut;
    vec2 texOut3 = texOut;
    vec2 texOut4 = texOut;
    vec2 texOut5 = texOut;
    vec2 texOut6 = texOut;
    vec2 texOut7 = texOut;
    vec2 texOut8 = texOut;

    texOut1.x = texOut1.x+0.01;
    //texOut1.y = texOut1.y+0.02;
    texOut2.x = texOut2.x+-0.01;
    texOut3.y = texOut3.y+0.01;
    texOut4.y = texOut4.y-0.01;

    texOut5.x = texOut5.x+0.014;
    texOut5.y = texOut5.y+0.014;

    texOut6.x = texOut6.x-0.014;
    texOut6.y = texOut6.y+0.014;

    texOut7.x = texOut7.x-0.014;
    texOut7.y = texOut7.y-0.014;


    texOut8.x = texOut8.x+0.014;
    texOut8.y = texOut8.y-0.014;

    vec4 col1 = texture(colorMap0, texOut.xy);
    vec4 col2 = texture(colorMap0, texOut1.xy);
    vec4 col3 = texture(colorMap0, texOut2.xy);
    vec4 col4 = texture(colorMap0, texOut3.xy);
    vec4 col5 = texture(colorMap0, texOut4.xy);
    vec4 col6 = texture(colorMap0, texOut5.xy);
    vec4 col7 = texture(colorMap0, texOut6.xy);
    vec4 col8 = texture(colorMap0, texOut7.xy);
    vec4 col9 = texture(colorMap0, texOut8.xy);


    FragColor = (col1 +col9 + col2 + col3 + col4 + col5 + col6 + col7 + col8) / 9.0;

//        FragColor.r=1.0-col1.r;
//        FragColor.g=1.0-col1.g;
//        FragColor.b=1.0-col1.b;
}
);



char* fragmentShaderSrc2 = GLSL(
precision mediump float;

uniform sampler2D colorMap0;

in vec2 texOut;
out vec4 FragColor;

void main() {

    vec4 col1 = texture(colorMap0, texOut.xy);
        FragColor=col1;
}
);

//char* fragmentShaderSrc = GLSL(
//            precision mediump float;
//            out vec4 FragColor;
//            void main() {
//                FragColor = vec4( 0.0, 1.0, 1.0, 1.0);
//});

int main()
{
    Display *display = XOpenDisplay(NULL);

    Window root = DefaultRootWindow(display);  /*get the root window*/
    Window parentmainw = XCreateSimpleWindow(display, root, 10,10,800,800,0,0,0);
    Window mainw = XCreateSimpleWindow(display, parentmainw, 0,20,700,700,0,0,0);

    XMapWindow(display, parentmainw);
    XMapWindow(display, mainw); //show the window and its inferiors

    XSelectInput(display, parentmainw, KeyPressMask);  //hangle key presses

    static Atom wmDeleteMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, parentmainw, &wmDeleteMessage, 1);

    GLXContext ctx=CreateGLContext(display,mainw);

    struct MyVertex
    {
        float x, y, z;        //Vertex
        float tx,ty;          //for texture stage 1
    };

   // MyVertex pvertex[3] = {{0,0,0,0,0},{1,0,0,1,0},{0,1,0,0,1}};
    MyVertex pvertex[6] = {{0,0,0,0,0}
                          ,{0,1,0,0,1}
                          ,{1,1,0,1,1}
                          ,{1,1,0,1,1}
                          ,{1,0,0,1,0}
                          ,{0,0,0,0,0}};


    GLuint VertexVBOID=0,IndexVBOID=0;
    glGenBuffers(1, &VertexVBOID);
    glBindBuffer(GL_ARRAY_BUFFER, VertexVBOID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pvertex), &pvertex[0].x, GL_STATIC_DRAW);

    GLuint program = glCreateProgram();
    shaderAttach(program, GL_VERTEX_SHADER,vertexShaderSrc);
    shaderAttach(program, GL_FRAGMENT_SHADER,fragmentShaderSrc);
    glLinkProgram(program);

    GLuint program2 = glCreateProgram();
    shaderAttach(program2, GL_VERTEX_SHADER,vertexShaderSrc);
    shaderAttach(program2, GL_FRAGMENT_SHADER,fragmentShaderSrc2);
    glLinkProgram(program2);

    GLint result=0;
    glGetProgramiv(program, GL_LINK_STATUS, &result);
    if(result == GL_FALSE) {
        GLint length;
        char *log;
        /* get the program info log */
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        log = (char*)malloc(length);
        glGetProgramInfoLog(program, length, &result, log);
        /* print an error message and the info log */
        fprintf(stderr, "sceneInit(): Program linking failed: %s\n", log);
        free(log);
        /* delete the program */
        glDeleteProgram(program);
        printf("shader linking failed\n");
    }


    GLuint colorTexture;
    glGenTextures(1, &colorTexture);
    glBindTexture(GL_TEXTURE_2D, colorTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 600,600, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);

    GLuint depthTexture;
    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, 600,600, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);

    GLuint fb;
    glGenFramebuffersEXT(1, &fb);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb);
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, colorTexture, 0/*mipmap level*/);
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, depthTexture, 0/*mipmap level*/);

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

    GLuint texture = SOIL_load_OGL_texture("/home/ulas/opengl3/NeHe.bmp",SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);


    bool running=true;
    while(running)
    {
        drawText(display,parentmainw,"Parent");
        drawText(display,mainw,"child");
        while(XPending(display))  //message pump
        {
            XEvent		event;
            XNextEvent(display, &event);

            switch (event.type)
            {
            case Expose:
                cout<<"exposed"<<endl<<flush;
                break;
            case KeyPress:
                running=false;  //break out of the loop
                break;
            case ClientMessage:
                if (event.xclient.data.l[0] == wmDeleteMessage)
                    running = false;
                break;
            default:
                break;
            }
        }
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb);

        glClearColor( 0.01f, 0, 0.5f, 0.0f );
        glClear( GL_COLOR_BUFFER_BIT |GL_DEPTH_BUFFER_BIT);
        glClearDepth(1.0f);   //is always to set to 1.0 when clearing
        glEnable(GL_DEPTH_TEST);

        glBindBuffer(GL_ARRAY_BUFFER, VertexVBOID);

        glUseProgram(program2);
        GLint posAttrib2 = glGetAttribLocation(program2, "pos");
        glEnableVertexAttribArray(posAttrib2);
        glVertexAttribPointer(posAttrib2, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);

        GLint texAttrib1 = glGetAttribLocation(program2, "tex");
        glEnableVertexAttribArray(texAttrib1);
        glVertexAttribPointer(texAttrib1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)( 3*sizeof(float)));

        //glUseProgram(program);
        GLint gli1 = glGetUniformLocation(program2, "colorMap0");
        glUniform1i(gli1, 0);


        glActiveTexture(GL_TEXTURE0); // stage ->
        glBindTexture(GL_TEXTURE_2D, texture);


        glDrawArrays(GL_TRIANGLES,0,6);


        // glDrawElements(GL_TRIANGLES, sizeof(pindices)/sizeof(ushort), GL_UNSIGNED_SHORT, 0);
        glUseProgram(0);

        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

        glClearColor( 0.5f, 0.5f, 0.5f, 0.0f );
        glClear( GL_COLOR_BUFFER_BIT |GL_DEPTH_BUFFER_BIT);
        glClearDepth(1.0f);   //is always to set to 1.0 when clearing
        glEnable(GL_DEPTH_TEST);

        glBindBuffer(GL_ARRAY_BUFFER, VertexVBOID);

        glUseProgram(program);

        GLint posAttrib = glGetAttribLocation(program, "pos");
        glEnableVertexAttribArray(posAttrib);
        glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);

        GLint texAttrib = glGetAttribLocation(program, "tex");
        glEnableVertexAttribArray(texAttrib);
        glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)( 3*sizeof(float)));

        //glUseProgram(program);
        GLint gli = glGetUniformLocation(program, "colorMap0");
        glUniform1i(gli, 0);


        glActiveTexture(GL_TEXTURE0); // stage ->
        glBindTexture(GL_TEXTURE_2D, colorTexture);


        glDrawArrays(GL_TRIANGLES,0,6);


        // glDrawElements(GL_TRIANGLES, sizeof(pindices)/sizeof(ushort), GL_UNSIGNED_SHORT, 0);
        glUseProgram(0);

        glXSwapBuffers (display, mainw);
    }

    glDeleteProgram(program);
    DestroyContext(display,ctx);

    XDestroyWindow(display, mainw);
    XDestroyWindow(display, parentmainw);
    XCloseDisplay(display);
    return 0;
}
