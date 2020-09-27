/*
  SkiJump.c
  
  Ski Jump GB
  A simple ski jumping game for the GameBoy
  https://github.com/periparus/Ski-Jump-GB
  
  Version: 1.0a
  Date: 27/09/2020
*/

// Dependencies
#include <gb/gb.h>
#include <gb/font.h>
#include <stdio.h>
#include <rand.h>

// Graphics
#include "Tiles.c"
#include "SkiMap1.c"
#include "Skier.c"
#include "EndScreen.c"
#include "SplashScreen_data.c"
#include "SplashScreen_map.c"

// Game Flow Variables
UBYTE gameRunning = 1;          // Game Loop 
UBYTE gameRamp = 0;             // flag to switch to ramp part of game
UBYTE gameAir = 0;              // flag to switch to airborn part of game
UBYTE gameLanded = 0;           // flag to switch to final part of game

// Game Variables
font_t gameFont;
UINT16 score = 0;               // Player Score
INT16 CPUScore = 0;             // CPU Score
INT16 xPos = 8;                 // Player x position
INT16 yPos = 16;                // Player y position
INT8 yVelocity = 0;             // Y axis Velocity
INT8 xVelocity = 0;             // X axis Velocity
const INT8 yGravity = 2;        // Gravity
const INT8 xGravity = -1;       // Wind Resistance
UBYTE addxGravity = 1;          // Counter that controls when wind resistance should be applied
UBYTE slideDownSlope = 1;       // Controls sliding fown the slope after landin
UINT8 seed;                     // Seed for PRNG

// Ramp variables, related to the ramp part of the game, before pitch off
const INT8 rampX[8] = {8, 8, 8, 8, 8, 6, 4, 9};     // X axis movement instructions for the ramp
const INT8 rampY[8] = {0, 8, 8, 8, 3, 0, 0, -9};    // Y axis movement instructions for the ramp
UINT8 rampCycle = 0;                                // Counter representing which part of the ramp 
                                                    // movement cycle we are in

// Heightmap constant to handle landing
const INT16 heightmap[20] = {8, 8, 12, 20, 28, 36, 36, 20, 36, 44, 52, 64, 68, 76, 82, 92, 100, 104, 104, 104};

// Game Functions
// Function for CPU efficient waiting
void efficient_wait(UINT8 loops)
{
    UINT8 i;
    for(i = 0; i < loops; i++)
    {
        wait_vbl_done();
    }
}

// Function to convert INT16 to tile in font tile map
// Used to convert numbers to their respective tiles for printing
char i_to_tile(INT16 x)
{
    switch (x)
    {
    case 0:
        return 0x02;
    case 1:
        return 0x03;
    case 2:
        return 0x04;
    case 3:
        return 0x05;
    case 4:
        return 0x06;
    case 5:
        return 0x07;
    case 6:
        return 0x08;
    case 7:
        return 0x09;
    case 8:
        return 0x0A;
    case 9:
        return 0x0B;
    default:
        return 0x00;
    }
}

// Function to clear screen
void clear_screen()
{
    set_win_tiles(0, 0, 20, 18, EmptyScreen);          
}

// Function to set game states to the desired states
void change_game_state(UBYTE stateRamp, UBYTE stateAir, UBYTE stateLand)
{
    // if Exiting Ramp state, reset ramp cycle to 0
    if(stateRamp == 0 && gameRamp == 1)
    {
        rampCycle = 0;
    }

    // if Exiting Air state, reset velocities
    if(stateAir == 0 && gameAir == 1)
    {
        xVelocity = 0;
        yVelocity = 0;
        addxGravity = 1;
    }

    // if Exiting Landed state, reset seed
    if(stateLand == 0 && gameLanded == 1)
    {
        seed = 0;
    }

    // Set game states
    gameRamp = stateRamp;
    gameAir = stateAir;
    gameLanded = stateLand;
}

// Function to update player position
void update_player_pos(INT8 x, INT8 y)
{
    xPos += x;
    yPos += y;
}

// Function to set player position
void set_player_pos(INT8 x, INT8 y)
{
    xPos = x;
    yPos = y;
}

