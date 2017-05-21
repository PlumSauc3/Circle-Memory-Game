#include "Circle.h"
#include <cmath>

ofColor Circle::green = ofColor(51, 204, 51);
ofColor Circle::red = ofColor(255, 51, 0);

Circle::Circle(float x_, float y_, float rad) {
  x = x_;
  y = y_;
  radius = rad;
  color = green;
}

float Circle::distance(int mouseX, int mouseY) {
  return sqrt(pow(mouseX - x, 2) + pow(mouseY - y, 2));
}
