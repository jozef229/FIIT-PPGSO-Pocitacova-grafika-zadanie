#pragma once

#include <ppgso/ppgso.h>

#include "object.h"

/*!
 * Object representing a rocket ammo that will accelerate from the ship one created
 */
class Ammo final : public Object {
private:
    static std::unique_ptr<ppgso::Shader> shader;
    static std::unique_ptr<ppgso::Mesh> mesh;
    static std::unique_ptr<ppgso::Texture> texture;
    static std::vector<tinyobj::material_t> mat;
    static std::map<std::string, int> mat_map;
    int bar;

public:
    /*
     * Create new ammo
     */
    Ammo();


    void set_bar(int x);

    /*!
     * Update ammo position
     * @param scene Scene to update
     * @param dt Time delta
     * @return true to delete the object
     */
    bool update(Scene &scene, float dt) override;

    /*!
     * Render ammo
     * @param scene Scene to render in
     */
    void render(Scene &scene) override;

};

