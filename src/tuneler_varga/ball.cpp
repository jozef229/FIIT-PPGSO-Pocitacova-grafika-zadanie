
#include "scene.h"
#include "ball.h"

#include <shaders/diffuse_vert_glsl.h>
#include <shaders/diffuse_frag_glsl.h>
#include <iostream>

using namespace std;
using namespace glm;
using namespace ppgso;

// shared resources
unique_ptr<Mesh> Ball::mesh;
unique_ptr<Shader> Ball::shader;
unique_ptr<Texture> Ball::texture;
std::vector<tinyobj::material_t> Ball::mat;
std::map<std::string, int> Ball::mat_map;


Ball::Ball() {
//    scale *= 0.1f;
    // Initialize static resources if needed
    if (!shader) shader = make_unique<Shader>(diffuse_vert_glsl, diffuse_frag_glsl);
    if (!texture) texture = make_unique<Texture>(image::loadBMP("tuneler/blue.bmp"));
    if (!mesh) mesh = make_unique<Mesh>("ball2.obj");
    ifstream x ("tuneler/emerald.mtl", std::ifstream::binary);
    tinyobj::LoadMtl(mat_map,mat, x);

}


bool Ball::update(Scene &scene, float dt) {



    if (scene.keyboard[GLFW_KEY_F]) {
        scene.lightDirection.x += 0.04f;
    }
    if (scene.keyboard[GLFW_KEY_T]) {
        scene.lightDirection.y += 0.04f;
    }
    if (scene.keyboard[GLFW_KEY_C]) {
        scene.lightDirection.z += 0.04f;
    }
    if (scene.keyboard[GLFW_KEY_H]) {
        scene.lightDirection.x -= 0.04f;
    }
    if (scene.keyboard[GLFW_KEY_B]) {
        scene.lightDirection.y -= 0.04f;
    }
    if (scene.keyboard[GLFW_KEY_V]) {
        scene.lightDirection.z -= 0.04f;
    }

    generateModelMatrix();
    return true;
}

void Ball::player(bool x) {
    player_number = x;
}


void Ball::render(Scene &scene) {
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

