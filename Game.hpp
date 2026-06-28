#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <optional>
#include "Bird.hpp"
#include "HighScore.hpp"
#include "PipeSystem.hpp"

class Game
{
public:
    Game();
    int run();

private:
    enum class State
    {
        Running,
        GameOver
    };

    void rebuildLayout();
    void resetRun();
    void pollEvents();
    void fixedUpdate(float dt);
    void draw();

    void onFlap();
    void onRestart();

    sf::RenderWindow m_window;
    sf::VideoMode m_mode;

    sf::Texture m_birdTex;
    sf::Texture m_pipeTex;

    sf::Font m_font;
    bool m_hasFont{};

    std::optional<Bird> m_bird;
    PipeSystem m_pipes;
    HighScore m_highScore{"highscore.txt"};

    State m_state{State::Running};
    int m_score{};

    float m_groundY{};
    float m_birdX{};
    float m_pipeWidth{};

    float m_gravity{2400.f};
    float m_flapImpulse{-650.f};
    float m_maxFallSpeed{1250.f};
    float m_fixedDt{1.f / 120.f};

    std::optional<sf::Text> m_textScore;
    std::optional<sf::Text> m_textHigh;
    std::optional<sf::Text> m_textTitle;
    std::optional<sf::Text> m_textHint;

    sf::VertexArray m_bg;
    sf::RectangleShape m_ground;
    bool m_debugMode{false};

    sf::Music m_backgroundMusic;
    
    sf::SoundBuffer m_flapBuffer;
    sf::SoundBuffer m_scoreBuffer;
    sf::SoundBuffer m_hitBuffer;
    sf::SoundBuffer m_gameOverBuffer;
};

