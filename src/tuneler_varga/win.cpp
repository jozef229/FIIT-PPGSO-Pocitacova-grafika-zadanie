
#include "scene.h"
#include "win.h"

#include <shaders/diffuse_vert_glsl.h>
#include <shaders/diffuse_frag_glsl.h>

using namespace std;
using namespace glm;
using namespace ppgso;

// shared resources
unique_ptr<Mesh> Win::mesh;
unique_ptr<Shader> Win::shader;
unique_ptr<Texture> Win::texture;

std::vector<tinyobj::material_t> Win::mat;
std::map<std::string, int> Win::mat_map;


Win::Win() {
    scale *= 8.0f;
    rotation.y = 4.85f;
    // Initialize static resources if needed
    if (!shader) shader = make_unique<Shader>(diffuse_vert_glsl, diffuse_frag_glsl);
    if (!mesh) mesh = make_unique<Mesh>("tuneler/cube.obj");
    ifstream x ("tuneler/white-rubber.mtl", std::ifstream::binary);
    tinyobj::LoadMtl(mat_map,mat, x);
}


bool Win::update(Scene &scene, float dt) {
    generateModelMatrix();
    return true;
}

void Win::player_win(bool x) {
    if (x == true) {
         texture = make_unique<Texture>(image::loadBMP("tuneler/winl.bmp"));
    } else {
         texture = make_unique<Texture>(image::loadBMP("tuneler/winr.bmp"));
    }
}

void Win::render(Scene &scene) {
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

