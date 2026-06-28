#pragma once
#include <SFML/Graphics.hpp>

class Bird
{
public:
    Bird(const sf::Texture& texture, sf::Vector2f startPos, float scale);

    void reset(sf::Vector2f startPos);
    void flap(float impulse);
    void update(float dt, float gravity, float maxFallSpeed);

    void setX(float x);
    void setRotationFromVelocity(float degreesPerUnit, float minDeg, float maxDeg);

    sf::FloatRect hitbox() const;
    sf::Vector2f position() const;
    float x() const;
    float y() const;
    float vy() const;

    void draw(sf::RenderTarget& target) const;

private:
    sf::Sprite m_sprite;
    sf::Vector2f m_startPos{};
    sf::Vector2f m_half{};
    float m_vy{};
    float m_lastRot{};
};

