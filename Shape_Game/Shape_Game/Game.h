#pragma once

#include "Entity.h"
#include "EntityManager.h"

#include <SFML/Graphics.hpp>


struct PlayerConfig { int SR, CR, FR, FG, FB, OR, OG, OB, OT, V; float S; }; 
struct EnemyConfig { int SR, CR, OR, OG, OB, OT, VMIN, VMAX, L, SI; float SMIN, SMAX; }; 
struct BulletConfig { int SR, CR, FR, FG, FB, OR, OG, OB, OT, V, L; float S; };
struct WindowConfig { int W, H, FL; bool FS; };
struct FontConfig { std::string F; int S, R, G, B; };

class Game
{

	sf::RenderWindow	m_window;					// the window we will draw to
	EntityManager		m_entities;					// vector of entities to maintain
	sf::Font			m_font;						// the font we will use to draw
	sf::Text			m_text;						// the score text to be drawn to the screen
	PlayerConfig		m_playerConfig;
	EnemyConfig			m_enemyConfig;
	BulletConfig		m_bulletConfig;
	WindowConfig		m_windowConfig;
	FontConfig			m_fontConfig;
	int 				m_score = 0;
	int					m_currentFrame = 0;
	int					m_lastEnemySpawnTime = 0;
	bool				m_paused = false;			// whether we update game logic
	bool 				m_running = true;			// whether the game is running

	std::shared_ptr<Entity> m_player;

    void init(const std::string& config);  // Initialize with configuration file

    void setPaused(bool paused);           // Pause state management

    void sMovement();                      // Handle entity movement
    void sUserInput();                     // Process user input
    void sLifespan();                      // Manage entity lifespans
    void sRender();                        // Handle rendering
    void sEnemySpawner();                  // Control enemy spawning
    void sCollision();                     // Handle collisions

    void spawnPlayer();
    void spawnEnemy();
    void spawnSmallEnemies(std::shared_ptr<Entity> entity);
    void spawnBullet(std::shared_ptr<Entity> entity, const Vec2& mousePos);
	void spawnSpecialWeapon(std::shared_ptr<Entity> entity);

public: 
	Game(const std::string & config);	// constructor, takes in game config

	void run();
};
