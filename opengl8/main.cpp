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

char* vertexShaderSrc = GLSL(
            in vec3 pos;
            uniform float Scale;
void main() {
    gl_Position = vec4(pos, 1.0)+vec4(Scale,0,0,0);
}
);

char* fragmentShaderSrc = GLSL(
            precision mediump float;
            out vec4 FragColor;
            void main() {
                FragColor = vec4( 0.0, 1.0, 1.0, 1.0);
});

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
    };

    MyVertex pvertex[3]={{0,0,0},{1,0,0},{0,1,0}};

    GLuint VertexVBOID=0,IndexVBOID=0;
    glGenBuffers(1, &VertexVBOID);
    glBindBuffer(GL_ARRAY_BUFFER, VertexVBOID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pvertex), &pvertex[0].x, GL_STATIC_DRAW);

    //create the indices (element buffer)
    ushort pindices[sizeof(pvertex)/sizeof(MyVertex)];
    for(int i=0;i<sizeof(pindices)/sizeof(ushort);++i) pindices[i]=i;

    glGenBuffers(1, &IndexVBOID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexVBOID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(pindices), pindices, GL_STATIC_DRAW);

    GLuint program = glCreateProgram();
    shaderAttach(program, GL_VERTEX_SHADER,vertexShaderSrc);
    shaderAttach(program, GL_FRAGMENT_SHADER,fragmentShaderSrc);
    glLinkProgram(program);

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

        glClearColor( 0.01f, 0, 0.0f, 0.0f );
        glClear( GL_COLOR_BUFFER_BIT |GL_DEPTH_BUFFER_BIT);
        glClearDepth(1.0f);   //is always to set to 1.0 when clearing
        glEnable(GL_DEPTH_TEST);


        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexVBOID);
        glBindBuffer(GL_ARRAY_BUFFER, VertexVBOID);

        glUseProgram(program);
        GLint posAttrib = glGetAttribLocation(program, "pos");
        glEnableVertexAttribArray(posAttrib);
        glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

        //glUseProgram(program);
        GLint locScale = glGetUniformLocation(program, "Scale");
        static float Scale1 =-1;Scale1+=.001;if(1<Scale1) Scale1=-1;
        glUniform1f(locScale, Scale1);

        glDrawElements(GL_TRIANGLES, sizeof(pindices)/sizeof(ushort), GL_UNSIGNED_SHORT, 0);
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
