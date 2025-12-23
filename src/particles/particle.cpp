#include "particles/particle.h"
#include <cmath>
#include "particle.h"

sf::Vector2u Particle::worldDims = {0, 0};

Particle::Particle(float mass, float radius, sf::Vector2f position, sf::Vector2f velocity, sf::Color color)
    : mass(mass), radius(radius), velocity(velocity), color(color)
{
 shape = sf::CircleShape(radius, 30);
 shape.setOrigin(sf::Vector2f(radius, radius));
 shape.setPosition(position);
 shape.setFillColor(color);
}

Particle::~Particle()
{
}

void Particle::update(float dt)
{
 sf::Vector2f pos = shape.getPosition();
 pos += velocity * dt;

 float r = shape.getRadius();

 if (pos.x + r > worldDims.x)
 {
  pos.x = (float)worldDims.x - r;
  velocity.x *= -1.0f;
 }
 else if (pos.x - r < 0)
 {
  pos.x = r;
  velocity.x *= -1.0f;
 }

 if (pos.y + r > worldDims.y)
 {
  pos.y = (float)worldDims.y - r;
  velocity.y *= -1.0f;
 }
 else if (pos.y - r < 0)
 {
  pos.y = r;
  velocity.y *= -1.0f;
 }

 shape.setPosition(pos);
}

void Particle::resolveCollision(Particle &p1, Particle &p2)
{
 sf::Vector2f diff = p1.shape.getPosition() - p2.shape.getPosition();
 float distance = diff.length();
 float radiusSum = p1.getRadius() + p2.getRadius();
 if (distance < radiusSum)
 {
  if (distance <= 0.0f)
   return;

  sf::Vector2f n = diff.normalized();
  sf::Vector2f t(-n.y, n.x);

  float v1n = p1.getVelocity().dot(n);
  float v1t = p1.getVelocity().dot(t);

  float v2n = p2.getVelocity().dot(n);
  float v2t = p2.getVelocity().dot(t);

  float m1 = p1.getMass();
  float m2 = p2.getMass();
  float totalMass = m1 + m2;

  float v1n_final = (v1n * (m1 - m2) + 2.0f * m2 * v2n) / totalMass;
  float v2n_final = (v2n * (m2 - m1) + 2.0f * m1 * v1n) / totalMass;

  p1.setVelocity((n * v1n_final) + (t * v1t));
  p2.setVelocity((n * v2n_final) + (t * v2t));

  float overlap = radiusSum - distance;
  sf::Vector2f correction = n * (overlap / 2.0f);

  p1.shape.move(correction);
  p2.shape.move(-correction);
 }
}

void Particle::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
 target.draw(shape, states);
}

sf::Vector2f Particle::getPosition() const
{
 return shape.getPosition();
}
sf::Vector2f Particle::getVelocity() const
{
 return velocity;
}
float Particle::getMass() const
{
 return mass;
}
float Particle::getRadius() const
{
 return radius;
}
void Particle::setPosition(sf::Vector2f position)
{
 shape.setPosition(position);
}
void Particle::setVelocity(sf::Vector2f velocity)
{
 this->velocity = velocity;
}
void Particle::setWorldDims(const sf::Vector2u &worldSize)
{
 worldDims = worldSize;
}
