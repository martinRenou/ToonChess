#ifndef EVENTSTACK_HXX_
#define EVENTSTACK_HXX_

#include <vector>

#include "Event.hxx"

class EventStack {
private:
  /* Array of events */
  std::vector<Event> waitingQueue;

public:
  /* Constructor */
  explicit EventStack();

  /* Push an Event in the waiting queue */
  void pushEvent(Event event);

  /* Poll an event, returns true if an event was there, false otherwise */
  bool pollEvent(Event* event);

  /* Destructor */
  ~EventStack(){};
};

#endif
