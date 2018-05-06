#include <GLFW/glfw3.h>

#include "Clock.hxx"

Clock::Clock() : m_time{glfwGetTime()}
{
}

double Clock::getElapsedTime()
{
  return glfwGetTime() - m_time;
}

void Clock::restart()
{
  m_time = glfwGetTime();
}
