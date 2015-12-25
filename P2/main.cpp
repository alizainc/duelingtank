// Author: Alizain Charania
// Date: 11/04/2015

#include "mbed.h"

#include "SDFileSystem.h"
#include "wave_player.h"
#include "game_synchronizer.h"
#include "tank.h"
#include "bullet.h"
#include "globals.h"
#include "playSound.h"
#include "temperature.h"


DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);
DigitalOut led4(LED4);

DigitalOut latch(p16);
DigitalOut enable(p17);

SPI spi(p11, p12, p13);

DigitalIn pb_u(p22);                        // Up Button
DigitalIn pb_r(p21);                        // Right Button
DigitalIn pb_d(p23);                        // Down Button
DigitalIn pb_l(p14);                        // Left Button

Serial pc(USBTX, USBRX);                    // Serial connection to PC. Useful for debugging!
MMA8452 acc(p9, p10, 100000);              // Accelerometer (SDA, SCL, Baudrate)
uLCD_4DGL uLCD(p28,p27,p29);                 // LCD (tx, rx, reset)
SDFileSystem sd(p5, p6, p7, p8, "sd");      // SD  (mosi, miso, sck, cs)
AnalogOut DACout(p18);                      // speaker
wave_player player(&DACout);                // wav player
Game_Synchronizer sync(PLAYER1);            // Game_Synchronizer (PLAYER)
Timer frame_timer;                          // Timer
TMP36 myTMP36(p15);                         // Temperature sensor

// Global variables go here.

int winner = -1;
int whose_turn = PLAYER1;
int life1 = 3;
int life2 = 3;
bool cheat = false;
bool hard_mode = false;
bool life_change1 = true;
bool life_change2 = true;
int shootcounter1 = 0;
int shootcounter2 = 0;

// The following methods does the follwoing tasks
// Asks the user whether to run the game in Single- or Multi-Player mode.
// Asks the user whether to run easy or dufficult mode.
// Initializes the cheat code mode and the welcome screen

/**
 * displays the first screen of the game.
 * it is the welcome screen which asks the user
 * to press any key to continue
 */
void game_welcome()
{
    bool anykey = false;
    uLCD.background_color(WHITE);
    uLCD.cls();
    uLCD.color(BLUE);
    uLCD.textbackground_color(WHITE);
    uLCD.text_width(2);
    uLCD.text_height(2);
    uLCD.printf(" Dueling\n  Tank!\n");
    uLCD.printf("\nPress anykey");
    while (!anykey) {
        char *file3 = "/sd/wavfiles/trumpet_A3.wav";
        playSound(file3);
        if (!pb_r || !pb_l || !pb_u || !pb_d) {
            anykey = true;
        }
        char *file4 = "/sd/wavfiles/trumpet_Cs.wav";
        playSound(file4);
        if (!pb_r || !pb_l || !pb_u || !pb_d) {
            anykey = true;
        }
        char *file1 = "/sd/wavfiles/trumpet_E3.wav";
        playSound(file1);
        if (!pb_r || !pb_l || !pb_u || !pb_d) {
            anykey = true;
        }
        char *file2 = "/sd/wavfiles/trumpet_E4.wav";
        playSound(file2);
        if (!pb_r || !pb_l || !pb_u || !pb_d) {
            anykey = true;
        }
    }
}

/**
 * as the method name describes, this method asks the user
 * which play mode will he/she would like to play.
 * based on the user input, corresponding playing mode will be
 * executed
 * it also plays the entry trumpet music
 */
