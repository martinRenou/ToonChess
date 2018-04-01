#ifndef EVENTSTACK_HXX_
#define EVENTSTACK_HXX_

#include <vector>

#include "Event.hxx"

class EventStack {
public:
  /* Constructor */
  explicit EventStack(){};

  /* Array of events */
  static std::vector<Event> waitingQueue;

  /* Push an Event in the waiting queue */
  static void pushEvent(Event event);

  /* Poll an event, returns true if an event was there, false otherwise */
  static bool pollEvent(Event* event);

  /* Destructor */
  ~EventStack(){};
};

#endif
