#include "player2.h"
#include "base.h"
#include "scene.h"
#include "ammunition.h"
#include "rock.h"
#include "block.h"
#include "crown2.h"
#include "projectile.h"
#include "Explosion.h"

#include <shaders/diffuse_vert_glsl.h>
#include <shaders/diffuse_frag_glsl.h>

using namespace std;
using namespace glm;
using namespace ppgso;

// shared resources
unique_ptr<Mesh> Player2::mesh;
unique_ptr<Texture> Player2::texture;
unique_ptr<Shader> Player2::shader;
std::vector<tinyobj::material_t> Player2::mat;
std::map<std::string, int> Player2::mat_map;

float last_position2 = 0;
float last_position2_x, last_position2_y;

Player2::Player2() {
    // Scale the default model
    scale *= 0.15f;
    shot_number = 10;
    // Initialize static resources if needed
    if (!shader) shader = make_unique<Shader>(diffuse_vert_glsl, diffuse_frag_glsl);
    if (!texture) texture = make_unique<Texture>(image::loadBMP("tuneler/tank_sz.bmp"));
    if (!mesh) mesh = make_unique<Mesh>("tuneler/tank.obj");
    ifstream x ("tuneler/white-plastic.mtl", std::ifstream::binary);
    tinyobj::LoadMtl(mat_map,mat, x);
}

bool Player2::update(Scene &scene, float dt) {
    // Fire delay increment
    fireDelay += dt;

    // Hit detection
    for (auto &obj : scene.objects) {
        // Ignore self in scene
        if (obj.get() == this)
            continue;

        // We only need to collide with asteroids, ignore other objects
        auto rock = dynamic_cast<Rock *>(obj.get());
        auto ammunition = dynamic_cast<Ammunition *>(obj.get());
        auto block = dynamic_cast<Block *>(obj.get());
        auto base = dynamic_cast<Base *>(obj.get());
        auto projectile = dynamic_cast<Projectile *>(obj.get());

        if (ammunition) {
            if (distance(position, ammunition->position) < (ammunition->scale.y + scale.y)) {
                shot_number += 10;
                ammunition->set_full(false);
            }
        }
        if (base) {
            if (base->get_player() == false && shot_number < 10 &&
                distance(position, base->position) < (base->scale.y + scale.y) * 2.0f) {
                shot_number = 10;
            }
        } else {
            if (!rock && !block)last_position2 = 0;
            else {
                if (rock && distance(position, rock->position) < rock->scale.y * 0.77) {
                    last_position2 = 1;
                    position.x = last_position2_x;
                    position.y = last_position2_y;
                }
                if (block && distance(position, block->position) < block->scale.y * 0.77) {
                    last_position2 = 1;
                    position.x = last_position2_x;
                    position.y = last_position2_y;
                }
            }
            if (!projectile)continue;
            else if (projectile->player() == true && distance(position, projectile->position) < projectile->scale.y) {
                // Explode
                auto explosion = make_unique<Explosion>();
                explosion->position = position;
                explosion->scale = scale * 3.0f;
                scene.objects.push_back(move(explosion));
                scene.score1++;
                scene.kill = true;
                //scene.win1 = true;
                // Die
                return false;
            }
        }
    }

    if (scene.score2 == 1 && scene.win2 == true) {
        scene.win2 = false;
        auto crown2 = make_unique<Crown2>();
        crowns2.push_back(move(crown2));
    }

    if (scene.score2 == 1) {
        // Simply update all crown1
        for (auto &obj : crowns2) {
            obj->update(scene, dt);
        }
    }

    if (last_position2 == 0) {
        last_position2_y = position.y;
        last_position2_x = position.x;
    }
    // Keyboard controls
    if (scene.keyboard[GLFW_KEY_A]) {
        position.x += 5 * dt;
        rotation.z = -PI / 8.0f;
        rotation.t = PI * 1.5;
    } else if (scene.keyboard[GLFW_KEY_D]) {
        position.x -= 5 * dt;
        rotation.z = PI / 8.0f;
        rotation.t = PI / 2;
    } else if (scene.keyboard[GLFW_KEY_W]) {
        position.y += 5 * dt;
        rotation.t = 0.0f;
        rotation.z = 0;
    } else if (scene.keyboard[GLFW_KEY_S]) {
        position.y -= 5 * dt;
        rotation.t = PI;
        rotation.z = 0;
    } else {
        rotation.z = 0;
    }
    bool rotationShot = false;
    if (scene.keyboard[GLFW_KEY_Z])rotationShot = true;
    // Firing projectiles
    if ((scene.keyboard[GLFW_KEY_X] || rotationShot == true) && fireDelay > fireRate && shot_number > 0) {
        // Reset fire delay
        fireDelay = 0;
        shot_number--;

        auto projectile = make_unique<Projectile>();
        if (rotation.t == 0.0f)projectile->smer(0, false, rotationShot);
        else if (rotation.t == PI) projectile->smer(1, false, rotationShot);
        else if (rotation.t == PI / 2)projectile->smer(3, false, rotationShot);
        else projectile->smer(2, false, rotationShot);
        projectile->position = position + glm::vec3(0.0f, 0.0f, 0.3f);
        scene.objects.push_back(move(projectile));
    }

    generateModelMatrix();
    return true;
}

int Player2::get_shot_number() {
    return shot_number;
}

void Player2::render(Scene &scene) {
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

    for (auto &obj : crowns2)
        obj->render(scene);
}

void Player2::onClick(Scene &scene) {
    cout << "Player2 has been clicked!" << endl;
}
