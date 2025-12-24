#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <optional>

#include "particles/particle.h"

enum ScenarioMode
{
    SIMILAR_MASSES,
    DIFFERENT_MASSES
};

enum class AppState
{
    PlacingBall1,
    ConfiguringBall1,
    PlacingBall2,
    ConfiguringBall2,
    Running
};

struct TempValues
{
    float tempMass = 10.0f;
    float tempRadius = 30.0f;
    sf::Vector2f tempVelocity = {100.0f, 0.0f};
    sf::Vector2f tempPosition = {0.0f, 0.0f};
};

void drawVelocityVector(sf::RenderWindow& window, const sf::Vector2f& start, const sf::Vector2f& velocity)
{
    sf::Vector2f end = start + velocity;
    sf::Vertex line[] = {
        {start, sf::Color::Yellow},
        {end, sf::Color::Yellow}
    };
    window.draw(line, 2, sf::PrimitiveType::Lines);
}

void defineActionState(
  const sf::Event& event, AppState& currentState, TempValues& tempValues,
  std::vector<Particle>& particles, sf::Vector2f mousePos
)
{
    if (currentState != AppState::Running)
    {
        if (event.is<sf::Event::MouseButtonPressed>())
        {
            if (currentState == AppState::PlacingBall1)
            {
                tempValues.tempPosition = mousePos;
                currentState = AppState::ConfiguringBall1;
                std::cout << "Ball 1 Placed. Use ARROWS for Velocity, W/S for Mass. ENTER to confirm.\n";
            }
            else if (currentState == AppState::PlacingBall2)
            {
                tempValues.tempPosition = mousePos;
                currentState = AppState::ConfiguringBall2;
                std::cout << "Ball 2 Placed. Configure and press ENTER to Start.\n";
            }
        }

        if (currentState == AppState::ConfiguringBall1 || currentState == AppState::ConfiguringBall2)
        {
            if (const auto* keyEvent = event.getIf<sf::Event::KeyPressed>())
            {
                float velocityStep = 10.0f;

                if (keyEvent->code == sf::Keyboard::Key::Up)
                    tempValues.tempVelocity.y -= velocityStep;
                if (keyEvent->code == sf::Keyboard::Key::Down)
                    tempValues.tempVelocity.y += velocityStep;
                
                if (keyEvent->code == sf::Keyboard::Key::Left)
                    tempValues.tempVelocity.x -= velocityStep;
                if (keyEvent->code == sf::Keyboard::Key::Right)
                    tempValues.tempVelocity.x += velocityStep;

                if (keyEvent->code == sf::Keyboard::Key::W)
                {
                    tempValues.tempMass += 5.0f;
                    tempValues.tempRadius += 2.0f;
                }
                if (keyEvent->code == sf::Keyboard::Key::S)
                {
                    tempValues.tempMass = std::max(5.0f, tempValues.tempMass - 5.0f);
                    tempValues.tempRadius = std::max(10.0f, tempValues.tempRadius - 2.0f);
                }

                if (keyEvent->code == sf::Keyboard::Key::Enter)
                {
                    std::cout << "Confirmed! Mass: " << tempValues.tempMass << " | Vel: ("
                              << tempValues.tempVelocity.x << ", " << tempValues.tempVelocity.y << ")\n";

                    if (currentState == AppState::ConfiguringBall1)
                    {
                        particles.emplace_back(
                            tempValues.tempMass, tempValues.tempRadius, tempValues.tempPosition,
                            tempValues.tempVelocity, sf::Color::Cyan);
                        
                        tempValues.tempVelocity = {-100.0f, 0.0f}; 
                        currentState = AppState::PlacingBall2;
                    }
                    else if (currentState == AppState::ConfiguringBall2)
                    {
                        particles.emplace_back(
                            tempValues.tempMass, tempValues.tempRadius, tempValues.tempPosition,
                            tempValues.tempVelocity, sf::Color::Magenta);
                        
                        currentState = AppState::Running;
                        std::cout << "Simulation Started!\n";
                    }
                }
            }
        }
    }
    else
    {
        if (const auto* keyEvent = event.getIf<sf::Event::KeyPressed>())
        {
            if (keyEvent->code == sf::Keyboard::Key::R)
            {
                particles.clear();
                currentState = AppState::PlacingBall1;
                std::cout << "Resetting...\n";
            }
        }
    }
}

int main()
{
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Collisions");
    window.setFramerateLimit(60);

    Particle::setWorldDims(window.getSize());

    AppState currentState = AppState::PlacingBall1;
    TempValues tempValues;
    std::vector<Particle> particles;
    sf::Clock clock;

    while (window.isOpen())
    {
        sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
        sf::Vector2f mousePos = window.mapPixelToCoords(mousePixel);

        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (event->is<sf::Event::Resized>())
            {
                const auto eventSize = event->getIf<sf::Event::Resized>()->size;
                sf::FloatRect visibleArea({0, 0}, sf::Vector2f(eventSize.x, eventSize.y));
                window.setView(sf::View(visibleArea));
                Particle::setWorldDims(window.getSize());
            }

            defineActionState(*event, currentState, tempValues, particles, mousePos);
        }

        if (currentState == AppState::Running)
        {
            float dt = clock.restart().asSeconds();
            if (dt > 0.1f) dt = 0.1f;

            for (auto& p : particles)
            {
                p.update(dt);
            }

            if (particles.size() >= 2) {
                for (size_t i = 0; i < particles.size(); ++i) {
                    for (size_t j = i + 1; j < particles.size(); ++j) {
                        Particle::resolveCollision(particles[i], particles[j]);
                    }
                }
            }
        }
        else
        {
            clock.restart();
        }

        window.clear();

        for (const auto& p : particles)
        {
            window.draw(p);
        }

        if (currentState != AppState::Running)
        {
            sf::CircleShape ghost(tempValues.tempRadius);
            ghost.setOrigin({tempValues.tempRadius, tempValues.tempRadius});

            if (currentState == AppState::PlacingBall1 || currentState == AppState::PlacingBall2)
            {
                ghost.setPosition(mousePos);
                ghost.setFillColor(sf::Color(255, 255, 255, 100));
            }
            else
            {
                ghost.setPosition(tempValues.tempPosition);
                ghost.setFillColor(sf::Color(255, 255, 255, 255));

                drawVelocityVector(window, tempValues.tempPosition, tempValues.tempVelocity);
            }
            window.draw(ghost);
        }

        window.display();
    }

    return 0;
}