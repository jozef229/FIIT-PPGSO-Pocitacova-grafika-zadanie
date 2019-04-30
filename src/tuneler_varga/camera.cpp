#include <glm/glm.hpp>
#include "ammunition.h"
#include "camera.h"

using namespace std;
using namespace glm;
using namespace ppgso;
int posun = 4;
bool pomocPosun = false;


Camera::Camera(float fow, float ratio, float near, float far) {
    posun = 4;
    float fowInRad = (PI / 180.0f) * fow;
    projectionMatrix = perspective(fowInRad, ratio, near, far);
}

void Camera::update(int dt) {
    float cameraSpeed = 2.0f;
    if (posun == 0) {
        cameraPos += cameraSpeed * cameraUp;
        posun = -1;
        pomocPosun = false;
    }
    if (posun == 1) {
        cameraPos -= cameraSpeed * cameraUp;
        posun = -1;
        pomocPosun = false;
    }
    if (posun == 2) {
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        posun = -1;
        pomocPosun = false;
    }
    if (posun == 3) {
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        posun = -1;
        pomocPosun = false;
    }
    if (posun == 4) {
        cameraPos = glm::vec3(0.0f, 0.0f, -27.0f);
        cameraFront = glm::vec3(0.0f, 0.0f, +1.0f);
        cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
        posun = -1;
        pomocPosun = false;
    }
    if (posun == 5) {
        cameraPos = glm::vec3(0.0f, -3.0f, -5.0f);
        cameraFront = glm::vec3(0.0f, 0.8f, 1.0f);
        cameraUp = glm::vec3(0.0f, 0.5f, 0.0f);
        cameraPos -= 16.0f * cameraFront;
        posun = -1;
        pomocPosun = false;
    }
    if (posun == 6) {
        if (pomocPosun == false) {
            pomocPosun = true;
            age = 0;
        }
        if (age < 4) {
            if (age % 2 == 0)
                cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * 1.0001f;
            else
                cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * 1.0001f;
            age += dt;
        } else {
            posun = -1;
            pomocPosun = false;
        }
    }
    if (posun == 7) {
        cameraPos += cameraSpeed * cameraFront;
        posun = -1;
        pomocPosun = false;
    }
    if (posun == 8) {
        cameraPos -= cameraSpeed * cameraFront;
        posun = -1;
        pomocPosun = false;
    }


    viewMatrix = lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}

void Camera::set_Posun(int x) {
    posun = x;

}


glm::vec3 Camera::cast(float u, float v) {
    // Create point in Screen coordinates
    glm::vec4 screenPosition{u, v, 0.0f, 1.0f};

    // Use inverse matrices to get the point in world coordinates
    auto invProjection = glm::inverse(projectionMatrix);
    auto invView = glm::inverse(viewMatrix);

    // Compute position on the camera plane
    auto planePosition = invView * invProjection * screenPosition;
    planePosition /= planePosition.w;

    // Create direction vector
    auto direction = glm::normalize(planePosition - vec4{position, 1.0f});
    return vec3{direction};
}
