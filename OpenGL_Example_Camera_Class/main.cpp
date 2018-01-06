///This program is a simple collection of cubes in screen with color and texture
///that rotate, with a moving functional camera using ModernOpenGL
///Author: Jorge Andres Gonzalez Borboa
///Following: https://learnopengl.com/#!Getting-started/Camera

///You need GLFW||GLUT||freeGLUT and GLEW||GLAD to compile this example.
///Also stb_image.h to do the texture loading:
///stb_image-v2.16 - public domain image loader-http://nothings.org/stb_image.h
///As well as GLM to do all the linear algebra
///https://glm.g-truc.net/0.9.8/index.html

///Write this define before including GLEW
#define GLEW_STATIC

///Library for loading files, (jgp, jpeg, png, etc)
#define STB_IMAGE_IMPLEMENTATION

/// third-party libraries
#include <windows.h>
#include <GL/glew.h>
#include <GL/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cassert>
#include <stdexcept>

///Use this if the header file is in the include folder
#include <stb_image.h>
///Use this if the header file is in the project
//#include "stb_image.h"


#include <iostream>

using namespace std;
using namespace glm;

#include "Shader.h"
#include "Camera.h"

///\/////////////////Data for the square////////////////////////////////////////
/*
GLfloat vertices[] = {

     //Positions           //Colors            //Texture
     0.5f,  0.5f, 0.5f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   //Top right [0]
     0.5f, -0.5f, 0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   //Bottom right [1]
    -0.5f, -0.5f, 0.5f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   //Bottom left [2]
    -0.5f,  0.5f, 0.5f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    //Top left  [3]

};

///Corresponding indices
///for the triangles
///this info is for the
///EBO
unsigned int indices[] =
{
    0, 1, 3,   // first triangle
    1, 2, 3    // second triangle

};
*/
///\////////////////////////////////////////////////////////////////////////////


///\/////////////////////Data for the cube//////////////////////////////////////

///THE CUBE ISN"T GENERATED IN THE LOCAL CENTER, WE'LL FIX THIS LATER WITH A
///SELF DEFINE LOCAL MATRIX TRANSFORMATION

GLfloat vertices[] = {

    ///Front
    //Position              //Color             //Texture
	1.0,	1.0,	1.0,    1.0f, 0.0f, 0.0f,   1.0f, 1.0f, //Top Right [0]
	0.0f,	1.0,	1.0,    0.0f, 1.0f, 0.0f,   0.0f, 1.0f, //Top Left [1]
	1.0,	0.0f,	1.0,    1.0f, 0.0f, 0.0f,   1.0f, 0.0f, //Bottom Right [2]
	0.0f,	0.0f,	1.0,    0.0f, 1.0f, 0.0f,   0.0f, 0.0f, //Bottom Left [3]

	///Back
	//Position              //Color             //Texture
	1.0,	1.0,	0.0f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f, //Top Right [4]
	0.0f,	1.0,	0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f, //Top Left  [5]
    0.0f,	0.0f,	0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,  //Bottom Left [6]
	1.0f,	0.0f,	0.0f,   1.0f, 1.0f, 0.0f,   1.0f, 0.0f //Bottom Right   [7]

};

// Declares the Elements Array, where the indexs to be drawn are stored
unsigned int indices [] = {
	3, 2, 6, 7, 4, 2, 0,
	3, 1, 6, 5, 4, 1, 0
};
///\////////////////////////////////////////////////////////////////////////////

///\//////////////////////////DECLARATIONS//////////////////////////////////////

///Declare Window
GLFWwindow* window;
///Declare Window aspect ratio
float fScreenWidth = 800.0f;
float fScreenHeight = 600.0f;

///Declare VAO, VBO and EBO
//VAO-Vertex Array Object
//VBO-Vertex Buffer Object
//EBO-Element Buffer Object
unsigned int VBO;
unsigned int VAO;
unsigned int EBO;

///Declare the Texture OpenGL Object
unsigned int iTexture;

///Declare the Second Texture
unsigned int  iTexture2;

///Camera Initial Values
vec3 camPos = vec3(0.0f,0.0f,3.0f);
vec3 camFront = vec3(0.0f,0.0f,-1.0f);
vec3 camRight = vec3(1.0f,0.0f,0.0f);
vec3 camUp = vec3(0.0f,1.0f,0.0f);

