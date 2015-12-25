// Author: Alizain Charania
// Date: 11/04/2015

#include "uLCD_4DGL.h"
#include "bullet.h"
#include "game_synchronizer.h"
#include "globals.h"
#include "math.h"
#include "playSound.h"
#include "wave_player.h"

extern Game_Synchronizer sync;
extern Serial pc;

// Initialize the bullet. Don't have to do much here.
// Keep a pointer to this bullet's tank.
// Set the speed, and default the bullet to not in_flight.
// I changed the constructor to add the boolean c
// It draws the bullet depending on this variable
Bullet::Bullet(Tank* t, bool c)
{
    tank = t;
    speed = 30;
    in_flight = false;
    cheat = c;
}

// If in_flight, do nothing. Otherwise,
// set the in_flight flag, and initialize values needed for
// the trajectory calculations. (x0, y0), (vx0, vy0), time
// Hint: tank->barrel_end(...) is useful here.
void Bullet::shoot(void)
{
    in_flight = true;
    time = 0;
    tank->barrel_end(&x0, &y0);
    vx0 = speed * cos(tank->barrel_theta);
    vy0 = speed * sin(tank->barrel_theta);
}

// If the bullet is in flight, calculate its new position
// after a time delta dt.
void Bullet::update_position(float dt)
{
    double g = 9.8;
    time += dt;
    x = (int) (x0 + vx0*time);
    y = (int) (y0 + vy0*time - 0.5*g*time*time);
}

// This is called in every loop execution
int Bullet::time_step(float dt)
{
    // dont do anything if the x, y values haven't changed significantly
    // preventing the bullet self collision
    if (in_flight) {
        int oldx = x;
        int oldy = y;
        // draw a more powerful bullet if cheat code activated
        if (cheat) {
            sync.triangle(x, y, x+2, y, x+1, y+2, SKY_COLOR);
        } else {
            sync.pixel(x,y,SKY_COLOR);
        }
        // update the bullet to the new position
        update_position(dt);
        // return BULLET_NO_COLLISION if x, y value not changed
        if(oldx == x && oldy == y) {
            return BULLET_NO_COLLISION;
        }
        // read the pixel value at the new position
        int pixel = sync.read_pixel(x, y);
        // bouncing the bullet from the wall edges
        if (x > 125 || x < 3 || y > 125 || y < 3) {
            vx0 = -0.95*vx0;
            x0 = x;
            y0 = y;
            time = 0;
            update_position(dt);
            if (cheat) {
                sync.triangle(x, y, x+2, y, x+1, y+2, DGREY);
            } else {
                sync.pixel(x,y,BLACK);
            }
            return BULLET_NO_COLLISION;
        }
        // keep the bullet moving in the sky
        if(sync.pixel_eq(pixel, SKY_COLOR) || sync.pixel_eq(pixel, DGREY)) {
            if (cheat) {
                sync.triangle(x, y, x+2, y, x+1, y+2, DGREY);
            } else {
                sync.pixel(x, y, BLACK);
            }
            return BULLET_NO_COLLISION;
            // else if there is a collsion
            // make the bomb explosion sound and return the pixel color it exploded
            // make a random explosion hole
        } else {
            pc.printf("pixel color, %i", pixel);
            char *bomb = "/sd/wavfiles/bomb.wav";
            playSound(bomb);
            sync.filled_rectangle(x, y, x+4, y+6, SKY_COLOR);
            sync.triangle(x-2,y, x+6, y, x+5, y+8, SKY_COLOR);
            sync.filled_circle(x+7, y, 2, SKY_COLOR);
            sync.filled_circle(x-2, y+9, 2, SKY_COLOR);
            sync.filled_circle(x+2, y+8, 2, SKY_COLOR);
            sync.filled_circle(x+1, y+2, 2, SKY_COLOR);
            in_flight = false;
            return pixel;
        }
    }
    return BULLET_NO_COLLISION;
}