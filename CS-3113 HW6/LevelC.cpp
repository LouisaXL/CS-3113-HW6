#include "LevelC.h"
#include "Utility.h"

#define LEVEL_WIDTH 12
#define LEVEL_HEIGHT 12

constexpr char FISH_FILEPATH[] = "assets/fish.png",
MAP_TILESET_FILEPATH[] = "assets/tileset.png",
BGM_FILEPATH[] = "assets/bgm.mp3",
JUMP_SFX_FILEPATH[] = "assets/bounce.wav",
CRAB_FILEPATH[] = "assets/crab.png",
BOAT_FILEPATH[] = "assets/boat.png",
PLATFORM_FILEPATH[] = "assets/platformPack_tile027.png",
BUBBLE_FILEPATH[] = "assets/bubble.png",
HOOK_FILEPATH[] = "assets/hook.png",
WORD_FILEPATH[] = "assets/words.png";

// BGM
constexpr int CD_QUAL_FREQ = 44100,  // CD quality
AUDIO_CHAN_AMT = 2,      // Stereo
AUDIO_BUFF_SIZE = 4096;

//constexpr char SPRITESHEET_FILEPATH[] = "assets/george_0.png",
//PLATFORM_FILEPATH[] = "assets/platformPack_tile027.png",
//ENEMY_FILEPATH[] = "assets/soph.png";

//constexpr char BGM_FILEPATH[] = "assets/bgm.mp3",
//                JUMP_SFX_FILEPATH[] = "assets/jump.mp3";
// constexpr int  LOOP_FOREVER = -1;  // -1 means loop forever in Mix_PlayMusic; 0 means play once and loop zero times

unsigned int LEVEL_3_DATA[] =
{
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3
};



LevelC::~LevelC()
{
    delete[] m_game_state.enemies;
    delete    m_game_state.player;
    delete    m_game_state.map;
    Mix_FreeChunk(m_game_state.jump_sfx);
    Mix_FreeMusic(m_game_state.bgm);
}



void LevelC::initialise()
{

    m_game_state.next_scene_id = -1;

    GLuint map_texture_id = Utility::load_texture("assets/tileset.png");
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVEL_3_DATA, map_texture_id, 1.0f, 4, 1);


    // ����� PLAYER ����� //

    GLuint player_texture_id = Utility::load_texture(FISH_FILEPATH);

    int player_walking_animation[4][4] =
    {
        { 1, 5, 9, 13 },  // for George to move to the left,
        { 3, 7, 11, 15 }, // for George to move to the right,
        { 2, 6, 10, 14 }, // for George to move upwards,
        { 0, 4, 8, 12 }   // for George to move downwards
    };

    glm::vec3 acceleration = glm::vec3(0.0f, -10.0f, 0.0f);

    m_game_state.player = new Entity(
        player_texture_id,         // texture id
        5.0f,                      // speed
        acceleration,              // acceleration
        5.0f,                      // jumping power
        player_walking_animation,  // animation index sets
        0.0f,                      // animation time
        4,                         // animation frame amount
        0,                         // current animation index
        4,                         // animation column amount
        4,                         // animation row amount
        0.7f,                      // width
        0.7f,                       // height
        PLAYER
    );

    m_game_state.player->set_position(glm::vec3(5.0f, -5.0f, 0.0f));

    // Jumping
    m_game_state.player->set_jumping_power(5.0f);



    // ����� NPC ����� //

    GLuint enemy_texture_id = Utility::load_texture(CRAB_FILEPATH);

    m_game_state.enemies = new Entity[ENEMY_COUNT];

    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_game_state.enemies[i] = Entity(enemy_texture_id, 0.7f, 0.8f, 0.8f, ENEMY, GUARD, IDLE);
    }


    m_game_state.enemies[0].set_position(glm::vec3(1.0f, -2.0f, 0.0f));
    m_game_state.enemies[0].set_movement(glm::vec3(0.0f));
    m_game_state.enemies[0].set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
    //m_game_state.enemies[0].set_scale(glm::vec3(2.0f, 2.0f, 0.0f));

    /**
     BGM and SFX
     */
    //Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

    //m_game_state.bgm = Mix_LoadMUS("assets/dooblydoo.mp3");
    //Mix_PlayMusic(m_game_state.bgm, -1);
    //Mix_VolumeMusic(0.0f);

        // ����� AUDIO SETUP ����� //
   // Start Audio
    Mix_OpenAudio(
        CD_QUAL_FREQ,        // the frequency to playback audio at (in Hz)
        MIX_DEFAULT_FORMAT,  // audio format
        AUDIO_CHAN_AMT,      // number of channels (1 is mono, 2 is stereo, etc).
        AUDIO_BUFF_SIZE      // audio buffer size in sample FRAMES (total samples divided by channel count)
    );

    // Similar to our custom function load_texture
    Mix_Music* g_music = Mix_LoadMUS("assets/bgm.mp3");

    // This will schedule the music object to begin mixing for playback.
// The first parameter is the pointer to the mp3 we loaded
// and second parameter is the number of times to loop.
    Mix_PlayMusic(g_music, -1);

    // Set the music to half volume
    // MIX_MAX_VOLUME is a pre-defined constant
    Mix_VolumeMusic(MIX_MAX_VOLUME / 2);

    m_game_state.jump_sfx = Mix_LoadWAV("assets/bounce.wav");


}


void LevelC::update(float delta_time)
{

    m_game_state.player->update(delta_time, m_game_state.player, m_game_state.enemies, ENEMY_COUNT, m_game_state.map);

    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_game_state.enemies[i].update(delta_time, m_game_state.player, NULL, NULL, m_game_state.map);
    }

    // TODO: check the order (done?)
    m_game_state.player->check_enemy_hit(m_game_state.enemies, ENEMY_COUNT);
    m_game_state.player->check_player_hit(m_game_state.enemies, ENEMY_COUNT);

    // TODO: CHANGE IS_HIT TO JUST COLLISION CHECKS
    if (m_game_state.player->get_is_hit() == true) {
        m_game_state.player->deactivate();
    }



    // CHANGE TO NEXT SCENE
    //if (m_game_state.enemies[0].get_is_active() == false && ENEMY_COUNT != 0) {
    //    // m_game_state.next_scene_id = 2;
    //    m_game_state.player->set_position(glm::vec3(5.0f, 2.0f, 0.0f)); // reset position
    //}
}


void LevelC::render(ShaderProgram* g_shader_program)
{
    GLuint g_font_texture_id = Utility::load_texture(WORD_FILEPATH);
    m_game_state.map->render(g_shader_program);
    m_game_state.player->render(g_shader_program);
    for (int i = 0; i < 1; i++)
        m_game_state.enemies[i].render(g_shader_program);

    if (m_game_state.player->get_is_active() == false) {
        Utility::draw_text(g_shader_program, g_font_texture_id, "WASTED", 0.5f, 0.05f,
            glm::vec3(9.0f, -1.0f, 0.0f));
    }
    // TODO: ADD THE BELOW TO THE LAST LEVEL
    // 
    if (m_game_state.enemies->get_is_active() == false && ENEMY_COUNT != 0) {
        Utility::draw_text(g_shader_program, g_font_texture_id, "WIN", 0.5f, 0.05f,
            glm::vec3(5.0f, 1.0f, 0.0f));
    }
}