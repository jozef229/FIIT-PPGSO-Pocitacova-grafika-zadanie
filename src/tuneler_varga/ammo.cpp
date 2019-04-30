
#include "scene.h"
#include "ammo.h"
#include "player2.h"
#include "player1.h"

#include <shaders/diffuse_vert_glsl.h>
#include <shaders/diffuse_frag_glsl.h>

using namespace std;
using namespace glm;
using namespace ppgso;

// shared resources
unique_ptr<Mesh> Ammo::mesh;
unique_ptr<Shader> Ammo::shader;
unique_ptr<Texture> Ammo::texture;
std::vector<tinyobj::material_t> Ammo::mat;
std::map<std::string, int> Ammo::mat_map;

float help;


Ammo::Ammo() {
    scale.y *= 1.4f;
    bar = 0;
//     Initialize static resources if needed
    if (!shader) shader = make_unique<Shader>(diffuse_vert_glsl, diffuse_frag_glsl);
    if (!texture) texture = make_unique<Texture>(image::loadBMP("tuneler/gold.bmp"));
    if (!mesh) mesh = make_unique<Mesh>("tuneler/progressBar.obj");
    ifstream x ("tuneler/pearl.mtl", std::ifstream::binary);
    tinyobj::LoadMtl(mat_map,mat, x);
}


bool Ammo::update(Scene &scene, float dt) {
    if (bar == 1 || bar == 2) {
        for (auto &obj : scene.objects) {
            // Ignore self in scene
            if (obj.get() == this)
                continue;

            // We only need to collide with asteroids, ignore other objects
            auto player1 = dynamic_cast<Player1 *>(obj.get());
            auto player2 = dynamic_cast<Player2 *>(obj.get());

            if (player1) {
                if (bar == 1) {
                    int progres = player1->get_shot_number();
                    if (progres > 10) progres = 10;
                    this->scale.x = help * (progres / 10.0f);
                }
            }
            if (player2) {
                if (bar == 2) {
                    int progres = player2->get_shot_number();
                    if (progres > 10) progres = 10;
                    scale.x = help * (progres / 10.0f);
                }
            }
        }
    }

    generateModelMatrix();
    return true;
}

void Ammo::set_bar(int x) {
    help = scale.x;
    bar = x;
}


void Ammo::render(Scene &scene) {

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

