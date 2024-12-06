// Scene.h
#pragma once
#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include "SDL_mixer.h"
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#include "Utility.h"
#include "Entity.h"
#include "Map.h"

/**
    Notice that the game's state is now part of the Scene class, not the main file.
*/
struct GameState
{
    // ————— GAME OBJECTS ————— //
    Map* map;
    Entity* player;
    Entity* enemies;
    Entity* bubble;
    Entity* hook;

    // ————— AUDIO ————— //
    Mix_Music* bgm;
    Mix_Chunk* jump_sfx;

    // ————— POINTERS TO OTHER SCENES ————— //
    int next_scene_id;
};


class Scene {
protected:
    GameState m_game_state;

public:
    // ————— ATTRIBUTES ————— //
    //int m_number_of_enemies = 2;    // never used
    //int m_number_of_lives = 3;

    // ————— METHODS ————— //
    virtual void initialise() = 0;
    virtual void update(float delta_time) = 0;
    virtual void render(ShaderProgram* program) = 0;

    //void const decrease_number_of_lives() { m_number_of_lives--; }

    void inc_scene() {
        // increase screne id
        m_game_state.next_scene_id += 1;
        
    }

    void set_scene_id(int amt) {
        m_game_state.next_scene_id = amt;
    }

    // ————— GETTERS ————— //
    GameState const get_state() const { return m_game_state; }
    //int const get_number_of_enemies() const { return m_number_of_enemies; } // never used
    //int const get_number_of_lives() const { return m_number_of_lives; }
};