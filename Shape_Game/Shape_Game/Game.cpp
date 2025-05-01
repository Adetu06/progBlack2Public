#include "Game.h"
#include <iostream>
#include <fstream>
#include <cmath>
#include <sstream>

Game::Game(const std::string& config) { init(config); }

void Game::init(const std::string& path)
{
    std::ifstream fin(path);
    std::string line;

    while (std::getline(fin, line))
    {
        std::istringstream iss(line);
        std::string type;
        iss >> type;

        if (type == "Window")
        {
            int w, h, fl;
            bool fs;
            iss >> w >> h >> fl >> fs;
            m_window.create(sf::VideoMode(w, h), "Game", fs ? sf::Style::Fullscreen : sf::Style::Default);
            m_window.setFramerateLimit(fl);
        }
        else if (type == "Font")
        {
            iss >> m_fontConfig.F >> m_fontConfig.S >> m_fontConfig.R >> m_fontConfig.G >> m_fontConfig.B;
        }
        else if (type == "Player")
        {
            iss >> m_playerConfig.SR  // 32
                >> m_playerConfig.CR  // 32
                >> m_playerConfig.S   // 5
                >> m_playerConfig.FR  // 5
                >> m_playerConfig.FG  // 5
                >> m_playerConfig.FB  // 5
                >> m_playerConfig.OR  // 255
                >> m_playerConfig.OG  // 0
                >> m_playerConfig.OB  // 0
                >> m_playerConfig.OT  // 4
                >> m_playerConfig.V;  // 8
        }
        else if (type == "Enemy")
        {
            iss >> m_enemyConfig.SR   // 32
                >> m_enemyConfig.CR    // 32
                >> m_enemyConfig.SMIN // 3
                >> m_enemyConfig.SMAX // 3
                >> m_enemyConfig.OR   // 255
                >> m_enemyConfig.OG   // 255
                >> m_enemyConfig.OB   // 255
                >> m_enemyConfig.OT   // 2
                >> m_enemyConfig.VMIN // 3
                >> m_enemyConfig.VMAX // 8
                >> m_enemyConfig.L    // 90
                >> m_enemyConfig.SI;  // 60
        }
        else if (type == "Bullet")
        {
            iss >> m_bulletConfig.SR  // 10
                >> m_bulletConfig.CR   // 10
                >> m_bulletConfig.S    // 20
                >> m_bulletConfig.FR  // 255
                >> m_bulletConfig.FG  // 255
                >> m_bulletConfig.FB  // 255
                >> m_bulletConfig.OR  // 255
                >> m_bulletConfig.OG  // 255
                >> m_bulletConfig.OB  // 255
                >> m_bulletConfig.OT  // 2
                >> m_bulletConfig.V   // 20
                >> m_bulletConfig.L; // 90
        }
    }

    spawnPlayer();
}

void Game::run()
{
    while (m_running)
    {
        m_entities.update();

        if (!m_paused)
        {
            sEnemySpawner();
            sMovement();
            sCollision();
            sLifespan();
            sUserInput();
        }

        sRender();
        m_currentFrame++;
    }
}

void Game::setPaused(bool paused) { m_paused = paused; }

void Game::spawnPlayer()
{
    auto entity = m_entities.addEntity("player");
    float mx = m_window.getSize().x / 2.0f;
    float my = m_window.getSize().y / 2.0f;

    entity->cTransform = std::make_shared<CTransform>(
        Vec2(mx, my),
        Vec2(0, 0),
        0.0f
    );

    entity->cShape = std::make_shared<CShape>(
        m_playerConfig.SR,
        m_playerConfig.V,
        sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB),
        sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB),
        m_playerConfig.OT
    );

    entity->cCollision = std::make_shared<CCollision>(m_playerConfig.CR);
    entity->cInput = std::make_shared<CInput>();
    m_player = entity;
}