// Function to apply gravity and wind resistance
void apply_gravity()
{
    yVelocity += yGravity;
    if(addxGravity)
    {
        xVelocity += xGravity;
        addxGravity = 0;
    } else
    {
        addxGravity++;
    }
    if (xVelocity < 0)
    {
        xVelocity = 0;
    }
}

// Function for smooth movement
void smooth_move(UINT8 spriteid, INT8 movex, INT8 movey)
{
    update_player_pos(movex, movey);
    // Check if player collides with slope
    if(yPos-16 > heightmap[xPos/8] && gameAir)
    {
        yPos = heightmap[xPos/8];
        switch (xPos/8)
        {
        case 7:
            yPos+=23;
            break;
        case 12:
            yPos += 1;
            break;
        case 14:
            yPos += 6;
            break;
        case 15:
            yPos += 5;
            break;
        case 16:
            yPos += 3;
            break;
        default:
            yPos +=4;
            break;
        }
        move_sprite(0, xPos, yPos);
        change_game_state(0, 0, 1);
        return;
    }

    while (movex != 0 || movey != 0)
    {
        if(movex != 0)
        {
            scroll_sprite(spriteid, movex < 0 ? -1 : 1, 0);
            movex += movex < 0 ? 1 : -1;
        }

        if(movey != 0)
        {
            scroll_sprite(spriteid, 0, movey < 0 ? -1 : 1);
            movey += movey < 0 ? 1 : -1;
        }
        wait_vbl_done();
    }
}

// Function to set up the game initially
void setup_game()
{
    // Load Font
    font_init();
    gameFont = font_load(font_min);
    font_set(gameFont);

    // Load Background
    set_bkg_data(37,25, SkiTiles); 
    set_bkg_tiles(0, 0, 20, 18, SkiMap1);


    // Load Player Sprite
    set_sprite_data(0, 4, Skier);
    set_sprite_tile(0, 0);

    // Move Sprite to starting position
    move_sprite(0, xPos, yPos);

    // Graphics setup
    SHOW_BKG;
    SHOW_SPRITES;
    DISPLAY_ON;
}

// Function that handles game restart
void restart_game()
{
    PlayerScoreText[9] = 0x00;
    PlayerScoreText[10] = 0x00;
    CPUScoreText[5] = 0x00;
    CPUScoreText[9] = 0x00;
    CPUScoreText[10] = 0x00;
    addxGravity = 1;
    yVelocity = 0;
    xVelocity = 0;
    slideDownSlope = 1;
    clear_screen();
    HIDE_WIN;
    set_sprite_tile(0, 0);
    set_player_pos(8,16);
    move_sprite(0, xPos, yPos);
    SHOW_SPRITES;
}

