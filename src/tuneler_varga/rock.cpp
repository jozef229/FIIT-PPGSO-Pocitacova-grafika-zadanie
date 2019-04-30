#include <glm/gtc/random.hpp>
#include "rock.h"
#include "projectile.h"
#include "Explosion.h"

#include <shaders/diffuse_vert_glsl.h>
#include <shaders/diffuse_frag_glsl.h>

using namespace std;
using namespace glm;
using namespace ppgso;

// Static resources
unique_ptr<Mesh> Rock::mesh;
unique_ptr<Texture> Rock::texture;
unique_ptr<Shader> Rock::shader;
std::vector<tinyobj::material_t> Rock::mat;
std::map<std::string, int> Rock::mat_map;

Rock::Rock() {

    // Initialize static resources if needed
    if (!shader) shader = make_unique<Shader>(diffuse_vert_glsl, diffuse_frag_glsl);
    if (!texture) texture = make_unique<Texture>(image::loadBMP("tuneler/rock.bmp"));
    if (!mesh) mesh = make_unique<Mesh>("tuneler/cube.obj");
    ifstream x ("tuneler/white-rubber.mtl", std::ifstream::binary);
    tinyobj::LoadMtl(mat_map,mat, x);
}

bool Rock::update(Scene &scene, float dt) {
    // Count time alive
    age += dt;


    // Collide with scene
    for (auto &obj : scene.objects) {
        // Ignore self in scene
        if (obj.get() == this) continue;

        // We only need to collide with and projectiles, ignore other objects
        auto projectile = dynamic_cast<Projectile *>(obj.get()); //dynamic_pointer_cast<Projectile>(obj);
        if (!projectile) continue;

        // When colliding with other rocks make sure the object is older than .5s
        // This prevents excessive collisions when rocks explode.
        if (age < 0.5f) continue;

        // Compare distance to approximate size of the rock estimated from scale.
        if (distance(position, obj->position) + (obj->scale.y * 0.1f) <= (obj->scale.y + scale.y) * 0.5f) {
            int pieces = 3;

            // Too small to split into pieces
            if (scale.y < 0.5) pieces = 0;

            // The projectile will be destroyed
            if (projectile) projectile->destroy();

            // Generate smaller rocks
            explode(scene, (obj->position + position) / 2.0f, (obj->scale + scale) / 2.0f, pieces);

            // Destroy self
            return false;
        }
    }

    // Generate modelMatrix from position, rotation and scale
    generateModelMatrix();

    return true;
}


void Rock::explode(Scene &scene, vec3 explosionPosition, vec3 explosionScale, int pieces) {
    // Generate explosion
    auto explosion = make_unique<Explosion>();
    explosion->position = explosionPosition;
    explosion->scale = explosionScale;
    explosion->speed = speed / 2.0f;
    scene.objects.push_back(move(explosion));

    // Generate smaller rocks
    for (int i = 0; i < pieces; i++) {
        auto rock = make_unique<Rock>();
        rock->position = position;
        rock->rotMomentum = rotMomentum;
        float factor = (float) pieces / 2.0f;
        rock->scale = scale / factor;
        scene.objects.push_back(move(rock));
    }
}

void Rock::render(Scene &scene) {
    glEnable(GL_DEPTH_TEST);
    // NOTE: this object does not use camera, just renders the entire quad as is
    shader->use();
    shader->setUniform("LightDirection", scene.lightDirection);
    shader->setUniform("LightColor", scene.lightColor);

    shader->setUniform("LightDirection2", scene.lightDirection2);
    shader->setUniform("LightColor2", scene.lightColor2);
    shader->setUniform("AmbientLightColor", scene.ambientLightColor);
    shader->setUniform("CameraPosition", scene.camera->position);
    shader->setUniform("MatShininess", mat.data()->shininess * 128);
    shader->setUniform("MatAmbient", vec3(mat.data()->ambient[0],mat.data()->ambient[1],mat.data()->ambient[2]));
    shader->setUniform("MatDiffuse", vec4(mat.data()->diffuse[0],mat.data()->diffuse[1],mat.data()->diffuse[2], 1.0f));
    shader->setUniform("MatSpacular", vec3(mat.data()->specular[0],mat.data()->specular[1],mat.data()->specular[2]));
    // Pass UV mapping offset to the shader

    // Render mesh, not using any projections, we just render in 2D
    shader->setUniform("ModelMatrix", modelMatrix);
    shader->setUniform("ViewMatrix", scene.camera->viewMatrix);
    shader->setUniform("ProjectionMatrix", scene.camera->projectionMatrix);
    shader->setUniform("Texture", *texture);
    mesh->render();

    glDepthMask(GL_TRUE);
}



