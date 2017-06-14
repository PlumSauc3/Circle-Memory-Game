//
//  Bomb.cpp
//  circleGame
//
//  Created by Derek Xu on 2017-05-21.
//
//

#include "Target.hpp"
#include "Game.hpp"

Bomb::Bomb() {
    
}

Bomb::Bomb(float x_, float y_, float rad) : Circle(x_, y_, rad){
    color = red;
}

Bomb::Bomb(float x_, float y_, float rad, int danger_) : Circle(x_, y_, rad){
    color = red;
    danger = danger_;
}

int Bomb::getDanger() {
    return danger;
}