///Camera Object
///This is a free roam camera
Camera camera(camPos,camFront,camUp,PERSPECTIVE);
///This camera is locked looking at the center of the world
//Camera camera(camPos,vec3(0,0,0),camUp,PERSPECTIVE,0);

///Time between current and last frame
float deltaTime = 0.0f;
///TimeStamp of last Frame
float lastFrame = 0.0f;

///\////////////////////////////////////////////////////////////////////////////
///Declare the local matrix
mat4 localMat;

///Declare the model matrix
mat4 modelMat;

///Declare the view matrix
mat4 viewMat;

///Declare the projection matrix
mat4 projMat;
///\////////////////////////////////////////////////////////////////////////////


///Declare the position of the cubes
vec3 cubePositions[] = {
  vec3( 0.0f,  0.0f,  0.0f),
  vec3( 2.0f,  5.0f, -15.0f),
  vec3(-1.5f, -2.2f, -2.5f),
  vec3(-3.8f, -2.0f, -12.3f),
  vec3( 2.4f, -0.4f, -3.5f),
  vec3(-1.7f,  3.0f, -7.5f),
  vec3( 1.3f, -2.0f, -2.5f),
  vec3( 1.5f,  2.0f, -2.5f),
  vec3( 1.5f,  0.2f, -1.5f),
  vec3(-1.3f,  1.0f, -1.5f)
};

///\////////////////////////////////////////////////////////////////////////////
void print(vec2 v)
{
    cout << "X: " << v.x << endl;
    cout << "Y: " << v.y << endl;
    cout << endl;

}

void print(vec3 v)
{
    cout << "X: " << v.x << endl;
    cout << "Y: " << v.y << endl;
    cout << "Z: " << v.z << endl;
    cout << endl;
}

void print(vec4 v)
{
    cout << "X: " << v.x << endl;
    cout << "Y: " << v.y << endl;
    cout << "Z: " << v.z << endl;
    cout << "W: " << v.w << endl;
    cout << endl;
}

void print(mat2 m)
{
    for(int i=0; i<2; i++)
    {
        cout << "[";
        for(int j=0; j<2; j++)
        {
            if(j!=1)
                cout << m[i][j] << " | ";
            else
                cout << m[i][j];
        }
        cout << "]" << endl;
    }
    cout << endl;
}

void print(mat3 m)
{
    for(int i=0; i<3; i++)
    {
        cout << "[";
        for(int j=0; j<3; j++)
        {
            if(j!=2)
                cout << m[i][j] << " | ";
            else
                cout << m[i][j];
        }
        cout << "]" << endl;
    }
    cout << endl;
}

void print(mat4 m)
{
    for(int i=0; i<4; i++)
    {
        cout << "[";
        for(int j=0; j<4; j++)
        {
            if(j!=3)
                cout << m[i][j] << " | ";
            else
                cout << m[i][j];
        }
        cout << "]" << endl;
    }
    cout << endl;
}

///This is the callback function for when the window gets resized
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    ///Just adjust the viewport to the new window size
    glViewport(0, 0, width, height);

    fScreenWidth = width;
    fScreenHeight = height;
}

///This is the callback function for input data, keyboard, mouse etc
void processInput(GLFWwindow *window)
{
    ///If the 'esc' key was pressed
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        ///Close the window
        glfwSetWindowShouldClose(window, true);
    }

///\/////////////////////////CAMERA CONTROLS////////////////////////////////////

    ///\//////////////////
    ///CAMERA POSITION////
    ///\//////////////////

    ///Move front and back
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        camera.MoveCamera(FORWARD,deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        camera.MoveCamera(BACKWARD,deltaTime);
    }

    ///Move left and right
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera.MoveCamera(LEFT,deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera.MoveCamera(RIGHT,deltaTime);
    }

    ///Move Up and Down
    if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
    {
        camera.MoveCamera(UP,deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
    {
        camera.MoveCamera(DOWN,deltaTime);
    }

///\////////////////////////////////////////////////////////////////////////////

    ///\/////////////////
    ///CAMERA ROTATION///
    ///\/////////////////

    ///Update new Yaw angle
    if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
       camera.MoveCamera(RIGHT_SPIN,deltaTime);
    }

    if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        camera.MoveCamera(LEFT_SPIN,deltaTime);
    }

    ///Update new Pitch angle
    if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        camera.MoveCamera(UP_SPIN,deltaTime);
    }

    if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {

        camera.MoveCamera(DOWN_SPIN,deltaTime);
    }

}