void game_hard_mode()
{
    uLCD.background_color(WHITE);
    uLCD.cls();
    uLCD.textbackground_color(WHITE);
    uLCD.color(BLACK);
    uLCD.printf("Select the Easy/\nDifficult mode:\n");
    uLCD.printf("\n\nEasy mode\n(right key)\nDifficult mode\n(left key)");
    int choose = 0;
    while(!choose) {
        char *file3 = "/sd/wavfiles/trumpet_A3.wav";
        playSound(file3);
        if (!pb_r) {
            choose = 1;
            hard_mode = false;
        } else if (!pb_l) {
            choose = 1;
            hard_mode = true;
        }
        char *file4 = "/sd/wavfiles/trumpet_Cs.wav";
        playSound(file4);
        if (!pb_r) {
            choose = 1;
            hard_mode = false;
        } else if (!pb_l) {
            choose = 1;
            hard_mode = true;
        }
        char *file1 = "/sd/wavfiles/trumpet_E3.wav";
        playSound(file1);
        if (!pb_r) {
            choose = 1;
            hard_mode = false;
        } else if (!pb_l) {
            choose = 1;
            hard_mode = true;
        }
        char *file2 = "/sd/wavfiles/trumpet_E4.wav";
        playSound(file2);
        if (!pb_r) {
            choose = 1;
            hard_mode = false;
        } else if (!pb_l) {
            choose = 1;
            hard_mode = true;
        }
    }
}

/**
 * initializing the cheat code mode
 * @return int     return the single/multi player mode
 * this methods gives player 1 additional cheating powers over player 2
 * such as inivisibility power and modified more powerful bulet.
 */
int cheat_code()
{
    cheat = true;
    uLCD.background_color(BLACK);
    uLCD.cls();
    uLCD.textbackground_color(BLACK);
    uLCD.color(WHITE);
    uLCD.printf("Welcome to the\nenchanced bullet\nand invisibility\ncheat-code\n");
    uLCD.printf("Your cheat code is activated\n");
    uLCD.printf("\nSelect the player mode:\nSingle player\n(right key)\nMulti-player\n(left key)");
    int choose = 0;
    while(!choose) {
        if (!pb_r) {
            choose = 1;
            return SINGLE_PLAYER;
        } else if (!pb_l) {
            choose = 1;
            return MULTI_PLAYER;
        }
    }
    return SINGLE_PLAYER;
}
/**
 * innitializing the game_menu
 * @return int     returns the single player or mutliplayer mode.
 * it also plays the entry music.
 * it also initializes the cheat code mode
 */
int game_menu(void)
{

    uLCD.baudrate(BAUD_3000000);

    // the locate command tells the screen where to place the text.
    uLCD.background_color(WHITE);
    uLCD.cls();
    uLCD.textbackground_color(WHITE);
    uLCD.color(BLACK);
    uLCD.printf("Welcome to Dueling Tanks!\n");
    uLCD.printf("\nSelect the player mode:\nSingle player\n(right key)\nMulti-player\n(left key)");
    int choose = 0;
    while(!choose) {
        char *file3 = "/sd/wavfiles/trumpet_A3.wav";
        playSound(file3);
        if (!pb_r) {
            choose = 1;
            return SINGLE_PLAYER;
        } else if (!pb_l) {
            choose = 1;
            return MULTI_PLAYER;
        } else if (!pb_u) {
            return cheat_code();
        }
        char *file4 = "/sd/wavfiles/trumpet_Cs.wav";
        playSound(file4);
        if (!pb_r) {
            choose = 1;
            return SINGLE_PLAYER;
        } else if (!pb_l) {
            choose = 1;
            return MULTI_PLAYER;
        } else if (!pb_u) {
            return cheat_code();
        }
        char *file1 = "/sd/wavfiles/trumpet_E3.wav";
        playSound(file1);
        if (!pb_r) {
            choose = 1;
            return SINGLE_PLAYER;
        } else if (!pb_l) {
            choose = 1;
            return MULTI_PLAYER;
        } else if (!pb_u) {
            return cheat_code();
        }
        char *file2 = "/sd/wavfiles/trumpet_E4.wav";
        playSound(file2);
        if (!pb_r) {
            choose = 1;
            return SINGLE_PLAYER;
        } else if (!pb_l) {
            choose = 1;
            return MULTI_PLAYER;
        } else if (!pb_u) {
            return cheat_code();
        }
    }
    //default return, will never be executed
    return SINGLE_PLAYER;
}