void Game::spawnEnemy()
{
    auto entity = m_entities.addEntity("enemy");
    float radius = m_enemyConfig.CR;

    int windowWidth = static_cast<int>(m_window.getSize().x);
    int windowHeight = static_cast<int>(m_window.getSize().y);

    int rangeX = windowWidth - static_cast<int>(radius * 2);
    int rangeY = windowHeight - static_cast<int>(radius * 2);

    rangeX = std::max(1, rangeX);
    rangeY = std::max(1, rangeY);

    float ex = radius + (rand() % rangeX);
    float ey = radius + (rand() % rangeY);

    //random direction
    Vec2 direction = Vec2(
        (rand() % 200 - 100) / 100.0f,  // Random float between -1.0 and 1.0
        (rand() % 200 - 100) / 100.0f
    ).normalized();

    //random speed
    float speed = m_enemyConfig.SMIN +
        (static_cast<float>(rand()) / RAND_MAX) *
        (m_enemyConfig.SMAX - m_enemyConfig.SMIN);

    //configure transform with position, velocity, rotation
    entity->cTransform = std::make_shared<CTransform>(
        Vec2(ex, ey),          //position
        direction * speed,     //velocity
        0.0f                   //angle (initial rotation)
    );

    //random num of verticies
    int vertices = m_enemyConfig.VMIN + rand() % (m_enemyConfig.VMAX - m_enemyConfig.VMIN + 1);

    //random colour
    sf::Color fill(rand() % 256, rand() % 256, rand() % 256);

    //shape configuration
    entity->cShape = std::make_shared<CShape>(
        m_enemyConfig.SR,       //radius
        vertices,               //vertex num
        fill,                   //colour
        sf::Color(m_enemyConfig.OR, m_enemyConfig.OG, m_enemyConfig.OB), // Outline color
        m_enemyConfig.OT        //outline thickness
    );

    //collsion configuration
    entity->cCollision = std::make_shared<CCollision>(m_enemyConfig.CR);

    //score configuration (score not fixed yet)
    entity->cScore = std::make_shared<CScore>(vertices * 100);

    //update last spawn time
    m_lastEnemySpawnTime = m_currentFrame;
}

void Game::spawnSmallEnemies(std::shared_ptr<Entity> e)
{
    if (!e->isActive()) return;

    int vertices = e->cShape->circle.getPointCount();
    float radius = e->cShape->circle.getRadius() / 2.0f;
    sf::Color fill = e->cShape->circle.getFillColor();
    sf::Color outline = e->cShape->circle.getOutlineColor();
    float thickness = e->cShape->circle.getOutlineThickness();

    const float angleIncrement = 360.0f / vertices;

    for (int i = 0; i < vertices; ++i)
    {
        auto smallEnemy = m_entities.addEntity("smallEnemy");

        float angle = angleIncrement * i;
        Vec2 direction = Vec2(cos(angle * (3.14159f / 180.0f)), sin(angle * (3.14159f / 180.0f)));

        float speed = e->cTransform->velocity.length();
        smallEnemy->cTransform = std::make_shared<CTransform>(
            e->cTransform->pos,
            direction * speed,
            0.0f
        );

        smallEnemy->cShape = std::make_shared<CShape>(
            radius,
            vertices,
            fill,
            outline,
            thickness
        );

        //collision
        smallEnemy->cCollision = std::make_shared<CCollision>(radius);

        smallEnemy->cScore = std::make_shared<CScore>(e->cScore->score * 2);
        smallEnemy->cLifespan = std::make_shared<CLifespan>(60);
    }
}

void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vec2& target)
{
    auto bullet = m_entities.addEntity("bullet");
    Vec2 direction = (target - entity->cTransform->pos).normalized();

    bullet->cTransform = std::make_shared<CTransform>(
        entity->cTransform->pos,
        direction * m_bulletConfig.S,
        0.0f
    );

    bullet->cShape = std::make_shared<CShape>(
        m_bulletConfig.SR,
        m_bulletConfig.V,
        sf::Color(m_bulletConfig.FR, m_bulletConfig.FG, m_bulletConfig.FB),
        sf::Color(m_bulletConfig.OR, m_bulletConfig.OG, m_bulletConfig.OB),
        m_bulletConfig.OT
    );

    bullet->cCollision = std::make_shared<CCollision>(m_bulletConfig.CR);
    bullet->cLifespan = std::make_shared<CLifespan>(m_bulletConfig.L);
}

