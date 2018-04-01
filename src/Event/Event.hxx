#ifndef EVENT_HXX_
#define EVENT_HXX_

#include <SFML/Graphics.hpp>

class Event {
public:
  /* Constructor */
  explicit Event(){};

  enum Type {
    SmokeEvent
  };

  Type type;

  struct Smoke {
    sf::Vector3f position;
    int numberParticles;
    sf::Vector3f color;
  };

  union {
    Smoke smoke;
  };

  /* Destructor */
  ~Event(){};
};

#endif
