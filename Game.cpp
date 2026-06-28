#include "Game.hpp"
#include <filesystem>
#include <optional>

static std::optional<std::filesystem::path> pickSystemFont()
{
    const std::filesystem::path fonts = "C:\\Windows\\Fonts";
    const std::filesystem::path a = fonts / "segoeui.ttf";
    const std::filesystem::path b = fonts / "arial.ttf";
    if (std::filesystem::exists(a)) return a;
    if (std::filesystem::exists(b)) return b;
    return std::nullopt;
}

static std::filesystem::path locateAssetsDir()
{
    namespace fs = std::filesystem;
    const fs::path c = fs::current_path();
    const fs::path a0 = c / "assets";
    const fs::path a1 = c.parent_path() / "assets";
    const fs::path a2 = c.parent_path().parent_path() / "assets";
    if (fs::exists(a0)) return a0;
    if (fs::exists(a1)) return a1;
    if (fs::exists(a2)) return a2;
    return a0;
}

Game::Game() : m_mode(sf::VideoMode::getDesktopMode()),
              m_window(m_mode, "Flappy Bird", sf::State::Fullscreen)
{
    m_window.setVerticalSyncEnabled(true);
    m_window.setKeyRepeatEnabled(false);

    const auto assets = locateAssetsDir();
    const auto sounds = assets / "sounds";
    (void)m_birdTex.loadFromFile(assets / "Bird.png");
    (void)m_pipeTex.loadFromFile(assets / "Pipes.png");
    if (m_backgroundMusic.openFromFile(sounds / "bgmusic.wav"))
    {
    m_backgroundMusic.setLooping(true);
    m_backgroundMusic.setVolume(35.f);
    m_backgroundMusic.play();
    }
    (void)m_flapBuffer.loadFromFile(sounds / "flap.mp3");
    (void)m_scoreBuffer.loadFromFile(sounds / "score.mp3");
    (void)m_hitBuffer.loadFromFile(sounds / "hit.mp3");
    (void)m_gameOverBuffer.loadFromFile(sounds / "gameover.mp3");
    if (const auto fontPath = pickSystemFont())
    {
        m_hasFont = m_font.openFromFile(*fontPath);
    }

    m_highScore.load();

    m_bg.setPrimitiveType(sf::PrimitiveType::TriangleStrip);
    m_bg.resize(4);

    m_ground.setFillColor(sf::Color(22, 24, 28));

    rebuildLayout();
    resetRun();
}

void Game::rebuildLayout()
{
    const auto size = m_window.getSize();
    const float w = static_cast<float>(size.x);
    const float h = static_cast<float>(size.y);

    m_groundY = h * 0.92f;
    m_birdX = w * 0.28f;
    m_pipeWidth = std::max(90.f, w * 0.065f);

    m_bg[0].position = {0.f, 0.f};
    m_bg[1].position = {w, 0.f};
    m_bg[2].position = {0.f, h};
    m_bg[3].position = {w, h};

    const sf::Color top(70, 160, 240);
    const sf::Color bottom(12, 25, 45);
    m_bg[0].color = top;
    m_bg[1].color = top;
    m_bg[2].color = bottom;
    m_bg[3].color = bottom;

    m_ground.setPosition({0.f, m_groundY});
    m_ground.setSize({w, h - m_groundY});

    if (m_hasFont)
    {
        m_textScore.emplace(m_font, "0", 54);
        m_textHigh.emplace(m_font, "", 24);
        m_textTitle.emplace(m_font, "", 96);
        m_textHint.emplace(m_font, "", 28);

        m_textScore->setFillColor(sf::Color::White);
        m_textHigh->setFillColor(sf::Color(235, 235, 235));
        m_textTitle->setFillColor(sf::Color::White);
        m_textHint->setFillColor(sf::Color(235, 235, 235));

        m_textScore->setPosition({24.f, 14.f});
        m_textHigh->setPosition({24.f, 74.f});
    }

    const auto bt = m_birdTex.getSize();
    const float desiredBirdH = std::max(54.f, h * 0.07f);
    const float birdScale = desiredBirdH / static_cast<float>(bt.y);

    m_bird.emplace(m_birdTex, sf::Vector2f{m_birdX, h * 0.45f}, birdScale);
    m_pipes = PipeSystem(m_pipeTex, m_pipeWidth);
    m_pipes.reset(w, m_groundY);
}

void Game::resetRun()
{
    const auto size = m_window.getSize();
    const float w = static_cast<float>(size.x);
    const float h = static_cast<float>(size.y);

    m_score = 0;
    if (m_bird) m_bird->reset({m_birdX, h * 0.45f});
    m_pipes.reset(w, m_groundY);

    if (m_hasFont)
    {
        if (m_textScore) m_textScore->setString("0");
        if (m_textHigh) m_textHigh->setString("High: " + std::to_string(m_highScore.value()));
    }
}

