#ifndef _PPGSO_SCENE_H
#define _PPGSO_SCENE_H

#include <memory>
#include <map>
#include <list>

#include "object.h"
#include "camera.h"
#include "rock.h"
#include "player1.h"
#include "player2.h"
#include "ammunition.h"

/*
 * Scene is an object that will aggregate all scene related data
 * Objects are stored in a list of objects
 * Keyboard and Mouse states are stored in a map and struct
 */


class Scene {
public:

    bool kill = false;
    int level = 1;
    int score1 = 0;
    int score2 = 0;
    bool win1 = false;
    bool win2 = false;

//    glm::vec3 lightDirection2{-1.5f, -1.5f, -1.0f};
    glm::vec3 lightDirection2{-0.65f, 0.01f, -0.90f};
//    glm::vec3 lightDirection{-6.0f, 2.0f, -12.20f};
    glm::vec3 lightDirection{0.65f, 0.01f, -0.90f};
    glm::vec3 lightColor = glm::vec3(1,1,1);

//    glm::vec3 lightColor2 = glm::vec3(0.98f,0.718f,0.074);
//    glm::vec3 lightColor2 = glm::vec3(1,0,1);
    glm::vec3 lightColor2 = glm::vec3(1,1,0.0f);
//    glm::vec3 lightColor2 = glm::vec3(1,1,0);
    glm::vec3 ambientLightColor = glm::vec3(0.2, 0.2, 0.2);

    /*!
     * Update all objects in the scene
     * @param time
     */
    void update(float time);

    /*!
     * Render all objects in the scene
     */
    void render();


    /*!
     * Pick objects using a ray
     * @param position - Position in the scene to pick object from
     * @param direction - Direction to pick objects from
     * @return Objects - Vector of pointers to intersected objects
     */
//    std::vector<Object*> intersect(const glm::vec3 &position, const glm::vec3 &direction);

    // Camera object
    std::unique_ptr<Camera> camera;

    // All objects to be rendered in scene
    std::list<std::unique_ptr<Object> > objects;

    // Keyboard state
    std::map<int, int> keyboard;

};

#endif // _PPGSO_SCENE_H
