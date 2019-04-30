// Example gl_scene
// - Introduces the concept of a dynamic scene of objects
// - Uses abstract object interface for Update and Render steps
// - Creates a simple game scene with Player, Asteroid and Space objects
// - Contains a generator object that does not render but adds Asteroids to the scene
// - Some objects use shared resources and all object deallocations are handled automatically
// - Controls: LEFT, RIGHT, "R" to reset, SPACE to fire

#include <iostream>
#include <map>
#include <list>

#include <ppgso/ppgso.h>

#include "camera.h"
#include "scene.h"
#include "generator.h"
#include "player1.h"
#include "player2.h"
#include "base.h"
#include "ball.h"
#include "win.h"
#include "ground.h"
#include "rock.h"
#include "crown1.h"
#include "ammo.h"
#include "trophy.h"
#include "ammunition.h"
#include "block.h"


using namespace std;
using namespace glm;
using namespace ppgso;

const unsigned int SIZE = 900;


/*!
 * Custom windows for our simple game
 */
class SceneWindow : public Window {
private:
    Scene scene;
    bool animate = true;

    /*!
     * Reset and initialize the game scene
     * Creating unique smart pointers to objects that are stored in the scene object list
     */
    void initScene() {
        scene.objects.clear();

        // Create a camera
        auto camera = make_unique<Camera>(40.0f, 1.0f, 0.1f, 100.0f);
        camera->position.z = -20.0f;
        scene.camera = move(camera);


        auto win = make_unique<Win>();
        if (scene.score2 == 2 || scene.score1 == 2) {
            if (scene.score1 == 2) {
                win->player_win(true);
            } else {
                win->player_win(false);
            }
            scene.objects.push_back(move(win));
        } else {
//       Add generator to scene
            auto ground = make_unique<Ground>();
            ground->position.y = 0;
            ground->position.x = 0;
            ground->position.z = 0.5;
            scene.objects.push_back(move(ground));

            // Add generator to scene
            auto generator = make_unique<Generator>();
            scene.objects.push_back(move(generator));

            // Add player to the scene
            auto player1 = make_unique<Player1>();
            player1->position.y = -5;
            player1->position.x = -5;
            scene.objects.push_back(move(player1));

            // Add player2 to the scene
            auto player2 = make_unique<Player2>();
            player2->position.y = 5;
            player2->position.x = 5;
            scene.objects.push_back(move(player2));

            auto base1 = make_unique<Base>();
            base1->position.y = -5;
            base1->position.x = -5;
            base1->player(true);
            scene.objects.push_back(move(base1));

            auto base2 = make_unique<Base>();
            base2->position.y = 5;
            base2->position.x = 5;
            base2->player(false);
            scene.objects.push_back(move(base2));

            auto trophy2 = make_unique<Trophy>();
            if (scene.score2 == 1) {
                trophy2->position.y = 5.5;
                trophy2->position.x = 8.5;
                scene.objects.push_back(move(trophy2));
            }

            auto trophy1 = make_unique<Trophy>();
            if (scene.score1 == 1) {
                trophy1->position.y = 5.5;
                trophy1->position.x = -8.5;
                scene.objects.push_back(move(trophy1));
            }


            auto ammo1 = make_unique<Ammo>();
            ammo1->position.y = -8;
            ammo1->position.x = -5;
            ammo1->set_bar(1);
            scene.objects.push_back(move(ammo1));


            auto ammo2 = make_unique<Ammo>();
            ammo2->position.y = -8;
            ammo2->position.x = 5;
            ammo2->set_bar(2);
            scene.objects.push_back(move(ammo2));

            auto ball = make_unique<Ball>();
            ball->position.y = -8;
            ball->position.x = 0;
            scene.objects.push_back(move(ball));

            for (int i = 1; i < 16; i++) {
                for (int j = 1; j < 16; j++) {
                    if (i >= 2 && i <= 4 && j >= 2 && j <= 4);
                    else if (i >= 12 && i <= 14 && j >= 12 && j <= 14);
                    else {
                        if (i == 1 || i == 15 || j == 1 || j == 15) {
                            auto obj = make_unique<Block>();
                            obj->position.x += (-8 + i);
                            obj->position.y += (-8 + j);
                            scene.objects.push_back(move(obj));
                        } else {
                            if (rand() % 100 > 92) {
                                auto obj = make_unique<Ammunition>(i, j, -8);
                                scene.objects.push_back(move(obj));
                            } else {
                                auto obj = make_unique<Rock>();
                                obj->position.x += (-8 + i);
                                obj->position.y += (-8 + j);
                                scene.objects.push_back(move(obj));
                            }
                        }
                    }
                }
            }
        }

    }

public:
    /*!
     * Construct custom game window
     */
    SceneWindow() : Window{"Tuneler - Varga", SIZE, SIZE} {
        //hideCursor();
        glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);

        // Initialize OpenGL state
        // Enable Z-buffer
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        // Enable polygon culling
        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CCW);
        glCullFace(GL_BACK);

        initScene();
    }

    /*!
     * Handles pressed key when the window is focused
     * @param key Key code of the key being pressed/released
     * @param scanCode Scan code of the key being pressed/released
     * @param action Action indicating the key state change
     * @param mods Additional modifiers to consider
     */
    void onKey(int key, int scanCode, int action, int mods) override {
        scene.keyboard[key] = action;

        // Reset
        if (key == GLFW_KEY_R && action == GLFW_PRESS) {
            scene.level = 1;
            scene.score1 = 0;
            scene.score2 = 0;
            scene.kill = false;
            initScene();
        }

        // Pause
        if (key == GLFW_KEY_P && action == GLFW_PRESS) {
            animate = !animate;
        }
        for (auto &obj : scene.objects) {

            auto camera = dynamic_cast<Camera *>(obj.get());

            if (key == GLFW_KEY_I && action == GLFW_PRESS)
                camera->set_Posun(0);
            else if (key == GLFW_KEY_K && action == GLFW_PRESS)
                camera->set_Posun(1);
            else if (key == GLFW_KEY_J && action == GLFW_PRESS)
                camera->set_Posun(2);
            else if (key == GLFW_KEY_L && action == GLFW_PRESS)
                camera->set_Posun(3);
            else if (key == GLFW_KEY_1 && action == GLFW_PRESS)
                camera->set_Posun(4);
            else if (key == GLFW_KEY_2 && action == GLFW_PRESS)
                camera->set_Posun(5);
            else if (key == GLFW_KEY_U && action == GLFW_PRESS)
                camera->set_Posun(7);
            else if (key == GLFW_KEY_O && action == GLFW_PRESS)
                camera->set_Posun(8);
        }
    }


    /*!
     * Window update implementation that will be called automatically from pollEvents
     */
    void onIdle() override {
        // Track time
        static auto time = (float) glfwGetTime();

        // Compute time delta
        float dt = animate ? (float) glfwGetTime() - time : 0;

        time = (float) glfwGetTime();

        if (scene.kill) {
            scene.kill = false;
            if (scene.score1 == 1)
                scene.win1 = true;
            if (scene.score2 == 1)
                scene.win2 = true;
            initScene();
        }



        // Set gray background
        glClearColor(0.0f, 0.0f, 0.0f, 0.0);
        // Clear depth and color buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Update and render all objects
        scene.update(dt);
        scene.render();
    }
};

int main() {
    // Initialize our window
    SceneWindow window;

    // Main execution loop
    while (window.pollEvents()) {}

    return EXIT_SUCCESS;
}
