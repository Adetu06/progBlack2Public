#include <SFML/Graphics.hpp>
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>

struct MovingCircle {
    sf::CircleShape shape;
    sf::Text text;
public:
    float xSpeed;
    float ySpeed;
};

struct MovingRectangle {
    sf::RectangleShape shape;
    sf::Text text;
public:
    float xSpeed;
    float ySpeed;
};

int main() {
    std::ifstream fin("info.txt");
    std::string line;

    // Variables to store window and font properties
    unsigned windowWidth = 800;
    unsigned windowHeight = 600;
    sf::Font font;
    sf::Color fontColor = sf::Color::White;

    // Vectors to store our objects
    std::vector<MovingCircle> circles;
    std::vector<MovingRectangle> rectangles;

    while (std::getline(fin, line)) {
        std::istringstream iss(line);
        std::string type;
        iss >> type;

        if (type == "Window") {
            iss >> windowWidth >> windowHeight;
        }
        else if (type == "Font") {
            std::string fontPath;
            unsigned fontSize, r, g, b;
            iss >> fontPath >> fontSize >> r >> g >> b;
            if (!font.loadFromFile(fontPath)) {
                std::cerr << "Failed to load font: " << fontPath << std::endl;
            }
            fontColor = sf::Color(r, g, b);
        }
        else if (type == "Circle") {
            MovingCircle circle;
            std::string text;
            float x, y;
            int r, g, b, radius;

            iss >> text >> x >> y >> circle.xSpeed >> circle.ySpeed
                >> r >> g >> b >> radius;

            circle.shape.setRadius(radius);
            circle.shape.setPosition(x, y);
            circle.shape.setFillColor(sf::Color(r, g, b));

            circle.text.setString(text);
            circle.text.setFont(font);
            circle.text.setCharacterSize(18); // Default size, adjust as needed
            circle.text.setFillColor(fontColor);

            // Center text
            sf::FloatRect textBounds = circle.text.getLocalBounds();
            circle.text.setOrigin(textBounds.left + textBounds.width / 2.0f,
                textBounds.top + textBounds.height / 2.0f);
            circle.text.setPosition(circle.shape.getPosition() +
                sf::Vector2f(radius, radius));

            circles.push_back(circle);
        }
        else if (type == "Rectangle") {
            MovingRectangle rect;
            std::string text;
            float x, y;
            int r, g, b, width, height;

            iss >> text >> x >> y >> rect.xSpeed >> rect.ySpeed
                >> r >> g >> b >> width >> height;

            rect.shape.setSize(sf::Vector2f(width, height));
            rect.shape.setPosition(x, y);
            rect.shape.setFillColor(sf::Color(r, g, b));

            rect.text.setString(text);
            rect.text.setFont(font);
            rect.text.setCharacterSize(18);
            rect.text.setFillColor(fontColor);

            // Center text
            sf::FloatRect textBounds = rect.text.getLocalBounds();
            rect.text.setOrigin(textBounds.left + textBounds.width / 2.0f,
                textBounds.top + textBounds.height / 2.0f);
            rect.text.setPosition(rect.shape.getPosition() +
                sf::Vector2f(width / 2.0f, height / 2.0f));

            rectangles.push_back(rect);
        }
    }

    // Create window
    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "SFML Shapes");


    // Main loop
    while (window.isOpen()) {
        // Handle events
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Update positions
        for (auto& circle : circles) {
            sf::Vector2f pos = circle.shape.getPosition();
            circle.shape.setPosition(pos.x + circle.xSpeed, pos.y + circle.ySpeed);
            circle.text.setPosition(pos.x + circle.xSpeed + circle.shape.getRadius(),
                pos.y + circle.ySpeed + circle.shape.getRadius());
        }

        for (auto& rect : rectangles) {
            sf::Vector2f pos = rect.shape.getPosition();
            rect.shape.setPosition(pos.x + rect.xSpeed, pos.y + rect.ySpeed);
            rect.text.setPosition(pos.x + rect.xSpeed + rect.shape.getSize().x / 2.0f,
                pos.y + rect.ySpeed + rect.shape.getSize().y / 2.0f);
        }

        // Draw everything
        window.clear();

        for (auto& circle : circles) {
            window.draw(circle.shape);
            window.draw(circle.text);
            sf::Vector2f circlePos = circle.shape.getPosition();
            if (circlePos.x <= 0 || circlePos.x >= windowWidth - circle.shape.getRadius() * 2) {
                circle.xSpeed = -circle.xSpeed; // Reverse xSpeed
            }
            if (circlePos.y <= 0 || circlePos.y >= windowHeight - circle.shape.getRadius() * 2) {
                circle.ySpeed = -circle.ySpeed; // Reverse ySpeed
            }
        }

        for (auto& rect : rectangles) {
            window.draw(rect.shape);
            window.draw(rect.text);
            sf::Vector2f rectPos = rect.shape.getPosition();
            sf::Vector2f rectDim = rect.shape.getSize();
            if (rectPos.x <= 0 || rectPos.x >= windowWidth - rectDim.x * 2) {
                rect.xSpeed = -rect.xSpeed; // Reverse xSpeed
            }
            if (rectPos.y <= 0 || rectPos.y >= windowHeight - rectDim.y * 2) {
                rect.ySpeed = -rect.ySpeed; // Reverse ySpeed
            }
        }
        window.display();
    }

    return 0;
}