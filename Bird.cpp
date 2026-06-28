#include "Bird.hpp"
#include <algorithm>

Bird::Bird(const sf::Texture& texture, sf::Vector2f startPos, float scale) : m_sprite(texture), m_startPos(startPos)
{
    const auto s = texture.getSize();
    m_sprite.setOrigin({static_cast<float>(s.x) * 0.5f, static_cast<float>(s.y) * 0.5f});
    m_sprite.setScale({scale, scale});
    m_sprite.setPosition(startPos);
    m_half = {static_cast<float>(s.x) * 0.5f * scale, static_cast<float>(s.y) * 0.5f * scale};
}

void Bird::reset(sf::Vector2f startPos)
{
    m_startPos = startPos;
    m_vy = 0.f;
    m_lastRot = 0.f;
    m_sprite.setRotation(sf::degrees(0.f));
    m_sprite.setPosition(startPos);
}

void Bird::flap(float impulse)
{
    m_vy = impulse;
}

void Bird::update(float dt, float gravity, float maxFallSpeed)
{
    m_vy += gravity * dt;
    if (m_vy > maxFallSpeed) m_vy = maxFallSpeed;
    m_sprite.move({0.f, m_vy * dt});
}

void Bird::setX(float x)
{
    auto p = m_sprite.getPosition();
    p.x = x;
    m_sprite.setPosition(p);
}

void Bird::setRotationFromVelocity(float degreesPerUnit, float minDeg, float maxDeg)
{
    const float target = std::clamp(m_vy * degreesPerUnit, minDeg, maxDeg);
    m_lastRot = m_lastRot + (target - m_lastRot) * 0.18f;
    m_sprite.setRotation(sf::degrees(m_lastRot));
}

sf::FloatRect Bird::hitbox() const
{
    const auto p = m_sprite.getPosition();
    const float w = m_half.x * 2.f;
    const float h = m_half.y * 2.f;
    const float insetX = w * 0.15f;
    const float insetY = h * 0.15f;
    sf::FloatRect b;
    b.position = {p.x - m_half.x + insetX, p.y - m_half.y + insetY};
    b.size = {w - insetX * 2.f, h - insetY * 2.f};
    return b;
}

sf::Vector2f Bird::position() const
{
    return m_sprite.getPosition();
}

float Bird::x() const
{
    return m_sprite.getPosition().x;
}

float Bird::y() const
{
    return m_sprite.getPosition().y;
}

float Bird::vy() const
{
    return m_vy;
}

void Bird::draw(sf::RenderTarget& target) const
{
    target.draw(m_sprite);
}