int main()  
{   
    // Sound Registers
    NR52_REG = 0x80; // Turn on Soynd
    NR50_REG = 0x77; // Set volume of L/R chanels
    NR51_REG = 0xFF; // Use all sound chanels

    // Splash screen
    set_bkg_data(0, 81, SplashScreenData);
    set_bkg_tiles(0, 0, 20, 18, SplashScreenMap);
    SHOW_BKG;
    DISPLAY_ON;
    waitpad(J_START);

    // Setup the game
    setup_game();

    // Game Loop
    while(gameRunning)
    {
        // Start game, push off
        if (joypad() & J_A)
        {
            change_game_state(1, 0, 0);
            xVelocity = 2;      // set initial velocity
            efficient_wait(10); // prevent long A press from making player jump immediately
            
            // Play push off sound
            NR41_REG = 0x03;
            NR42_REG = 0x81;
            NR43_REG = 0x02;
            NR44_REG = 0x80;
        }
        
        // Ramp part
        while(gameRamp)
        {
            if(rampCycle == 1)
            {
                set_sprite_tile(0, 1); // set sprite to "facing down tile"
            } else if (rampCycle == 4)
            {
                set_sprite_tile(0, 0); // set sprite to "crouching straight tile"
            } else if (rampCycle == 6)
            {
                // Changing from straight to upward facing sprite is rather jarring
                // so the final animation is split into two parts
                // this first part executes with the straight sprite,
                // then it advances the cycle to the second half and changes to 
                // the upward facing sprite
                smooth_move(0, rampX[rampCycle], rampY[rampCycle]);
                xVelocity--; // reduce player xvelocity, going up.
                rampCycle++;
                set_sprite_tile(0, 2); // set sprite to "going up tile"
            }

            smooth_move(0, rampX[rampCycle], rampY[rampCycle]);
            xVelocity++;
            rampCycle++;
            

            // Player chooses to pitch off
            if(joypad() & J_A)
            {
                // Play jump sound
                NR41_REG = 0x0B;
                NR42_REG = 0x91;
                NR43_REG = 0x30;
                NR44_REG = 0xC0;

                yVelocity = -12;
                set_sprite_tile(0, 2); // set sprite to "facing up tile"
                change_game_state(0, 1, 0);
                break;
            }

            // End of Ramp Reached
            if (rampCycle == 8)
            {
                xVelocity--; // reduce player xvelocity, going up.
                yVelocity = -6;
                change_game_state(0, 1, 0);
                break;
            }

            efficient_wait(2);
        }

        // In air
        while(gameAir)
        {
            smooth_move(0, xVelocity, yVelocity);
            apply_gravity();
            efficient_wait(1);
        }
        
        // Landed
        while(gameLanded)
        {
            // If the player landed within the top ramp area
            // the player landed on straight ground, so set sprite
            // to the "standing up tile" sprite and set score to 0
            if(xPos/8 < 8)
            {
                set_sprite_tile(0,3); // set sprite to "standing up tile"
                score = 0;
            }else if (slideDownSlope)
            {
                // Play landing sound
                NR41_REG = 0x28;
                NR42_REG = 0x81;
                NR43_REG = 0x79;
                NR44_REG = 0x80;
                set_sprite_tile(0, 1); // set sprite to "facing down tile"

                // Slide down the slope until you reach the floor level
                while(yPos < 112) 
                {
                    smooth_move(0, 4, 4);
                    efficient_wait(1);
                }

                // Ground reached, make sure we didn't go past it and stop
                // then calculate score
                set_sprite_tile(0,3); // set sprite to "standing up tile"
                move_sprite(0,xPos, 112);
                set_player_pos(xPos, 112);
                smooth_move(0,14,0);
                update_player_pos(14,0);
                slideDownSlope = 0;
                score = (UINT16)(xPos/8);
            }
            
            // Player pressed A, show scoreboard
            if(joypad() & J_A)
            {
                INT8 i = 1;
                HIDE_SPRITES;
                SHOW_WIN;

                // Show title of scoreboard and display player score
                set_win_tiles(6, 0, TitleWidth, SingleLine, TitleText);

                if (score > 9)
                {
                    PlayerScoreText[9] = i_to_tile(score/10);
                }
                
                PlayerScoreText[10] = i_to_tile(score%10);

                set_win_tiles(1, 2, PlayerScoreWidth, SingleLine, PlayerScoreText);
                efficient_wait(10);

                // Pseudo-RNG generation of CPU "opponent" scores
                while(i < 5)
                {
                    while(1)
                    {
                        if(seed >=255)
                        {
                            seed = 0;
                        } else
                        {
                            seed++;
                        }

                        // Player presses A to see the next CPU score, generate it
                        // then display it
                        if(joypad() & J_A)
                        {
                            initrand(seed);
                            CPUScore = rand()%17;
                            CPUScoreText[5] = i_to_tile(i);
                            if (CPUScore == 10)
                            {
                                CPUScoreText[9] = i_to_tile(1);
                            } else
                            {
                                CPUScoreText[9] = i_to_tile(CPUScore/10);
                            }
                            CPUScoreText[10] = i_to_tile(CPUScore%10);
                            set_win_tiles(1, 2 + 2*i, CPUScoreWidth, SingleLine, CPUScoreText);
                            i++;
                            seed = seed/2 + seed%7; // pseudorandomise seed to prevent 2 of the same result right after one another
                            efficient_wait(10);
                            break;
                        }
                    }
                }
                // Display remaining text
                set_win_tiles(4, 13, PressStartWidth, SingleLine, PressStartText);
                set_win_tiles(3, 14, PlayAgainWidth, SingleLine, PlayAgainText);

                // Game Restarts when player presses Start
                waitpad(J_START);
                restart_game();
                change_game_state(0, 0, 0);
            }
        }
    // End of main game loop
    }

    DISPLAY_OFF;
    return 0;
}