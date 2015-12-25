#include "tank.h"
#include "globals.h"
#include "math.h"
#include "game_synchronizer.h"

extern Game_Synchronizer sync;

// sx is the x-coord of the bottom left corner of the tank
// sy is the y-coord of the same corner
// width is the width of the tank
// height is the height of the tank
Tank::Tank(int sx, int sy, int width, int height, int color)
{
    x = sx;
    y = sy;
    w = width;
    h = height;
    tank_color = color;
    barrel_theta = PI/4.0;
    barrel_length = w;
    wheel_rad = 2.0;
    draw();
}

// Return the minimum x-coord of your tank's bounding box.
int Tank::min_x(void)
{
    return x-(w/2)-1;
}

// Return the minimum y-coord of your tank's bounding box.
int Tank::min_y(void)
{
    return y;
}

// Return the maximum x-coord of your tank's bounding box.
int Tank::max_x(void)
{
    return x+w+(w/2);
}

// Return the maximum y-coord of your tank's bounding box.
int Tank::max_y(void)
{
    return y+h+22;
}

void Tank::barrel_end(int* bx, int* by)
{
    // Set the x and y coords of the end of the barrel.
    *bx = x + w/2.0 + (barrel_length+1)*cos(barrel_theta);
    *by = y+h+wheel_rad + (barrel_length+1)*sin(barrel_theta);
}

// Changing the position of the tank based on the input from the user
void Tank::reposition(int dx, int dy, float dtheta)
{
    // when the x-direction is increased
    if (dx > 0) {
        int pixel = sync.read_pixel(x+w+w/2+1, y+1);
        if(x <= 0) {
            x = 1;
        }
        //only execute if the tank is within the parameters of the screen and the rule
        if(sync.pixel_eq(pixel, SKY_COLOR) && x > 0 && x < 128) {
            sync.filled_rectangle(min_x(), min_y(), max_x(), max_y(), SKY_COLOR);
            x += dx;
            y += dy;
            barrel_theta += dtheta;
            draw();
        }
        // when the x-direction is decreased
    } else if (dx < 0) {
        int pixel = sync.read_pixel(x-w/2-1, y+1);
        if(x <= 0) {
            x = 1;
        }
        //only execute if the tank is within the parameters of the screen and the rule
        if(sync.pixel_eq(pixel, SKY_COLOR) && x > 0 && x < 128) {
            sync.filled_rectangle(min_x(), min_y(), max_x(), max_y(), SKY_COLOR);
            x += dx;
            y += dy;
            barrel_theta += dtheta;
            draw();
        }
        // change the barrel angle
    } else if (dtheta != 0) {
        barrel_theta += (dtheta*(PI/24));
        if (barrel_theta >= PI+(PI/4)) {
            barrel_theta = PI+(PI/4);
        }
        if (barrel_theta <= -PI/4) {
            barrel_theta = -PI/4;
        }
        sync.filled_rectangle(x-(w/2)-1,y,x+w+(w/2),max_y(), SKY_COLOR);
        draw();
        // increase the elevation
    } else if (dy < 0) {
        int pixel1 = sync.read_pixel(x-w/2-1, y-29);
        int pixel2 = sync.read_pixel(x-w/2-1, y-19);
        int pixel3 = sync.read_pixel(x-w/2-1, y-9);
        int pixel4 = sync.read_pixel(x+w+w/2+1, y-29);
        int pixel5 = sync.read_pixel(x+w+w/2+1, y-19);
        int pixel6 = sync.read_pixel(x+w+w/2+1, y-9);
        if(sync.pixel_eq(pixel1, SKY_COLOR) && sync.pixel_eq(pixel2, SKY_COLOR) && sync.pixel_eq(pixel3, SKY_COLOR) && sync.pixel_eq(pixel4, SKY_COLOR) && sync.pixel_eq(pixel5, SKY_COLOR) && sync.pixel_eq(pixel6, SKY_COLOR)) {
            sync.filled_rectangle(min_x(), min_y(), max_x(), max_y(), SKY_COLOR);
            y = 21;
            draw();
        }
        // decrease the elevation
    } else if (dy > 0) {
        sync.filled_rectangle(min_x(), min_y(), max_x(), max_y(), SKY_COLOR);
        y = 51;
        draw();
    }
}


// Drawing the tank
void Tank::draw()
{
    // the flag pole
    sync.line(x+1, y+h, x+1, y+h+20, BLACK);
    // the flag
    sync.filled_rectangle(x+1, y+h+12, x+7, y+h+20, tank_color);
    // barrel
    sync.line(x + w/2.0, y+h+wheel_rad, x + w/2.0 + barrel_length*cos(barrel_theta), y+h+wheel_rad + barrel_length*sin(barrel_theta), tank_color);
    // tank itself
    sync.filled_rectangle(x, y+wheel_rad, x+w, y+h+wheel_rad, tank_color);
    // left wheel
    sync.filled_circle(x+wheel_rad, y+wheel_rad, wheel_rad, tank_color);
    // center wheel
    sync.filled_circle(x+3*wheel_rad, y+wheel_rad, wheel_rad, BLACK);
    // right wheel
    sync.filled_circle(x+w-wheel_rad, y+wheel_rad, wheel_rad, tank_color);
}