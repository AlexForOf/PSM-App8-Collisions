#include <SFML/Graphics.hpp>
#include <iostream>

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

void initScenario(ScenarioMode mode, std::vector<Particle> &particles, sf::Vector2u windowSize)
{
 particles.clear();
 float cx = windowSize.x / 2.0f;
 float cy = windowSize.y / 2.0f;

 if (mode == ScenarioMode::SIMILAR_MASSES)
 {
  particles.emplace_back(12.0f, 30.0f, sf::Vector2f(cx - 200, cy), sf::Vector2f(200.0f, 0.0f), sf::Color::Cyan);
  particles.emplace_back(8.0f, 30.0f, sf::Vector2f(cx + 200, cy), sf::Vector2f(-200.0f, 0.0f), sf::Color::Magenta);
 }
 else if (mode == ScenarioMode::DIFFERENT_MASSES)
 {
  particles.emplace_back(100.0f, 50.0f, sf::Vector2f(cx - 200, cy), sf::Vector2f(50.0f, 0.0f), sf::Color::Red);

  particles.emplace_back(5.0f, 15.0f, sf::Vector2f(cx + 100, cy), sf::Vector2f(0.0f, 0.0f), sf::Color::Green);
 }
}

void drawVelocityVector(sf::RenderWindow &window, const sf::Vector2f &start, const sf::Vector2f &velocity)
{
 sf::Vector2f end = start + velocity;

 sf::Vertex line[] =
     {
         {start, sf::Color::Yellow},
         {end, sf::Color::Yellow}};

 window.draw(line, 2, sf::PrimitiveType::Lines);
}

int main()
{
 sf::RenderWindow window(sf::VideoMode({800, 600}), "Collisions");
 window.setFramerateLimit(60);

 Particle::setWorldDims(window.getSize());
 AppState currentState = AppState::PlacingBall1;

 std::vector<Particle> particles;
 sf::Clock clock;

 float tempMass = 10.0f;
 float tempRadius = 30.0f;
 sf::Vector2f tempVelocity(100.0f, 0.0f);
 sf::Vector2f tempPosition;

 // initScenario(ScenarioMode::SIMILAR_MASSES, particles, window.getSize());

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

   if (currentState != AppState::Running)
   {
    if (event->is<sf::Event::MouseButtonPressed>())
    {
     if (currentState == AppState::PlacingBall1)
     {
      tempPosition = mousePos;
      currentState = AppState::ConfiguringBall1;
      std::cout << "Ball 1 Placed. Use ARROWS for Velocity, W/S for Mass. ENTER to confirm.\n";
     }
     else if (currentState == AppState::PlacingBall2)
     {
      tempPosition = mousePos;
      currentState = AppState::ConfiguringBall2;
      std::cout << "Ball 2 Placed. Configure and press ENTER to Start.\n";
     }
    }

    if (const auto *keyEvent = event->getIf<sf::Event::KeyPressed>())
    {
     if (keyEvent->code == sf::Keyboard::Key::Enter)
     {
      if (currentState == AppState::ConfiguringBall1)
      {
       particles.emplace_back(tempMass, tempRadius, tempPosition, tempVelocity, sf::Color::Cyan);
       tempVelocity = {-100.0f, 0.0f};
       currentState = AppState::PlacingBall2;
      }
      else if (currentState == AppState::ConfiguringBall2)
      {
       particles.emplace_back(tempMass, tempRadius, tempPosition, tempVelocity, sf::Color::Magenta);
       currentState = AppState::Running;
       std::cout << "Simulation Started!" << std::endl;
      }
     }

     float velocityStep = 10.0f;
     if (currentState == AppState::ConfiguringBall1 || currentState == AppState::ConfiguringBall2)
     {
      if (keyEvent->code == sf::Keyboard::Key::Up)
       tempVelocity.y -= velocityStep;
      if (keyEvent->code == sf::Keyboard::Key::Down)
       tempVelocity.y += velocityStep;
      if (keyEvent->code == sf::Keyboard::Key::Left)
       tempVelocity.x -= velocityStep;
      if (keyEvent->code == sf::Keyboard::Key::Right)
       tempVelocity.x += velocityStep;

      if (keyEvent->code == sf::Keyboard::Key::W)
      {
       tempMass += 5.0f;
       tempRadius += 2.0f;
      }
      if (keyEvent->code == sf::Keyboard::Key::S)
      {
       tempMass = std::max(5.0f, tempMass - 5.0f);
       tempRadius = std::max(10.0f, tempRadius - 2.0f);
      }

      std::cout << "Mass: " << tempMass << " | Vel: (" << tempVelocity.x << ", " << tempVelocity.y << ")\n";
     }
    }
   }
   else
   {
    if (const auto *keyEvent = event->getIf<sf::Event::KeyPressed>())
    {
     if (keyEvent->code == sf::Keyboard::Key::R)
     {
      particles.clear();
      currentState = AppState::PlacingBall1;
      std::cout << "Resetting..." << std::endl;
     }
    }
   }
  }

  if (currentState == AppState::Running)
  {
   float dt = clock.restart().asSeconds();

   if (dt > 0.1f)
    dt = 0.1f;

   for (auto &p : particles)
   {
    p.update(dt);
   }
   Particle::resolveCollision(particles[0], particles[1]);
  }
  else
  {
   clock.restart();
  }

  window.clear();

  for (auto &p : particles)
  {
   window.draw(p);
  }

  if (currentState != AppState::Running)
  {
   sf::CircleShape ghost(tempRadius);
   ghost.setOrigin({tempRadius, tempRadius});

   if (currentState == AppState::PlacingBall1 || currentState == AppState::PlacingBall2)
   {
    ghost.setPosition(mousePos);
    ghost.setFillColor(sf::Color(255, 255, 255, 100));
   }
   else
   {
    ghost.setPosition(tempPosition);
    ghost.setFillColor(sf::Color(255, 255, 255, 255));

    drawVelocityVector(window, tempPosition, tempVelocity);
   }
   window.draw(ghost);
  }

  window.display();
 }

 return 0;
}