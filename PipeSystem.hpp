#pragma once
#include <SFML/Graphics.hpp>
#include <random>
#include <vector>

class PipeSystem
{
public:
    PipeSystem() = default;
    PipeSystem(const sf::Texture& texture, float pipeWidth);

    void reset(float windowWidth, float groundY);

    void update(float dt, float windowWidth, float groundY, int score);
    void draw(sf::RenderTarget& target) const;

    bool collides(const sf::FloatRect& bird, float groundY) const;
    int consumePassedScore(float birdX);
    void drawDebug(sf::RenderTarget& target, float groundY) const;

private:
    struct Pair
    {
        bool active{};
        float x{};
        float gapCenterY{};
        float gap{};
        bool passed{};
    };

    void spawn(float windowWidth, float groundY, int score);
    float randomGapCenter(float groundY, float gap, float extraRange);
    void recycleOffscreen(float pipeW);
    float scale() const;
    float pipeW() const;
    float pipeH() const;

    const sf::Texture* m_texture{};
    float m_pipeWidth{};
    sf::Vector2u m_texSize{};
    std::vector<Pair> m_pairs;
    std::mt19937 m_rng{std::random_device{}()};
    float m_spawnTimer{};
};

