/**
* Author: Louisa Liu
* Assignment: Platformer
* Date due: 2023-11-29, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/


/*
FOR GRADER:
There is a bug where when you are killed by enemy, the enemy is registered as killed and the player descend from spawn unable to move.
If you still see this message tho that means it is not fixed.

*/


/*
NOTE FOR SELF:
- add in boat as gate to next level
- add in attack animation and choice to attack enemies
- add in backgrounds
- add in variety of platform

^ will see if I end up doing any of those tho lol
*/

#define GL_SILENCE_DEPRECATION
//#define STB_IMAGE_IMPLEMENTATION
//#define LOG(argument) std::cout << argument << '\n'
#define GL_GLEXT_PROTOTYPES 1
#define FIXED_TIMESTEP 0.0166666f
//#define PLATFORM_COUNT 10
//#define ENEMY_COUNT 0
//#define PLAYER_COUNT 1
#define LEVEL1_LEFT_EDGE 3.0f
#define LIVES 3




#ifdef _WINDOWS
#include <GL/glew.h>
#endif

//#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "SDL_mixer.h"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"
#include "cmath"
#include <ctime>
#include <vector>

#include "Entity.h"
#include "Map.h"
#include "Utility.h"
#include "Scene.h"
#include "LevelA.h"
#include "LevelB.h"
#include "LevelC.h"
#include "LevelStart.h"


// GAME STATE now in Scene.h //

enum AppStatus { RUNNING, TERMINATED };

// ����� CONSTANTS ����� //
constexpr int WINDOW_WIDTH = 640 * 2,
WINDOW_HEIGHT = 480 * 2;

constexpr float BG_RED = 0.1922f,
                BG_BLUE = 0.549f,
                BG_GREEN = 0.9059f,
                BG_OPACITY = 1.0f;

constexpr int   VIEWPORT_X = 0,
                VIEWPORT_Y = 0,
                VIEWPORT_WIDTH = WINDOW_WIDTH,
                VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
                F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

constexpr float MILLISECONDS_IN_SECOND = 1000.0;

// comment below when ensure working
constexpr int NUMBER_OF_TEXTURES = 1;
constexpr GLint LEVEL_OF_DETAIL = 0;
constexpr GLint TEXTURE_BORDER = 0;



//// BGM
constexpr int CD_QUAL_FREQ = 44100,  // CD quality
AUDIO_CHAN_AMT = 2,      // Stereo
AUDIO_BUFF_SIZE = 4096;
//
//// SFX
//constexpr int PLAY_ONCE = 0,
//NEXT_CHNL = -1,  // next available channel
//MUTE_VOL = 0,
//MILS_IN_SEC = 1000,
//ALL_SFX_CHN = -1;


// ����� GLOBAL VARIABLES ����� //
// ����� GLOBAL VARIABLES ����� //
Scene* g_current_scene;
LevelStart* g_level_start;
LevelA* g_level_a;
LevelB* g_level_b;
LevelC* g_level_c;

Scene* g_levels[4];

SDL_Window* g_display_window;

AppStatus g_app_status = RUNNING;
ShaderProgram g_shader_program;
glm::mat4 g_view_matrix, g_projection_matrix;

float g_previous_ticks = 0.0f;
float g_accumulator = 0.0f;

//bool is_hit = false;
//constexpr float PLATFORM_OFFSET = 5.0f;
//
//constexpr int FONTBANK_SIZE = 16;


// Audio
Mix_Music* g_music;
//Mix_Chunk* g_bouncing_sfx;


// ���� GENERAL FUNCTIONS ���� //

void initialise();
void process_input();
void update();
void render();
void shutdown();


void switch_to_scene(Scene* scene)
{
    g_current_scene = scene;
    g_current_scene->initialise();
    update();
}

void initialise()
{
    SDL_Init(SDL_INIT_VIDEO);

    g_display_window = SDL_CreateWindow("PLEASE WORK!",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);

    if (context == nullptr)
    {
        //LOG("ERROR: Could not create OpenGL context.\n");
        shutdown();
    }

#ifdef _WINDOWS
    glewInit();
#endif

    // ����� VIDEO SETUP ����� //

    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);

    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_shader_program.get_program_id());

    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);

    // ����� AUDIO SETUP ����� //
   // Start Audio
    Mix_OpenAudio(
        CD_QUAL_FREQ,        // the frequency to playback audio at (in Hz)
        MIX_DEFAULT_FORMAT,  // audio format
        AUDIO_CHAN_AMT,      // number of channels (1 is mono, 2 is stereo, etc).
        AUDIO_BUFF_SIZE      // audio buffer size in sample FRAMES (total samples divided by channel count)
    );

    // Similar to our custom function load_texture
    g_music = Mix_LoadMUS("assets/bgm.mp3");

    // This will schedule the music object to begin mixing for playback.
