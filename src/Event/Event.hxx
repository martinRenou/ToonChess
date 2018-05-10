#ifndef EVENT_HXX_
#define EVENT_HXX_

#include "../utils/math.hxx"

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
    Vector3f position;
    float volume;
    int piece;
  };

  struct Piece {
    Vector2i position;
    int piece;
  };

  struct MovingPiece {
    Vector2f currentPosition;
    Vector2i startPosition;
    Vector2i endPosition;
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
