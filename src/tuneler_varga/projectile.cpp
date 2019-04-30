#include <glm/gtc/random.hpp>
#include "scene.h"
#include "projectile.h"
#include "Explosion.h"
#include "ground.h"
#include "block.h"

#include <shaders/diffuse_vert_glsl.h>
#include <shaders/diffuse_frag_glsl.h>

using namespace std;
using namespace glm;
using namespace ppgso;

// shared resources
unique_ptr<Mesh> Projectile::mesh;
unique_ptr<Shader> Projectile::shader;
unique_ptr<Texture> Projectile::texture;
std::vector<tinyobj::material_t> Projectile::mat;
std::map<std::string, int> Projectile::mat_map;


Projectile::Projectile() {
    // Set default speed
    scale *= 0.5f;
    speed = {0.0f, 0.0f, 0.0f};


    // Initialize static resources if needed
    if (!shader) shader = make_unique<Shader>(diffuse_vert_glsl, diffuse_frag_glsl);
    if (!texture) texture = make_unique<Texture>(image::loadBMP("tuneler/missile.bmp"));
    if (!mesh) mesh = make_unique<Mesh>("tuneler/missile.obj");
    ifstream x ("tuneler/white-rubber.mtl", std::ifstream::binary);
    tinyobj::LoadMtl(mat_map,mat, x);
}

void Projectile::smer(int x, bool hrac, bool rShot) {
    rotationShot = rShot;
    player_number = hrac;
    if (x == 0) {
        rotation.t = 0.0f;
        smerxy = {0.0f, realSpeed, -0.3f};
    } else if (x == 1) {
        rotation.t = PI;
        smerxy = {0.0f, -realSpeed, -0.3f};
    } else if (x == 2) {
        rotation.t = PI * 1.5;
        smerxy = {realSpeed, 0.0f, -0.3f};
    } else if (x == 3) {
        rotation.t = PI / 2;
        smerxy = {-realSpeed, 0.0f, -0.3f};
    }

}

bool Projectile::player() {
    return player_number;
}

bool Projectile::update(Scene &scene, float dt) {
    // Increase age
    age += dt;
    if (rotationShot == true) {


        // Accelerate
        speed += smerxy * dt;
        // Move the projectile




        speed.z += -(realSpeed * (float)sin(45) * age - (g * age * age)/2) * 2 * dt;
        position += speed * dt;


        if (position.z > 0.3f ) {
            auto explosion = make_unique<Explosion>();
            explosion->position = position;
            explosion->scale = scale * 0.5f;
            scene.objects.push_back(move(explosion));
            return false;
        }
    }
    else{
        // Accelerate
        speed += smerxy * dt;
        // Move the projectile
        position += speed * dt;
    }

    // Die after 5s
    if (age > 5.0f) {
        if (age > 5.5) {
            for (auto &obj : scene.objects) {
                // Ignore self in scene
                if (obj.get() == this)continue;
                auto camera = dynamic_cast<Camera *>(obj.get());
                camera->set_Posun(6);
            }
        }
        return false;
    }


    for (auto &obj : scene.objects) {

        // We only need to collide with asteroids, ignore other objects
        auto block = dynamic_cast<Block *>(obj.get());
        if (block) {
            if (distance(position, block->position) < (block->scale.y + scale.y)* 0.48f) {
                auto explosion = make_unique<Explosion>();
                explosion->position = position;
                explosion->scale = scale * 0.5f;
                scene.objects.push_back(move(explosion));
                return false;
            }
        }
    }


    generateModelMatrix();
    return true;
}

void Projectile::render(Scene &scene) {
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

void Projectile::destroy() {
    // This will destroy the projectile on Update
    age = 70.0f;
}
