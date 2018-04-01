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
    FragmentDisappearsEvent,
    PieceTakenEvent,
    PieceMovingEvent,
    PieceStopsEvent,
  };
  Type type;

  /* Event data */
  struct Fragment {
    sf::Vector3f position;
    float volume;
    int piece;
  };

  struct Piece {
    sf::Vector2i position;
    int piece;
  };

  struct MovingPiece {
    sf::Vector2f currentPosition;
    sf::Vector2i startPosition;
    sf::Vector2i endPosition;
  };

  union {
    Fragment fragment;
    Piece piece;
    MovingPiece movingPiece;
  };

  /* Destructor */
  ~Event(){};
};

#endif
