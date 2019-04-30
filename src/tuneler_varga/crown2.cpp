
#include "scene.h"
#include "crown2.h"
#include "player2.h"

#include <shaders/diffuse_vert_glsl.h>
#include <shaders/diffuse_frag_glsl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>

using namespace std;
using namespace glm;
using namespace ppgso;

// shared resources
unique_ptr<Mesh> Crown2::mesh;
unique_ptr<Shader> Crown2::shader;
unique_ptr<Texture> Crown2::texture;
std::vector<tinyobj::material_t> Crown2::mat;
std::map<std::string, int> Crown2::mat_map;


Crown2::Crown2() {
    scale *= 0.15f;
    rotation.x += PI;
    position.z -= 0.5f;
    // Initialize static resources if needed
    if (!shader) shader = make_unique<Shader>(diffuse_vert_glsl, diffuse_frag_glsl);
    if (!texture) texture = make_unique<Texture>(image::loadBMP("tuneler/crown.bmp"));
    if (!mesh) mesh = make_unique<Mesh>("tuneler/crown.obj");
    ifstream x ("tuneler/white-plastic.mtl", std::ifstream::binary);
    tinyobj::LoadMtl(mat_map,mat, x);

}

bool Crown2::update(Scene &scene, float dt) {
    for (auto &obj : scene.objects) {
        auto player = dynamic_cast<Player2 *>(obj.get());
        if(player){
            rotation.y += dt;
            modelMatrix =
                    glm::translate(mat4(1.0f), position)
                    * glm::orientate4(rotation)
                    * glm::scale(mat4(1.0f), scale);
            modelMatrix = player->modelMatrix * modelMatrix;
        }
    }
    return true;
}

void Crown2::player(bool x) {
    player_number = x;
}

bool Crown2::get_player() {
    return player_number;
}

void Crown2::render(Scene &scene) {
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