void mouse_callback(GLFWwindow* window, double xPos, double yPos)
{
    ///Nothing yet.

}

///\///////////////////INITIALIZE ALL THE FRAMEWORKS////////////////////////////
void initialize()
{
    ///Initialize GLFW
    if(glfwInit() == GLFW_TRUE)
    {
        cout << "GLFW initialized successfully" << endl;
    }
    else
    {
        cout << "Unable to initialize GLFW" << endl;
    }

    ///Set GLFW version to 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    ///Uncomment this function for MAC-OS
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    ///Create a window
    window = glfwCreateWindow(fScreenWidth, fScreenHeight, "ModernOpenGL", NULL,
                              NULL);
    if (window == NULL)
    {
        ///If it fails, end the program
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
    }

    ///Set the current context, the window we are currently working in.
    glfwMakeContextCurrent(window);

    ///Register 'framebuffer_size_callback' as the resize window callback
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    ///Register 'mouse_callback' as the mouse input callback
    glfwSetCursorPosCallback(window,mouse_callback);

    ///Set the initial viewport
    glViewport(0, 0, fScreenWidth, fScreenHeight);

    ///Initialize GLEW
    if(glewInit() == GLEW_OK)
    {
        cout << "GLEW initialized successfully" << endl;
    }
    else
    {
        cout << "Unable to initialize GLEW" << endl;
    }


}
///\////////////////////////////////////////////////////////////////////////////


///\////////////////CREATION OF VAOs VBOs and EBOs//////////////////////////////
void setBufferObjects()
{
    ///Generate VBO
    glGenBuffers(1, &VBO);

    ///Generate VAO
    glGenVertexArrays(1, &VAO);

    ///Generate EBO
    glGenBuffers(1, &EBO);

    ///First Bind the VAO, so that all the configuration is saved in this VAO
    glBindVertexArray(VAO);

    ///Bind the VBO to GL_ARRAY_BUFFER
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    ///Bind EBO to GL_ELEMENT_ARRAY_BUFFER
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    ///Populate VBO with data
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
                 GL_STATIC_DRAW);

    ///Populate EBO with data
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
                 GL_STATIC_DRAW);

    ///Set the info of how the VBO must be read
    /// position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);
    /// color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);
    /// texture attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void*)(6* sizeof(float)));
    glEnableVertexAttribArray(2);
}
///\////////////////////////////////////////////////////////////////////////////

void loadTextures(Shader s)
{

///\///////////////////////////FIRST TEXTURE////////////////////////////////////

    ///Generate the OpenGL texture object
    glGenTextures(1, &iTexture);

    ///Active the texture unit before binding
    glActiveTexture(GL_TEXTURE0);

    ///Bind our texture
    glBindTexture(GL_TEXTURE_2D, iTexture);

    ///Set the texture wrapping/filtering options
    ///(on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

     ///Load the texture data
    int width, height, nrChannels;
    unsigned char *data = stbi_load("textures/container.jpg", &width, &height,
                                    &nrChannels, 0);

    if(data)
    {
        ///Populate the object with data and generate its MipMap
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                     GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        cout << "Failed to load texture data" << endl;
    }


    ///Free resources
    stbi_image_free(data);

///\///////////////////////SECOND TEXTURE///////////////////////////////////////

    ///Generate the second texture object
    glGenTextures(1,&iTexture2);

    ///Active the texture unit
    glActiveTexture(GL_TEXTURE1);

    ///Bind it
    glBindTexture(GL_TEXTURE_2D, iTexture2);

    ///Set the texture wrapping/filtering options
    ///(on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    ///Flip image
    stbi_set_flip_vertically_on_load(true);

    ///Load the texture data
    data = stbi_load("textures/face.png", &width, &height, &nrChannels, 0);

    if(data)
    {
        ///Populate the object with data and generate its MipMap
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                     GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        cout << "Failed to load texture data" << endl;
    }

    ///Free resources
    stbi_image_free(data);

    ///SET THE UNIFORM DATA FOR THE FRAGMENT SHADER
    s.use();
    s.setInt("myTexture", 0);
    s.setInt("myTexture2", 1);

}

void setLocalMat(Shader s)
{
    ///Load Identity Matrix
    localMat = mat4();

    ///Set the local coordinates here:

    ///This is the translation to fix the generated offset in the cube data
    ///comment this line if you are drawing the square
    localMat = translate(localMat,vec3(-0.5f,-0.5f,-0.5f));

    ///Set Shader
    s.use();

    ///Set uniform - For the Vertex Shader
    s.setMatrix4fv("localMat",localMat);

}

