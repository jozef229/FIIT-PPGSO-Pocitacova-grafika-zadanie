#pragma once

#include <ppgso/ppgso.h>

#include "object.h"

/*!
 * Object representing a rocket projectile that will accelerate from the ship one created
 */
class Projectile final : public Object {
private:
    static std::unique_ptr<ppgso::Shader> shader;
    static std::unique_ptr<ppgso::Mesh> mesh;
    static std::unique_ptr<ppgso::Texture> texture;

    static std::vector<tinyobj::material_t> mat;
    static std::map<std::string, int> mat_map;
    float age{0.0f};
    glm::vec3 speed;
    glm::vec3 smerxy;
    bool player_number;
    bool rotationShot = false;
    float realSpeed = 20;
    float g = 6*9.78f;

public:
    /*
     * Create new projectile
     */
    Projectile();

    void smer(int x, bool hrac, bool rotationShot);

    bool player();

    /*!
     * Update projectile position
     * @param scene Scene to update
     * @param dt Time delta
     * @return true to delete the object
     */
    bool update(Scene &scene, float dt) override;

    /*!
     * Render projectile
     * @param scene Scene to render in
     */
    void render(Scene &scene) override;

    /*!
     * Destroy the projectile
     */
    void destroy();
};

