#pragma once

#include <SFML/Graphics.hpp>

class Particle : public sf::Drawable
{
public:
 Particle(float mass, float radius, sf::Vector2f position, sf::Vector2f velocity, sf::Color color);
 ~Particle();

 void update(float dt);
 static void resolveCollision(Particle &p1, Particle &p2);

 sf::Vector2f getPosition() const;
 sf::Vector2f getVelocity() const;
 float getMass() const;
 float getRadius() const;
 void setPosition(sf::Vector2f position);
 void setVelocity(sf::Vector2f velocity);

 static void setWorldDims(const sf::Vector2u &windowSize);

private:
 sf::Vector2f velocity;
 sf::CircleShape shape;
 sf::Color color;
 float mass, radius;

 static sf::Vector2u worldDims;

 virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
};
