#include <vector>

#include "EventStack.hxx"

std::vector<Event> EventStack::waitingQueue;

void EventStack::pushEvent(Event event){
  waitingQueue.push_back(event);
};

bool EventStack::pollEvent(Event* event){
  if(waitingQueue.size()){
    *event = waitingQueue.at(0);
    waitingQueue.erase(waitingQueue.begin());

    return true;
  }else{
    return false;
  }
};