// Initialize the world map.
// The calls to sync.function() will run function()
// on both players' LCDs (assuming you are in multiplayer mode).
// In single player mode, only your lcd will be modified.
void map_init()
{

    // Fill the entire screen with sky blue.
    sync.background_color(SKY_COLOR);

    // Call the clear screen function to force the LCD to redraw the screen
    // with the new background color.
    sync.cls();

    sync.locate(10,13);
    sync.textbackground_color(SKY_COLOR);
    char l1[] = "Life 3";
    sync.puts(l1, sizeof(l1));

    sync.locate(0,13);
    sync.textbackground_color(SKY_COLOR);
    char l2[] = "Life 3";
    sync.puts(l2, sizeof(l2));

    // Draw the ground in green.
    sync.filled_rectangle(0,0,128,20, GND_COLOR);

    // Draw some obstacles.
    // Get creative here. You could use brown and grey to draw a mountain
    // or something cool like that.
    sync.filled_rectangle(59, 20, 69, 60, 0xFF3954);//drawing the center wall
    sync.triangle(55, 60, 74, 60, 64, 74, 0xFFFF56);//drawing the triangle on top of it
    if (hard_mode) {//these additional features are only implemented if we selected the hard_mode
        sync.filled_rectangle(55, 74, 74, 80, BLACK);
        //making different sizes of circles on top of each other
        sync.filled_circle(54, 25, 5, 0xFFFF00);
        sync.filled_circle(55, 34, 4, 0x25FFFF);
        sync.filled_circle(56, 41, 3, 0x41FF52);
        sync.filled_circle(57, 46, 2, 0xFF5428);
        sync.filled_circle(74, 25, 5, 0xFFFF00);
        sync.filled_circle(73, 34, 4, 0x25FFFF);
        sync.filled_circle(72, 41, 3, 0x41FF52);
        sync.filled_circle(71, 46, 2, 0xFF5428);
    }

    // Flush the draw buffer and execute all the accumulated draw commands.
    sync.update();
}

// Initializes the game hardware.
// Calls game_menu, welcome screen, hard_mode ad cheat code
// to find out which mode to play the game
// Initializes the game synchronizer.
void game_init(void)
{
    led1 = 0;
    led2 = 0;
    led3 = 0;
    led4 = 0;

    pb_u.mode(PullUp);
    pb_r.mode(PullUp);
    pb_d.mode(PullUp);
    pb_l.mode(PullUp);

    pc.printf("\033[2J\033[0;0H");              // Clear the terminal screen.

    game_welcome();
    game_hard_mode();
    int mode = game_menu();
    sync.init(&uLCD, &acc, &pb_u, &pb_r, &pb_d, &pb_l, mode); // Connect to the other player.
    map_init();
}

void RGB_LED(int red, int green, int blue) {
    unsigned int low_color=0;
    unsigned int high_color=0;
    high_color=(blue<<4)|((red&0x3C0)>>6);
    low_color=(((red&0x3F)<<10)|(green));
    spi.write(high_color);
    spi.write(low_color);
    latch=1;
    latch=0;
}

// Displays game over screen which lets us know who won.
// Plays a cool sound! (beethoven)
void game_over()
{
    sync.background_color(WHITE);
    sync.cls();
    sync.locate(2,14);
    char congo[] = "Congrats!";
    sync.puts(congo, sizeof(congo));

    sync.locate(2,12);
    if (winner == PLAYER1) {
        char win1[] = "Player 1 won";
        sync.puts(win1, sizeof(win1));
    } else if (winner == PLAYER2) {
        char win2[] = "Player 2 won";
        sync.puts(win2, sizeof(win2));
    }
    
    sync.locate(4,10);
    char stat[] = "Stats below:";
    sync.puts(stat, sizeof(stat));
    
    char s1[16];
    sprintf(s1, "Player1 %i shots", shootcounter1);
    sync.locate(1, 8);
    sync.puts(s1, sizeof(s1));
    
    char s2[16];
    sprintf(s2, "Player2 %i shots", shootcounter2);
    sync.locate(1, 6);
    sync.puts(s2, sizeof(s2));

    sync.locate(4,4);
    char over[] = "Game Over!";
    sync.puts(over, sizeof(over));
    sync.update();

    char *endmussic = "/sd/wavfiles/beethoven.wav";
    playSound(endmussic);
}

