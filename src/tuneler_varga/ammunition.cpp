#include <glm/gtc/random.hpp>
#include "ammunition.h"
#include "projectile.h"
#include "Explosion.h"

#include <shaders/diffuse_vert_glsl.h>
#include <shaders/diffuse_frag_glsl.h>

using namespace std;
using namespace glm;
using namespace ppgso;

// Static resources
unique_ptr<Mesh> Ammunition::mesh;
unique_ptr<Texture> Ammunition::texture;
unique_ptr<Shader> Ammunition::shader;
std::vector<tinyobj::material_t> Ammunition::mat;
std::map<std::string, int> Ammunition::mat_map;


Ammunition::Ammunition(int x, int y, int c) {
    scale *= 0.8f;

    if (!shader) shader = make_unique<Shader>(diffuse_vert_glsl, diffuse_frag_glsl);
    if (!texture) texture = make_unique<Texture>(image::loadBMP("tuneler/missile.bmp"));
    if (!mesh) mesh = make_unique<Mesh>("tuneler/missile.obj");
    ifstream xx ("tuneler/pearl.mtl", std::ifstream::binary);
    tinyobj::LoadMtl(mat_map,mat, xx);

    keyframeAnimation[0].duration = 150;
    keyframeAnimation[0].keyframePosition = {x - 0.1 + c, y + c, position.z};

    keyframeAnimation[1].duration = 150;
    keyframeAnimation[1].keyframeRotation = {1, 5, 1};
    keyframeAnimation[1].keyframePosition = {x + c, y - 0.1 + c, position.z};

    keyframeAnimation[2].duration = 150;
    keyframeAnimation[2].keyframeRotation = {1, 1, 5};
    keyframeAnimation[2].keyframePosition = {x + 0.1 + c, y + c, position.z};

    keyframeAnimation[3].duration = 150;
    keyframeAnimation[3].keyframeRotation = {5, 1, 1};
    keyframeAnimation[3].keyframePosition = {x + c, y + 0.1 + c, position.z};
}

bool Ammunition::update(Scene &scene, float dt) {

    Keyframe current = keyframeAnimation[processedKeyframes];
    Keyframe next = keyframeAnimation[(processedKeyframes + 1) % keyframeCount];
    float t = keyframeDuration / current.duration;
    position = lerp(current.keyframePosition, next.keyframePosition, t);
    rotation = lerp(current.keyframeRotation, next.keyframeRotation, t);
    keyframeDuration++;
    if (keyframeDuration >= current.duration) {
        keyframeDuration = 0;
        processedKeyframes = (processedKeyframes + 1) % keyframeCount;
    }

    if (amm == false) {
        return false;
    }
    generateModelMatrix();
    return true;
}

void Ammunition::set_full(bool x) {
    amm = x;
}


void Ammunition::render(Scene &scene) {
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



