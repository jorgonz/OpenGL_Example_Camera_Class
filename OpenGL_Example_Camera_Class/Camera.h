#ifndef CAMERA_H_INCLUDED
#define CAMERA_H_INCLUDED

///GLEW
#define GLEW_STATIC
#include <GL/glew.h>


///GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

enum Camera_Movement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN,
    LEFT_SPIN,
    RIGHT_SPIN,
    UP_SPIN,
    DOWN_SPIN
};

enum Camera_Type
{
    ORTHOGRAPHIC,
    PERSPECTIVE
};

enum Camera_Target_Type
{
    FREE_ROAM,
    ANCHORED
};

/// Default camera values
const float YAW        = 270.0f;
const float PITCH      =   0.0f;
const float SPEED      =   2.5f;
const float SENSITIVTY =  20.0f;
const float FOV        =  45.0f;
const float NEARCP     =   0.1f;
const float FARCP      =  100.0f;
const float WIDTH      = 800.0f;
const float HEIGHT     = 600.0f;
const vec3 WORLD_UP    = vec3(0.0f,1.0f,0.0f);

class Camera
{
    private:

        /// Camera Attributes
        vec3 Position;
        vec3 Front;
        vec3 Up;
        vec3 Right;
        vec3 WorldUp;
        vec3 Target;

        ///Field of vision
        float fZoom;

        ///Camera Settings
        Camera_Type type;
        Camera_Target_Type t_type;
        float fNearClippingPlane;
        float fFarClippingPlane;
        float fWidth;
        float fHeight;

        ///Angles
        float fYaw;
        float fPitch;

        ///Movement and Rotation Speed
        float MovementSpeed;
        float RotationSensitivity;

        ///Private Functions
        void updateCameraVectors();

    public:

        ///Constructor
        Camera(vec3 pos, vec3 dir, vec3 up, Camera_Type t);
        Camera(vec3 pos, vec3 tar, vec3 up, Camera_Type t,int);

        ///Public Functions
        mat4 GetViewMatrix();
        mat4 GetProjectionMatrix();

        ///Function to get keyboard input and move the camera
        void MoveCamera(Camera_Movement direction, float deltaTime);

};

///Constructor for Roaming Camera
Camera::Camera(vec3 pos, vec3 dir, vec3 up, Camera_Type t)
{
    Position = pos;
    Up = up;
    Target =  pos + dir;
    Front = dir;

    type = t;

    fYaw = YAW;
    fPitch = PITCH;
    fZoom = FOV;

    MovementSpeed = SPEED;
    RotationSensitivity = SENSITIVTY;
    WorldUp = WORLD_UP;

    fNearClippingPlane = NEARCP;
    fFarClippingPlane = FARCP;

    fWidth = WIDTH;
    fHeight = HEIGHT;

    t_type = FREE_ROAM;

    updateCameraVectors();
}

///Constructor for Anchored Camera
Camera::Camera(vec3 pos, vec3 tar, vec3 up, Camera_Type t,int i)
{
    Position = pos;
    Up = up;
    Target =  tar;

    type = t;

    fYaw = YAW;
    fPitch = PITCH;
    fZoom = FOV;

    MovementSpeed = SPEED;
    RotationSensitivity = SENSITIVTY;
    WorldUp = WORLD_UP;

    fNearClippingPlane = NEARCP;
    fFarClippingPlane = FARCP;

    fWidth = WIDTH;
    fHeight = HEIGHT;

    t_type = ANCHORED;

    updateCameraVectors();
}

///Whenever the camera's position
///changes we must update the corresponding
///vectors
void Camera::updateCameraVectors()
{
    if(t_type == FREE_ROAM)
    {
       ///Calculate the new Front vector
        vec3 newFront;
        newFront.x = cos(radians(fYaw)) * cos(radians(fPitch));
        newFront.y = sin(radians(fPitch));
        newFront.z = sin(radians(fYaw)) * cos(radians(fPitch));

        Front = normalize(newFront);

        ///Also re-calculate the Right and Up vector
        Right = normalize(cross(Front, WorldUp));
        Up    = normalize(cross(Right, Front));
    }
    else if (t_type == ANCHORED)
    {
        ///Calculate the new Front Vector
        vec3 newFront = normalize(Target - Position);

        Front = newFront;

        ///Also re-calculate the Right and Up vector
        Right = normalize(cross(Front, WorldUp));
        Up    = normalize(cross(Right, Front));

    }

}

///This function handles the camera movement
void Camera::MoveCamera(Camera_Movement direction, float deltaTime)
{
    float fCameraSpeed = MovementSpeed * deltaTime;
    float fCameraRotationSpeed = fCameraSpeed * RotationSensitivity;

    switch(direction)
    {
        case FORWARD:
            {
                Position += Front * fCameraSpeed;
                if(t_type == ANCHORED)
                {
                    updateCameraVectors();
                }
                break;
            }
        case BACKWARD:
            {
                Position -= Front * fCameraSpeed;
                if(t_type == ANCHORED)
                {
                    updateCameraVectors();
                }
                break;
            }
        case LEFT:
            {
                Position -= Right * fCameraSpeed;
                if(t_type == ANCHORED)
                {
                    updateCameraVectors();
                }
                break;
            }
        case RIGHT:
            {
                Position += Right * fCameraSpeed;
                if(t_type == ANCHORED)
                {
                    updateCameraVectors();
                }
                break;
            }
        case UP:
            {
                Position += WorldUp * fCameraSpeed;
                if(t_type == ANCHORED)
                {
                    updateCameraVectors();
                }
                break;
            }
        case DOWN:
            {
                Position -= WorldUp * fCameraSpeed;
                if(t_type == ANCHORED)
                {
                    updateCameraVectors();
                }
                break;
            }
        case LEFT_SPIN:
            fYaw -= fCameraRotationSpeed;
            updateCameraVectors();
            break;

        case RIGHT_SPIN:
            fYaw += fCameraRotationSpeed;
            updateCameraVectors();
            break;

        case UP_SPIN:
            fPitch += fCameraRotationSpeed;
            updateCameraVectors();
            break;

        case DOWN_SPIN:
            fPitch -= fCameraRotationSpeed;
            updateCameraVectors();
            break;
    }

    if(t_type == FREE_ROAM)
    {
        ///Recalculate the target of the camera
        Target = Position + Front;
    }
}

///This function returns the corresponding lookAt
///matrix of this camera for the vertex shader
mat4 Camera::GetViewMatrix()
{
    return lookAt(Position, Target, Up);
}

///This function returns the corresponding Projection
///matrix of this camera for the vertex shader
mat4 Camera::GetProjectionMatrix()
{
    if(type == PERSPECTIVE)
    {
       return perspective(radians(FOV), (float)fWidth/(float)fHeight,
                          fNearClippingPlane, fFarClippingPlane);
    }
    else if(type == ORTHOGRAPHIC)
    {
        return ortho(0.0f, fWidth, 0.0f, fHeight, fNearClippingPlane,
              fFarClippingPlane);
    }

    return mat4();

}

#endif // CAMERA_H_INCLUDED