//the main method where the program begins
int main (void)
{
    //declaring the push buttons local varables
    int* p1_buttons;
    int* p2_buttons;

    //initializing the player to play1
    bool play1 = true;
    bool play2 = false;

    float ax1, ay1, az1;
    float ax2, ay2, az2;

    //starting the game by calling the game_initi()
    game_init();

    // Creates tanks.
    Tank t1(4, 21, 12, 8, TANK_RED);            // (min_x, min_y, width, height, color)
    Tank t2(111, 21, 12, 8, TANK_BLUE);         // (min_x, min_y, width, height, color)

    // For each tank, creates a bullet.
    Bullet b1(&t1, cheat);
    Bullet b2(&t2, cheat);

    // external hardware
    // displays the external temperature of player 1 on the screen
    frame_timer.start();
    float thermoRand = myTMP36.read();
    char tempe[6];
    sprintf(tempe, "%.2fC", thermoRand);
    sync.locate(10, 14);
    sync.puts(tempe, sizeof(tempe));

    while(true) {

        // Get a pointer to the buttons for both sides.

        //initializing the pushbuttons
        p1_buttons = sync.get_p1_buttons();
        p2_buttons = sync.get_p2_buttons();

        led1 = p1_buttons[0] ^ p2_buttons[0];
        led2 = p1_buttons[1] ^ p2_buttons[1];
        led3 = p1_buttons[2] ^ p2_buttons[2];
        led4 = p1_buttons[3] ^ p2_buttons[3];

        // since printf() is not implemented in the sync interface
        // this is the way I used to display the life of player 1
        if (life_change1) {
            if (life1 == 1) {
                sync.locate(0,13);
                sync.textbackground_color(SKY_COLOR);
                char lifeof1[] = "Life 1";
                sync.puts(lifeof1, sizeof(lifeof1));
            }
            if (life1 == 2) {
                sync.locate(0,13);
                sync.textbackground_color(SKY_COLOR);
                char lifeof1[] = "Life 2";
                sync.puts(lifeof1, sizeof(lifeof1));
            }
            if (life1 == 3) {
                sync.locate(0,13);
                sync.textbackground_color(SKY_COLOR);
                char lifeof1[] = "Life 3";
                sync.puts(lifeof1, sizeof(lifeof1));
            }
            life_change1 = false;
        }

        // since printf() is not implemented in the sync interface
        // this is the way I used to display the life of player 2
        if (life_change2) {
            if (life2 == 1) {
                sync.locate(10,13);
                sync.textbackground_color(SKY_COLOR);
                char lifeof1[] = "Life 1";
                sync.puts(lifeof1, sizeof(lifeof1));
            }
            if (life2 == 2) {
                sync.locate(10,13);
                sync.textbackground_color(SKY_COLOR);
                char lifeof1[] = "Life 2";
                sync.puts(lifeof1, sizeof(lifeof1));
            }
            if (life2 == 3) {
                sync.locate(10,13);
                sync.textbackground_color(SKY_COLOR);
                char lifeof1[] = "Life 3";
                sync.puts(lifeof1, sizeof(lifeof1));
            }
            life_change2 = false;
        }

        // Get the accelerometer values.
        sync.get_p1_accel_data(&ax1, &ay1, &az1);
        sync.get_p2_accel_data(&ax2, &ay2, &az2);

        // player 1's turn
        if(whose_turn == PLAYER1) {
            if (play1) {
                play1 = false;
                play2 = true;
                shootcounter1++;
                RGB_LED(0, 0, 40);
                sync.locate(0,14);
                sync.textbackground_color(SKY_COLOR);
                char playe1[] = "Player 1";
                sync.puts(playe1, sizeof(playe1));

            }
            // Accelerometer example
            if(ax1 >  ACC_THRESHOLD) {
                // Move the tank to the right if the accelerometer is tipped far enough to the right.
                t1.reposition(+1, 0, 0);
            }

            if(-ax1 > ACC_THRESHOLD) {
                // Move the tank to the left if the accelerometer is tipped far enough to the left.
                t1.reposition(-1, 0, 0);
            }

            if(p1_buttons[U_BUTTON]) {
                // Move the tank to the up if the up push button is pressed.
                t1.reposition(0, 10, 0);
            }

            if(ay1 >  ACC_THRESHOLD) {
                // Move the barrel to the front if the accelerometer is tipped far enough to the front.
                t1.reposition(0,0,1);
            }

            if(-ay1 >  ACC_THRESHOLD) {
                // Move the barrel to the back if the accelerometer is tipped far enough to the back.
                t1.reposition(0,0,-1);
            }

            if(p1_buttons[D_BUTTON]) {
                //shoot the bullet
                pc.printf("shoutcounter1 %i", shootcounter1);
                b1.shoot();
            }

            if(p1_buttons[R_BUTTON]) {
                // Move the tank to the bottom if the right push button is pressed.
                t1.reposition(0,-1,0);
            }

            if(p1_buttons[L_BUTTON]) {
                // give the inivibility power if the cheat_code is activated
                if (cheat)
                    sync.filled_rectangle(t1.min_x(), t1.min_y(), t1.max_x(), t1.max_y(), SKY_COLOR);
            }

            float dt = frame_timer.read();
            int intersection_code = b1.time_step(dt);
            if(intersection_code != BULLET_NO_COLLISION || intersection_code == BULLET_OFF_SCREEN) {
                whose_turn = PLAYER2;
            }

            // If you shot yourself, you lose your life.
            if(sync.pixel_eq(intersection_code, t1.tank_color)) {
                sync.update();
                whose_turn = PLAYER2;
                char *boo = "/sd/wavfiles/boo.wav";
                playSound(boo);
                life_change1 = true;
                --life1;
                //if life remaining is 0, you lose
                if (life1 == 0) {
                    winner = PLAYER2;
                    break;
                }
            }

            // If you shot the other guy, other guys loses life.
            if(sync.pixel_eq(intersection_code, t2.tank_color)) {
                sync.update();
                whose_turn = PLAYER2;
                char *clap = "/sd/wavfiles/applause.wav";
                playSound(clap);
                life_change2 = true;
                --life2;
                //if this life is 0, you win
                if (life2 == 0) {
                    winner = PLAYER1;
                    break;
                }
            }
        } else if(whose_turn == PLAYER2) {

            if (play2) {
                play2 = false;
                play1 = true;
                shootcounter2++;
                RGB_LED(0, 40, 0);
                sync.locate(0,14);
                sync.textbackground_color(SKY_COLOR);
                char playe2[] = "Player 2";
                sync.puts(playe2, sizeof(playe2));
            }

            //if single player use all p1 functions
            if (sync.play_mode == SINGLE_PLAYER) {
                if(ax1 >  ACC_THRESHOLD) {
                    // Move the tank to the right if the accelerometer is tipped far enough to the right.
                    t2.reposition(+1, 0, 0);
                }

                if(-ax1 > ACC_THRESHOLD) {
                    // Move the tank to the left if the accelerometer is tipped far enough to the left.
                    t2.reposition(-1, 0, 0);
                }

                if(p1_buttons[U_BUTTON]) {
                    // Move the tank to the up if the up push button is pressed.
                    t2.reposition(0, 10, 0);
                }

                if(ay1 >  ACC_THRESHOLD) {
                    // Move the barrel to the front if the accelerometer is tipped far enough to the front.
                    t2.reposition(0,0,1);
                }

                if(-ay1 >  ACC_THRESHOLD) {
                    // Move the barrel to the back if the accelerometer is tipped far enough to the back.
                    t2.reposition(0,0,-1);
                }

                if(p1_buttons[D_BUTTON]) {
                    //shoot the bullet
                    b2.shoot();
                }

                if(p1_buttons[R_BUTTON]) {
                    // Move the tank to the bottom if the right push button is pressed.
                    t2.reposition(0,-1,0);
                }

                if(p1_buttons[L_BUTTON]) {
                    //t2.reposition(100,0,0);
                }

                float dt = frame_timer.read();
                int intersection_code = b2.time_step(dt);

                if(intersection_code != BULLET_NO_COLLISION || intersection_code == BULLET_OFF_SCREEN) {
                    whose_turn = PLAYER1;
                }

                // If you shot the other guy, he loses this life!
                if(sync.pixel_eq(intersection_code, t1.tank_color)) {
                    sync.update();  // Is this necessary?
                    whose_turn = PLAYER1;
                    char *clap = "/sd/wavfiles/applause.wav";
                    playSound(clap);
                    life_change1 = true;
                    // if his life is 0, you win.
                    --life1;
                    if (life1 == 0) {
                        winner = PLAYER2;
                        break;
                    }
                }
                
                // If you shot yourself, you lose your life.
                if(sync.pixel_eq(intersection_code, t2.tank_color)) {
                    sync.update();
                    whose_turn = PLAYER1;
                    char *boo = "/sd/wavfiles/boo.wav";
                    playSound(boo);
                    life_change2 = true;
                    --life2;
                    // if your life is 0, he wins,
                    if (life2 == 0) {
                        winner = PLAYER1;
                        break;
                    }
                }

            } else if (sync.play_mode == MULTI_PLAYER) {
                if(ax2 >  ACC_THRESHOLD) {
                    // Move the tank to the right if the accelerometer is tipped far enough to the right.
                    t2.reposition(+1, 0, 0);
                }

                if(-ax2 > ACC_THRESHOLD) {
                    t2.reposition(-1, 0, 0);
                }

                if(p2_buttons[U_BUTTON]) {
                    t2.reposition(0, 10, 0);
                }

                if(ay2 >  ACC_THRESHOLD) {
                    // Move the barrel to the front if the accelerometer is tipped far enough to the front.
                    t2.reposition(0,0,1);
                }

                if(-ay2 >  ACC_THRESHOLD) {
                    // Move the barrel to the back if the accelerometer is tipped far enough to the back.
                    t2.reposition(0,0,-1);
                }

                // Button example
                if(p2_buttons[D_BUTTON]) {
                    //shoot the bullet
                    b2.shoot();
                }

                if(p2_buttons[R_BUTTON]) {
                    // Move the tank to the bottom if the right push button is pressed.
                    t2.reposition(0,-1,0);
                }

                if(p2_buttons[L_BUTTON]) {
                    //t2.reposition(100,0,0);
                }

                float dt = frame_timer.read();
                int intersection_code = b2.time_step(dt);

                if(intersection_code != BULLET_NO_COLLISION || intersection_code == BULLET_OFF_SCREEN) {
                    whose_turn = PLAYER1;
                }

                // If you shot the other guy, he loses this life!
                if(sync.pixel_eq(intersection_code, t1.tank_color)) {
                    sync.update();  // Is this necessary?
                    whose_turn = PLAYER1;
                    char *clap = "/sd/wavfiles/applause.wav";
                    playSound(clap);
                    life_change1 = true;
                    --life1;
                    // if his life is 0, you win.
                    if (life1 == 0) {
                        winner = PLAYER2;
                        break;
                    }
                }

                // If you shot yourself, you lose your life.
                if(sync.pixel_eq(intersection_code, t2.tank_color)) {
                    sync.update();
                    whose_turn = PLAYER1;
                    char *boo = "/sd/wavfiles/boo.wav";
                    playSound(boo);
                    life_change2 = true;
                    --life2;
                    // if your life is 0, he wins,
                    if (life2 == 0) {
                        winner = PLAYER1;
                        break;
                    }
                }
            }
        }

        frame_timer.reset();
        sync.update();
    }

    // call the game_over() method
    game_over();

}