#ifndef EVENT_HXX_
#define EVENT_HXX_

#include <SFML/Graphics.hpp>

/* Class event, inspired from the SFML Event class */
class Event {
public:
  /* Constructor */
  explicit Event(){};

  /* Event type */
  enum Type {
    SmokeEvent,
    FragmentDisappearsEvent,
    PieceTakenEvent
  };
  Type type;

  /* Event data */
  struct Smoke {
    sf::Vector3f position;
    int numberParticles;
    sf::Vector3f color;
  };

  struct Fragment {
    sf::Vector3f position;
    float volume;
    int piece;
  };

  struct Piece {
    sf::Vector2i position;
    int piece;
  };

  union {
    Smoke smoke;
    Fragment fragment;
    Piece piece;
  };

  /* Destructor */
  ~Event(){};
};

#endif
