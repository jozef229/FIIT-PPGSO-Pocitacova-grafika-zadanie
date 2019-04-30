#include <glm/gtc/random.hpp>
#include <ppgso/ppgso.h>

#include "generator.h"
#include "ammunition.h"
#include "base.h"
#include "rock.h"
#include "player1.h"
#include "player2.h"

using namespace std;
using namespace glm;
using namespace ppgso;

bool Generator::update(Scene &scene, float dt) {
    // Accumulate time
    time += dt;

    // Add object to scene when time reaches certain level
    if (time > 3.0) {
        int x = linearRand(2.0f, 14.0f);
        int y = linearRand(2.0f, 14.0f);
        bool help = true;
        for (auto &obj : scene.objects) {
            // We only need to collide with asteroids, ignore other objects
            auto rock = dynamic_cast<Rock *>(obj.get());
            auto ammunition = dynamic_cast<Ammunition *>(obj.get());
            auto base = dynamic_cast<Base *>(obj.get());
            auto player1 = dynamic_cast<Player1 *>(obj.get());
            auto player2 = dynamic_cast<Player2 *>(obj.get());
            if (ammunition && ammunition->position.x > x - 8.5 && ammunition->position.x < x - 7.5 &&
                ammunition->position.y > y - 8.5 && ammunition->position.y < y - 7.5)
                help = false;
            if (rock && rock->position.x == x - 8 && rock->position.y == y - 8)help = false;
            if (base && base->position.x == x - 8 && base->position.y == y - 8)help = false;
            if (player1 && player1->position.x == x - 8 && player1->position.y == y - 8)help = false;
            if (player2 && player2->position.x == x - 8 && player2->position.y == y - 8)help = false;
        }
        if (help == true) {
            auto ammu = make_unique<Ammunition>(x, y, -8);
            for (auto &obj : scene.objects) {
                // Ignore self in scene
                if (obj.get() == this)
                    continue;

                // We only need to collide with asteroids, ignore other objects
                auto ammunition = dynamic_cast<Ammunition *>(obj.get());
                if (ammunition) {
                    ammu->keyframeDuration = ammunition->keyframeDuration;
                    ammu->processedKeyframes = ammunition->processedKeyframes;
                }
            }
            scene.objects.push_back(move(ammu));
        }
        time = 0;
    }

    return true;
}

void Generator::render(Scene &scene) {
    // Generator will not be rendered
}
