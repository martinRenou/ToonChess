#ifndef EVENT_HXX_
#define EVENT_HXX_

#include <string>

class Event {
private:
  std::string type = "NONE";

public:
  /* Constructor */
  explicit Event();

  /* Destructor */
  ~Event(){};
};

#endif
