#include <glm/gtc/random.hpp>
#include "scene.h"
#include "Explosion.h"

#include <shaders/texture_vert_glsl.h>
#include <shaders/texture_frag_glsl.h>

using namespace std;
using namespace glm;
using namespace ppgso;

// static resources
unique_ptr<Mesh> Explosion::mesh;
unique_ptr<Texture> Explosion::texture;
unique_ptr<Shader> Explosion::shader;
std::vector<tinyobj::material_t> Explosion::mat;
std::map<std::string, int> Explosion::mat_map;

Explosion::Explosion() {
    // Random rotation and momentum
    rotation = ballRand(PI) * 3.0f;
    rotMomentum = ballRand(PI) * 3.0f;
    speed = {0.0f, 0.0f, 0.0f};

    // Initialize static resources if needed
    if (!shader) shader = make_unique<Shader>(texture_vert_glsl, texture_frag_glsl);
    if (!texture) texture = make_unique<Texture>(image::loadBMP("tuneler/explosion.bmp"));
    if (!mesh) mesh = make_unique<Mesh>("tuneler/cube.obj");
    ifstream x ("tuneler/tank.mtl", std::ifstream::binary);
    tinyobj::LoadMtl(mat_map,mat, x);
}

void Explosion::render(Scene &scene) {
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

bool Explosion::update(Scene &scene, float dt) {
    // Update scale and rotation
    scale = scale * (1.0f + dt * 5.0f);
    rotation += rotMomentum * dt;
    position += speed * dt;

    // Die after reaching maxAge
    age += dt;
    if (age > maxAge) return false;

    generateModelMatrix();
    return true;
}
