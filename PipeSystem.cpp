#include "PipeSystem.hpp"
#include <algorithm>

PipeSystem::PipeSystem(const sf::Texture& texture, float pipeWidth)
{
    m_texture = &texture;
    m_pipeWidth = pipeWidth;
    m_texSize = texture.getSize();
}

static float clampf(float v, float lo, float hi)
{
    return std::max(lo, std::min(v, hi));
}

static float pipeSpeedForScore(int score)
{
    const float base = 300.f;
    const float inc = 5.0f;
    return base + static_cast<float>(score) * inc;
}

static float gapForScore(int score)
{
    const float base = 320.f;
    const float red = 1.8f;
    const float minGap = 200.f;
    return std::max(minGap, base - static_cast<float>(score) * red);
}

static float intervalForScore(int score)
{
    const float base = 1.70f;
    const float red = 0.01f;
    const float minInterval = 1.10f;
    return std::max(minInterval, base - static_cast<float>(score) * red);
}

static float extraRangeForScore(int score)
{
    const float base = 0.f;
    const float inc = 1.2f;
    return base + static_cast<float>(score) * inc;
}

float PipeSystem::scale() const
{
    return m_pipeWidth / static_cast<float>(m_texSize.x);
}

float PipeSystem::pipeW() const
{
    return static_cast<float>(m_texSize.x) * scale();
}

float PipeSystem::pipeH() const
{
    return static_cast<float>(m_texSize.y) * scale();
}

void PipeSystem::reset(float windowWidth, float groundY)
{
    const int poolCount = 7;
    m_pairs.assign(poolCount, Pair{});
    for (auto& p : m_pairs) p.active = false;
    m_spawnTimer = 0.f;
    for (int i = 0; i < 3; ++i) spawn(windowWidth + static_cast<float>(i) * 420.f, groundY, 0);
}

float PipeSystem::randomGapCenter(float groundY, float gap, float extraRange)
{
    const float margin = 70.f;
    const float top = margin + gap * 0.5f;
    const float bottom = (groundY - margin) - gap * 0.5f;
    const float mid = (top + bottom) * 0.5f;
    const float span = (bottom - top) * 0.5f;
    const float extra = std::min(span, extraRange);
    const float lo = clampf(mid - span - extra, top, bottom);
    const float hi = clampf(mid + span + extra, top, bottom);
    return std::uniform_real_distribution<float>(lo, hi)(m_rng);
}

void PipeSystem::spawn(float x, float groundY, int score)
{
    const float gap = gapForScore(score);
    const float extra = extraRangeForScore(score);
    for (auto& p : m_pairs)
    {
        if (!p.active)
        {
            p.active = true;
            p.x = x;
            p.gap = gap;
            p.gapCenterY = randomGapCenter(groundY, gap, extra);
            p.passed = false;
            return;
        }
    }
    auto it = std::min_element(m_pairs.begin(), m_pairs.end(), [](const Pair& a, const Pair& b) { return a.x < b.x; });
    it->active = true;
    it->x = x;
    it->gap = gap;
    it->gapCenterY = randomGapCenter(groundY, gap, extra);
    it->passed = false;
}

void PipeSystem::recycleOffscreen(float pipeW)
{
    const float killX = -pipeW - 20.f;
    for (auto& p : m_pairs)
    {
        if (p.active && (p.x + pipeW) < killX)
            p.active = false;
    }
}

void PipeSystem::update(float dt, float windowWidth, float groundY, int score)
{
    if (!m_texture) return;
    const float speed = pipeSpeedForScore(score);
    for (auto& p : m_pairs)
        if (p.active) p.x -= speed * dt;

    recycleOffscreen(pipeW());

    m_spawnTimer += dt;
    const float interval = intervalForScore(score);
    while (m_spawnTimer >= interval)
    {
        m_spawnTimer -= interval;
        spawn(windowWidth + pipeW() * 0.6f, groundY, score);
    }
}

static sf::FloatRect pipeRect(float x, float y, float w, float h)
{
    sf::FloatRect r;
    r.position = {x, y};
    r.size = {w, h};
    return r;
}

bool PipeSystem::collides(const sf::FloatRect& bird, float groundY) const
{
    if (!m_texture) return false;
    const float w = pipeW();

    for (const auto& p : m_pairs)
    {
        if (!p.active) continue;

        const float gapTop = p.gapCenterY - p.gap * 0.5f;
        const float gapBottom = p.gapCenterY + p.gap * 0.5f;

        sf::FloatRect topPipe = pipeRect(p.x, 0.f, w, gapTop);
        sf::FloatRect botPipe = pipeRect(p.x, gapBottom, w, groundY - gapBottom);

        if (bird.findIntersection(topPipe) || bird.findIntersection(botPipe))
        {
            return true;
        }
    }
    return false;
}

void PipeSystem::drawDebug(sf::RenderTarget& target, float groundY) const
{
    if (!m_texture) return;
    const float w = pipeW();
    const sf::Color redFill(255, 0, 0, 80);
    const sf::Color redOutline(255, 0, 0, 255);
    const sf::Color greenFill(0, 255, 0, 60);
    const sf::Color greenOutline(0, 255, 0, 255);

    sf::RectangleShape topRect;
    sf::RectangleShape botRect;
    sf::RectangleShape gapRect;

    topRect.setFillColor(redFill);
    topRect.setOutlineThickness(2.f);
    topRect.setOutlineColor(redOutline);

    botRect.setFillColor(redFill);
    botRect.setOutlineThickness(2.f);
    botRect.setOutlineColor(redOutline);

    gapRect.setFillColor(greenFill);
    gapRect.setOutlineThickness(2.f);
    gapRect.setOutlineColor(greenOutline);

    for (const auto& p : m_pairs)
    {
        if (!p.active) continue;
        const float gapTop = p.gapCenterY - p.gap * 0.5f;
        const float gapBottom = p.gapCenterY + p.gap * 0.5f;

        topRect.setPosition({p.x, 0.f});
        topRect.setSize({w, gapTop});
        target.draw(topRect);

        botRect.setPosition({p.x, gapBottom});
        botRect.setSize({w, groundY - gapBottom});
        target.draw(botRect);

        gapRect.setPosition({p.x, gapTop});
        gapRect.setSize({w, gapBottom - gapTop});
        target.draw(gapRect);
    }
}

int PipeSystem::consumePassedScore(float birdX)
{
    const float midX = birdX;
    const float w = pipeW();
    int gained = 0;
    for (auto& p : m_pairs)
    {
        if (!p.active) continue;
        const float pipeMid = p.x + w * 0.5f;
        if (!p.passed && midX > pipeMid)
        {
            p.passed = true;
            ++gained;
        }
    }
    return gained;
}

void PipeSystem::draw(sf::RenderTarget& target) const
{
    if (!m_texture) return;
    sf::Sprite pipe(*m_texture);
    const float s = scale();
    
    for (const auto& p : m_pairs)
    {
        if (!p.active) continue;
        const float gapTop = p.gapCenterY - p.gap * 0.5f;
        const float gapBottom = p.gapCenterY + p.gap * 0.5f;

        pipe.setOrigin({0.f, 0.f});
        pipe.setPosition({p.x, gapBottom});
        pipe.setScale({s, s});
        target.draw(pipe);

        pipe.setOrigin({0.f, 0.f});
        pipe.setPosition({p.x, gapTop});
        pipe.setScale({s, -s});
        target.draw(pipe);
    }
}