void Game::sMovement()
{
    //player movement
    Vec2 playerVelocity(0, 0);
    if (m_player->cInput->up) playerVelocity.y -= m_playerConfig.S;
    if (m_player->cInput->down) playerVelocity.y += m_playerConfig.S;
    if (m_player->cInput->left) playerVelocity.x -= m_playerConfig.S;
    if (m_player->cInput->right) playerVelocity.x += m_playerConfig.S;
    m_player->cTransform->velocity = playerVelocity;

    //update all entities
    for (auto& e : m_entities.getEntities())
    {
        if (!e->isActive()) continue;
        e->cTransform->pos += e->cTransform->velocity;

        //window bounds
        Vec2& pos = e->cTransform->pos;
        Vec2& vel = e->cTransform->velocity;
        float radius = e->cCollision->radius;

        if (pos.x < radius || pos.x > m_window.getSize().x - radius) vel.x *= -1;
        if (pos.y < radius || pos.y > m_window.getSize().y - radius) vel.y *= -1;
    }
}

void Game::sLifespan()
{
    for (auto& e : m_entities.getEntities())
    {
        if (!e->isActive() || !e->cLifespan) continue;

        if (e->cLifespan->remaining > 0)
        {
            e->cLifespan->remaining--;
            float ratio = static_cast<float>(e->cLifespan->remaining) / e->cLifespan->total;
            sf::Color color = e->cShape->circle.getFillColor();
            color.a = static_cast<sf::Uint8>(255 * ratio);
            e->cShape->circle.setFillColor(color);
        }
        else
        {
            e->destroy();
        }
    }
}

void Game::sCollision()
{
    for (auto& a : m_entities.getEntities())
    {
        if (!a->isActive()) continue;
        for (auto& b : m_entities.getEntities())
        {
            if (!b->isActive() || a == b) continue;

            Vec2 delta = a->cTransform->pos - b->cTransform->pos;
            float dist = delta.length();
            float minDist = a->cCollision->radius + b->cCollision->radius;

            if (dist < minDist)
            {
                //handle collision based on entity types
                if (a->tag() == "player" && b->tag() == "enemy")
                {
                    a->destroy();
                    spawnPlayer();
                }
                else if (a->tag() == "bullet" && b->tag() == "enemy")
                {
                    spawnSmallEnemies(b);
                    a->destroy();
                    b->destroy();
                    m_score += b->cScore->score;
                }
            }
        }
    }
}

void Game::sEnemySpawner()
{
    if ((m_currentFrame - m_lastEnemySpawnTime) > m_enemyConfig.SI)
    {
        spawnEnemy();
    }
}

void Game::sRender()
{
    m_window.clear();

    for (auto& e : m_entities.getEntities())
    {
        if (!e->isActive()) continue;

        e->cShape->circle.setPosition(e->cTransform->pos.x, e->cTransform->pos.y);
        e->cShape->circle.setRotation(e->cTransform->angle++);
        m_window.draw(e->cShape->circle);
    }

    //score display (needs fixing)
    sf::Text text;
    text.setFont(m_font);
    text.setString("Score: " + std::to_string(m_score));
    text.setCharacterSize(m_fontConfig.S);
    text.setFillColor(sf::Color(m_fontConfig.R, m_fontConfig.G, m_fontConfig.B));
    m_window.draw(text);

    m_window.display();
}

void Game::sUserInput()
{
    sf::Event event;
    while (m_window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed) m_running = false;

        //key press handling
        if (event.type == sf::Event::KeyPressed)
        {
            switch (event.key.code)
            {
            case sf::Keyboard::P: setPaused(!m_paused); break;
            case sf::Keyboard::W: m_player->cInput->up = true; break;
            case sf::Keyboard::S: m_player->cInput->down = true; break;
            case sf::Keyboard::A: m_player->cInput->left = true; break;
            case sf::Keyboard::D: m_player->cInput->right = true; break;
            case sf::Keyboard::Escape: m_running = false; break;
            }
        }

        //key release handling
        if (event.type == sf::Event::KeyReleased)
        {
            switch (event.key.code)
            {
            case sf::Keyboard::W: m_player->cInput->up = false; break;
            case sf::Keyboard::S: m_player->cInput->down = false; break;
            case sf::Keyboard::A: m_player->cInput->left = false; break;
            case sf::Keyboard::D: m_player->cInput->right = false; break;
            }
        }

        //mouse input
        if (event.type == sf::Event::MouseButtonPressed)
        {
            if (event.mouseButton.button == sf::Mouse::Left)
            {
                spawnBullet(m_player, Vec2(event.mouseButton.x, event.mouseButton.y));
            }
        }
    }
}