void Game::onFlap()
{
    static sf::Sound flapSound(m_flapBuffer);

    if (m_state == State::GameOver)
    {
        resetRun();
        m_state = State::Running;

        if (m_bird)
        {
            m_bird->flap(m_flapImpulse);
            flapSound.play();
        }
        return;
    }

    if (m_state == State::Running)
    {
        if (m_bird)
        {
            m_bird->flap(m_flapImpulse);
            flapSound.play();
        }
    }
}

void Game::onRestart()
{
    resetRun();
    m_state = State::Running;
}

void Game::pollEvents()
{
    while (const std::optional event = m_window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            m_window.close();
            break;
        }

        if (const auto* key = event->getIf<sf::Event::KeyPressed>())
        {
            if (key->code == sf::Keyboard::Key::Escape)
            {
                m_window.close();
            }
            else if (key->code == sf::Keyboard::Key::Space)
            {
                onFlap();
            }
            else if (key->code == sf::Keyboard::Key::R)
            {
                if (m_state == State::GameOver) onRestart();
            }
            else if (key->code == sf::Keyboard::Key::D)
            {
                m_debugMode = !m_debugMode;
            }
        }

        if (event->is<sf::Event::MouseButtonPressed>())
            onFlap();

        if (const auto* resized = event->getIf<sf::Event::Resized>())
        {
            sf::View v(sf::FloatRect({0.f, 0.f},
                                     {static_cast<float>(resized->size.x), static_cast<float>(resized->size.y)}));
            m_window.setView(v);
            rebuildLayout();
        }
    }
}

void Game::fixedUpdate(float dt)
{
    if (m_state != State::Running) return;
    if (!m_bird) return;

    const auto size = m_window.getSize();
    const float w = static_cast<float>(size.x);

    m_bird->update(dt, m_gravity, m_maxFallSpeed);
    m_bird->setX(m_birdX);
    m_bird->setRotationFromVelocity(0.06f, -28.f, 78.f);

    m_pipes.update(dt, w, m_groundY, m_score);

    int gained = m_pipes.consumePassedScore(m_birdX);

    if (gained > 0)
    {
        static sf::Sound scoreSound(m_scoreBuffer);
        scoreSound.play();
    }

    m_score += gained;

    if (m_textScore)
        m_textScore->setString(std::to_string(m_score));

    const auto hb = m_bird->hitbox();
    const bool hitGround = (hb.position.y + hb.size.y) > m_groundY;
    const bool hitCeil = hb.position.y < 0.f;
    const bool hitPipe = m_pipes.collides(hb, m_groundY);
    if (hitGround || hitPipe || hitCeil)
    {
        static sf::Sound hitSound(m_hitBuffer);
        static sf::Sound gameOverSound(m_gameOverBuffer);

        hitSound.play();
        gameOverSound.play();

        m_state = State::GameOver;
        m_highScore.saveIfHigher(m_score);

        if (m_textHigh)
             m_textHigh->setString("High: " + std::to_string(m_highScore.value()));
    }
}

void Game::draw()
{
    m_window.clear();
    m_window.draw(m_bg);
    m_pipes.draw(m_window);
    if (m_bird) m_bird->draw(m_window);
    m_window.draw(m_ground);

    if (m_debugMode)
    {
        m_pipes.drawDebug(m_window, m_groundY);
        if (m_bird)
        {
            const auto hb = m_bird->hitbox();
            sf::RectangleShape r;
            r.setPosition(hb.position);
            r.setSize(hb.size);
            r.setFillColor(sf::Color(255, 0, 0, 25));
            r.setOutlineThickness(2.f);
            r.setOutlineColor(sf::Color(255, 0, 0, 210));
            m_window.draw(r);
        }
    }

    if (m_hasFont && m_textScore && m_textHigh && m_textTitle && m_textHint)
    {
        m_window.draw(*m_textScore);
        m_window.draw(*m_textHigh);

        const auto size = m_window.getSize();
        const float w = static_cast<float>(size.x);
        const float h = static_cast<float>(size.y);

        if (m_state == State::GameOver)
        {
            m_textTitle->setString("GAME OVER");
            const auto tb2 = m_textTitle->getLocalBounds();
            m_textTitle->setOrigin({tb2.position.x + tb2.size.x * 0.5f, tb2.position.y + tb2.size.y * 0.5f});
            m_textTitle->setPosition({w * 0.5f, h * 0.30f});

            m_textHint->setString("Space/Click to restart   R to restart   Esc to quit");
            const auto hb4 = m_textHint->getLocalBounds();
            m_textHint->setOrigin({hb4.position.x + hb4.size.x * 0.5f, hb4.position.y + hb4.size.y * 0.5f});
            m_textHint->setPosition({w * 0.5f, h * 0.42f});

            m_window.draw(*m_textTitle);
            m_window.draw(*m_textHint);
        }
    }

    m_window.display();
}

int Game::run()
{
    sf::Clock clock;
    float acc = 0.f;
    while (m_window.isOpen())
    {
        pollEvents();

        float frame = clock.restart().asSeconds();
        if (frame > 0.05f) frame = 0.05f;
        acc += frame;

        while (acc >= m_fixedDt)
        {
            fixedUpdate(m_fixedDt);
            acc -= m_fixedDt;
        }

        draw();
    }
    return 0;
}
