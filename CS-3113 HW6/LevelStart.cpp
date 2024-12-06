#include "LevelStart.h"
#include "Utility.h"

#define LEVEL_WIDTH 14
#define LEVEL_HEIGHT 5

constexpr char FISH_FILEPATH[] = "assets/fish.png",
MAP_TILESET_FILEPATH[] = "assets/tileset.png",
BGM_FILEPATH[] = "assets/bgm.mp3",
JUMP_SFX_FILEPATH[] = "assets/bounce.wav",
CRAB_FILEPATH[] = "assets/crab.png",
BOAT_FILEPATH[] = "assets/boat.png",
PLATFORM_FILEPATH[] = "assets/platformPack_tile027.png",
ENEMY_FILEPATH[] = "assets/soph.png",
WORD_FILEPATH[] = "assets/words.png";

unsigned int LEVEL_START_DATA[] =
{
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

LevelStart::~LevelStart()
{
    delete[] m_game_state.enemies;
    delete    m_game_state.player;
    delete    m_game_state.map;
    Mix_FreeChunk(m_game_state.shoot_sfx);
    Mix_FreeMusic(m_game_state.bgm);
}



void LevelStart::initialise() {

    m_game_state.next_scene_id = -1;

    GLuint map_texture_id = Utility::load_texture("assets/tileset.png");
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVEL_START_DATA, map_texture_id, 1.0f, 4, 1);


    // ————— PLAYER ————— //

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
        0.6f,                      // width
        0.7f,                       // height
        PLAYER
    );

    m_game_state.player->set_position(glm::vec3(3.0f, 0.0f, 0.0f));

    // Jumping
    m_game_state.player->set_jumping_power(5.0f);

    m_game_state.shoot_sfx = Mix_LoadWAV("assets/bounce.wav");
}



void LevelStart::update(float delta_time)
{
    //m_game_state.player->update(delta_time, m_game_state.player, m_game_state.enemies, ENEMY_COUNT, m_game_state.map);

    if (delta_time == -1) {
        m_game_state.next_scene_id = 1;
    }

}

void LevelStart::render(ShaderProgram* g_shader_program)
{
    GLuint g_font_texture_id = Utility::load_texture(WORD_FILEPATH);

    Utility::draw_text(g_shader_program, g_font_texture_id, "Arrow to walk, Space to jump; ", 0.3f, 0.02f,
        glm::vec3(0.5f, 2.0f, 0.0f));

    Utility::draw_text(g_shader_program, g_font_texture_id, "Kill the enemies to move on;", 0.3f, 0.02f,
        glm::vec3(1.0f, 0.0f, 0.0f));

    Utility::draw_text(g_shader_program, g_font_texture_id, "ENTER TO BEGIN", 0.4f, 0.02f,
        glm::vec3(2.0f, -2.5f, 0.0f));


}