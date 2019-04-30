#pragma once

#include <ppgso/ppgso.h>

#include "object.h"
#include "crown2.h"

/*!
 * Simple object representing the player2
 * Reads keyboard status and manipulates its own position
 * On Update checks collisions with Asteroid objects in the scene
 */
class Player2 final : public Object {
private:
    // Static resources (Shared between instances)
    static std::unique_ptr<ppgso::Mesh> mesh;
    static std::unique_ptr<ppgso::Shader> shader;
    static std::unique_ptr<ppgso::Texture> texture;
    static std::vector<tinyobj::material_t> mat;
    static std::map<std::string, int> mat_map;

    // Delay fire and fire rate
    float fireDelay{0.0f};
    float fireRate{0.3f};
    glm::vec3 fireOffset{0.0f, 0.0f, 0.0f};
    int shot_number;
    std::list<std::unique_ptr<Crown2> > crowns2;

public:
    /*!
     * Create a new player2
     */
    Player2();

    int get_shot_number();

    /*!
     * Update player2 position considering keyboard inputs
     * @param scene Scene to update
     * @param dt Time delta
     * @return true to delete the object
     */
    bool update(Scene &scene, float dt) override;

    /*!
     * Render player2
     * @param scene Scene to render in
     */
    void render(Scene &scene) override;


    /*!
     * Player2 click event
     * @param scene
     */
    void onClick(Scene &scene) override;
};

