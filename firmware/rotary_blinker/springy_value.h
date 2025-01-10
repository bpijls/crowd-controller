#ifndef _SPRINGY_VALUE
#define _SPRINGY_VALUE


class SpringyValue {

public:
  float position;
  float velocity;
  float restPosition;
  float damping;
  float stiffness;
  float mass;

  SpringyValue(float startPosition, float damping, float stiffness, float mass){
    this->damping = damping;
    this->velocity = 0.0f;
    this->position = startPosition;
    this->restPosition = startPosition;
    this->stiffness = stiffness;
    this->mass = mass;
  }

  void update(float dt){
    float acceleration = -stiffness * (position - restPosition) - damping * velocity;

    velocity += acceleration * dt;
    position += velocity * dt;
  }

  void perturb(float newPosition){
    position = newPosition;
  }
  
};

#endif