void setModelMat(Shader s, vec3 vc3Pos, int i)
{
    ///Load Identity Matrix
    modelMat = mat4();

    if(i == 0)
    {
        ///Rotate in the x axis
        modelMat = rotate(modelMat, radians(50.0f) * (float) glfwGetTime(),
                      vec3(0.5f, 1.0f, 0.0f));

        ///Translate to the corresponding position
        modelMat = translate(modelMat, vc3Pos);
    }
    else
    {
         ///Translate to the corresponding position
        modelMat = translate(modelMat, vc3Pos);

        ///Rotate in the x axis
        modelMat = rotate(modelMat, radians(-50.0f) * (float) glfwGetTime(),
                      vec3(0.5f, 1.0f, 0.0f));
    }

    ///Set Shader
    s.use();

    ///Set uniform - For the Vertex Shader
    s.setMatrix4fv("modelMat",modelMat);
}

void setViewMat (Shader s)
{
    ///Load Identity Matrix
    viewMat = mat4();

    ///Populate the view Matrix
    viewMat = camera.GetViewMatrix();

    ///Set Shader
    s.use();

    ///Set uniform - For the Vertex Shader
    s.setMatrix4fv("viewMat", viewMat);
}

void setProjMat (Shader s)
{
    ///Load Identity Matrix
    projMat = mat4();

    ///Projection.
    ///Type: Perspective
    ///FOV: 45 degrees
    ///Aspect Ratio: 800/600
    ///Near Clipping Plane: 0.1
    ///Far Clipping Plane: 100
    projMat = camera.GetProjectionMatrix();

    ///Set Shader
    s.use();

    ///Set uniform - For the Vertex Shader
    s.setMatrix4fv("projMat", projMat);

}

void drawSquare(Shader s)
{
    ///Set the shader program
    s.use();

    ///Set the VAO
    glBindVertexArray(VAO);

    ///Draw
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

}

void drawCube(Shader s)
{
    ///Set the shader program
    s.use();

    ///Set the VAO
    glBindVertexArray(VAO);

    ///Draw
    glDrawElements(GL_TRIANGLE_STRIP, 14, GL_UNSIGNED_INT, 0);
}

void calcDeltaTime()
{
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
}

int main ()
{
    ///Initialize all the frameworks
    initialize();

    ///Compile and Link Shaders into Shader Program
    Shader shader("shaders/vShader.vs","shaders/fShader.fs");

    ///Set all the info regarding buffer Objects
    setBufferObjects();

    ///Load Texture
    loadTextures(shader);

    ///Enable depth testing
    glEnable(GL_DEPTH_TEST);

    ///Draw in Wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    ///Draw in Fillmode, this is default.
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    ///Tell the user to use the arrow to move
    cout << "\n-----------------------------------" << endl;
    cout << "W-A-S-D to move the camera" << endl;
    cout << "page Up and page Down to change camera elevation" << endl;
    cout << "Arrow Keys to rotate the camera" << endl;
    cout << "-----------------------------------" << endl;

    ///This is the render loop *While the window is open*
    while(!glfwWindowShouldClose(window))
    {
        ///Get time in between frames for camera transformations
        calcDeltaTime();

        ///Set background color
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        ///Refresh Color Bit and Z-Buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ///Set the local view Matrix (Local Coordinates)
        setLocalMat(shader);

        ///Draw 10 cubes in different positions
        for(int i=0; i<10; i++)
        {
            ///Set up the Model Matrix (World coordinates)
            vec3 pos = cubePositions[i];

            ///Rotate Around Origin
            //setModelMat(shader,pos,0);

            ///Rotate Around Itself
            setModelMat(shader,pos,1);

            ///Draw
            drawCube(shader);
        }

        ///Set the View Matrix (Camera Coordinates)
        setViewMat(shader);

        ///Set the Projection Matrix (the perspective of the camera)
        setProjMat(shader);

        ///Process user input, in this case if the user presses the 'esc' key
        ///to close the application
        processInput(window);

        ///Swap the Front and Back buffer.
        glfwSwapBuffers(window);

        ///Poll CallBack Events
        glfwPollEvents();

    }

    ///Free resources when application ends.
    glfwTerminate();
    return 0;
}
