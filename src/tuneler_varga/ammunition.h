#pragma once
#include <memory>

#include <ppgso/ppgso.h>

#include "scene.h"
#include "object.h"

/*!
 * Simple ammunition object
 * This sphere object represents an instance of mesh geometry
 * It initializes and loads all resources only once
 * It will move down along the Y axis and self delete when reaching below -10
 */

// Keyframe animation
struct Keyframe {
    glm::vec3 keyframePosition = {1, 1, 1}, keyframeRotation = {1, 1, 1};
    float duration = 100;
};

class Ammunition final : public Object {
private:
  // Static resources (Shared between instances)
  static std::unique_ptr<ppgso::Mesh> mesh;
  static std::unique_ptr<ppgso::Shader> shader;
  static std::unique_ptr<ppgso::Texture> texture;
  static std::vector<tinyobj::material_t> mat;
  static std::map<std::string, int> mat_map;
  bool amm = true;

  Keyframe keyframeAnimation[5];
  int keyframeCount = 3;

public:
    float keyframeDuration = 0;
    int processedKeyframes = 0;

  /*!
   * Create new ammunition
   */
  Ammunition(int x, int y, int c);
    void set_full(bool x);
  /*!
   * Update ammunition
   * @param scene Scene to interact with
   * @param dt Time delta for animation purposes
   * @return
   */
  bool update(Scene &scene, float dt) override;

  /*!
   * Render ammunition
   * @param scene Scene to render in
   */
  void render(Scene &scene) override;


private:
};