// The first parameter is the pointer to the mp3 we loaded
// and second parameter is the number of times to loop.
    Mix_PlayMusic(g_music, -1);

    // Set the music to half volume
    // MIX_MAX_VOLUME is a pre-defined constant
    Mix_VolumeMusic(MIX_MAX_VOLUME / 2);

    // ����� LEVELS SETUP ����� //
    g_level_start = new LevelStart();
    g_level_a = new LevelA();
    g_level_b = new LevelB();
    g_level_c = new LevelC();

    g_levels[0] = g_level_start;
    g_levels[1] = g_level_a;
    g_levels[2] = g_level_b;
    g_levels[3] = g_level_c;

    switch_to_scene(g_levels[0]);

    //g_current_scene->update();
    //update();

    // ����� BLENDING ����� //
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{
    g_current_scene->get_state().player->set_movement(glm::vec3(0.0f));

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type) {
            // End game
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            g_app_status = TERMINATED;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_q:
                // Quit the game with a keystroke
                g_app_status = TERMINATED;
                break;

            //case SDLK_SPACE:
            //    // Jump
            //    if (g_current_scene->get_state().player->get_collided_bottom())
            //    {
            //        g_current_scene->get_state().player->jump();
            //        Mix_PlayChannel(-1, g_current_scene->get_state().jump_sfx, 0);
            //    }
            //    break;

            case SDLK_w:
                g_current_scene->get_state().player->shoot_up();
                Mix_PlayChannel(-1, g_current_scene->get_state().jump_sfx, 0);
                break;

            case SDLK_s:
                g_current_scene->get_state().player->shoot_down();
                Mix_PlayChannel(-1, g_current_scene->get_state().jump_sfx, 0);
                break;

            case SDLK_a:
                g_current_scene->get_state().player->shoot_left();
                Mix_PlayChannel(-1, g_current_scene->get_state().jump_sfx, 0);
                break;

            case SDLK_d:
                g_current_scene->get_state().player->shoot_right();
                Mix_PlayChannel(-1, g_current_scene->get_state().jump_sfx, 0);
                break;

            //case SDLK_m:
            //    // Mute volume
            //    Mix_HaltMusic();
            //    break;

            case SDLK_RETURN:
                if (g_current_scene == g_levels[0]) {
                    // increase after checking current level
                    g_current_scene->update(-1);
                }

                break;

            default:
                break;
            }

        default:
            break;
        }
    }

    // ����� KEY HOLD ����� //
    const Uint8* key_state = SDL_GetKeyboardState(NULL);

    if (key_state[SDL_SCANCODE_LEFT])        g_current_scene->get_state().player->move_left();
    else if (key_state[SDL_SCANCODE_RIGHT])  g_current_scene->get_state().player->move_right();
    else if (key_state[SDL_SCANCODE_UP])     g_current_scene->get_state().player->move_up();
    else if (key_state[SDL_SCANCODE_DOWN])   g_current_scene->get_state().player->move_down();

    if (glm::length(g_current_scene->get_state().player->get_movement()) > 1.0f)
        g_current_scene->get_state().player->normalise_movement();
}


void update()
{
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;

    delta_time += g_accumulator;

    if (delta_time < FIXED_TIMESTEP)
    {
        g_accumulator = delta_time;
        return;
    }




    while (delta_time >= FIXED_TIMESTEP) {
        // ����� UPDATING THE SCENE (i.e. map, character, enemies...) ����� //
        g_current_scene->update(FIXED_TIMESTEP);

        delta_time -= FIXED_TIMESTEP;
    }

    g_accumulator = delta_time;



    // Enemy die when fall
    //for (int i = 0; i < ENEMY_COUNT; i++) {
    //    if (g_game_state.enemies[i].get_position().y <= -5.0) {
    //        g_game_state.player->inc_enemy_slay();
    //        g_game_state.enemies[i].deactivate();
    //    }
    //}



    // ����� PLAYER CAMERA ����� //
//     // Prevent the camera from showing anything outside of the "edge" of the level
    g_view_matrix = glm::mat4(1.0f);

    //if (g_current_scene->get_state().player->get_position().x > LEVEL1_LEFT_EDGE) {
    g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-g_current_scene->get_state().player->get_position().x, -g_current_scene->get_state().player->get_position().y, 0));
    //}
    ////else {
    //    g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-5, 1.0, 0));
    //}

   
    

    //if (g_current_scene == g_level_a && g_current_scene->get_state().player->get_enemy_hit() == true) {
    //    switch_to_scene(g_level_b);
    //    g_current_scene->get_state().player->set_enemy_hit_false();
    //}
    

    //g_view_matrix = glm::mat4(1.0f);

    //if (g_current_scene->get_state().player->get_position().x > LEVEL1_LEFT_EDGE) {
    //    g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-g_current_scene->get_state().player->get_position().x, 1.0, 0));
    //}
    //else {
    //    g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-5, 1.0, 0));
    //}

}

void render()
{
    g_shader_program.set_view_matrix(g_view_matrix);

    glClear(GL_COLOR_BUFFER_BIT);

    // ����� RENDERING THE SCENE (i.e. map, character, enemies...) ����� //
    g_current_scene->render(&g_shader_program);

    //for (int i = 0; i < ENEMY_COUNT; i++) {
    //    g_game_state.enemies[i].render(&g_shader_program);
    //}




    SDL_GL_SwapWindow(g_display_window);
}

void shutdown()
{
    SDL_Quit();

    // ����� DELETING LEVEL DATA (i.e. map, character, enemies...) ����� //
    delete g_level_start;
    delete g_level_a;
    //delete g_level_b;
}

// ����� GAME LOOP ����� //
int main(int argc, char* argv[])
{
    initialise();

    while (g_app_status == RUNNING)
    {
        process_input();
        update();
        if (g_current_scene->get_state().next_scene_id >= 0) {
            switch_to_scene(g_levels[g_current_scene->get_state().next_scene_id]);
        

        }
        render();
    }

    shutdown();
    return 0;
}