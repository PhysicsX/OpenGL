#include <stdio.h>
#include <string.h>
#include <iostream>
#include <malloc.h>
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glext.h>


GLuint loadBMP_custom(const char * imagepath)
{
    unsigned char header[54]; // Each BMP file begins by a 54-bytes header
    unsigned int dataPos;     // Position in the file where the actual data begins
    unsigned int width, height;
    unsigned int imageSize;   // = width*height*3
    // Actual RGB data
    unsigned char * data;

    FILE * file = fopen(imagepath, "rb");
    if (!file)
    {
        printf("Image could not be opened\n"); return 0;
    }
    if (fread(header, 1, 54, file) != 54){ // If not 54 bytes read : problem
        printf("Not a correct BMP file\n");
        return false;
    }

    if (header[0] != 'B' || header[1] != 'M'){
        printf("Not a correct BMP file\n");
        return 0;
    }

    // Read ints from the byte array
    dataPos = *(int*)&(header[0x0A]);
    imageSize = *(int*)&(header[0x22]);
    width = *(int*)&(header[0x12]);
    height = *(int*)&(header[0x16]);
    // Some BMP files are misformatted, guess missing information
    if (imageSize == 0)    imageSize = width*height * 3; // 3 : one byte for each Red, Green and Blue component
    if (dataPos == 0)      dataPos = 54; // The BMP header is done that way
    // Create a buffer
    data = new unsigned char[imageSize];
    // Read the actual data from the file into the buffer
    fread(data, 1, imageSize, file);
    //Everything is in memory now, the file can be closed
    fclose(file);

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Give the image to OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // "Bind" the newly created texture : all future texture functions will modify this texture
    delete [] data;
    return textureID;
}

static GLuint shaderCompile(GLenum type,char *source)
{
    GLuint shader;
    GLint length, result;

    /* create shader object, set the source, and compile */
    shader = glCreateShader(type);
    length = strlen(source);
    glShaderSource(shader, 1, (const char **)&source, &length);
    glCompileShader(shader);

    /* make sure the compilation was successful */
    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
    if(result == GL_FALSE) {
        char *log;

        /* get the shader info log */
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        log = (char*)malloc(length);
        glGetShaderInfoLog(shader, length, &result, log);

        /* print an error message and the info log */
        printf("shaderCompileFromFile(): Unable to compile %s\n",  log);
        free(log);

        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

/*
 * Compiles and attaches a shader of the
 * given type to the given program object.
 */
void shaderAttach(GLuint program, GLenum type, char *shaderSource=NULL)
{
    /* compile the shader */
    GLuint shader = shaderCompile(type,shaderSource);
    if(shader != 0) {
        /* attach the shader to the program */
        glAttachShader(program, shader);

        /* delete the shader - it won't actually be
         * destroyed until the program that it's attached
         * to has been destroyed */
        glDeleteShader(shader);
    }
}
