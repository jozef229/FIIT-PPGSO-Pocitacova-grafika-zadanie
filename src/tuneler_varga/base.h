#pragma once

#include <ppgso/ppgso.h>

#include "object.h"

/*!
 * Object representing a rocket base1 that will accelerate from the ship one created
 */
class Base final : public Object {
private:
    static std::unique_ptr<ppgso::Shader> shader;
    static std::unique_ptr<ppgso::Mesh> mesh;
    static std::unique_ptr<ppgso::Texture> texture;

    static std::vector<tinyobj::material_t> mat;
    static std::map<std::string, int> mat_map;

public:
    bool player_number = false;

    /*
     * Create new base1
     */
    Base();

    void player(bool x);

    bool get_player();

    /*!
     * Update base1 position
     * @param scene Scene to update
     * @param dt Time delta
     * @return true to delete the object
     */
    bool update(Scene &scene, float dt) override;

    /*!
     * Render base1
     * @param scene Scene to render in
     */
    void render(Scene &scene) override;

};

