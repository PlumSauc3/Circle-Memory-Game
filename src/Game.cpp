//
//  Game.cpp
//  circleGame
//
//  Created by Derek Xu on 2017-05-23.
//
//

#include "ofMain.h"
#include "Game.hpp"

Game::Game() {
    // Begin at round 1
    round = 1;
    score = 0;
    gameOver = false;
}

void Game::newRound(int difficulty) {
    //Store start time of this round
    startTime = ofGetElapsedTimef();
    //Prevents player from clicking targets before they've been hidden
    allowClicks = false;
    
    //Allow the player to miss 3 times per round
    lives = 3;
    switch (difficulty) {
            // Set the number of targets and bombs in the current round
        case 1:
            numTargets = 2;
            numBombs = 0;
            break;
        case 2:
            numTargets = 4;
            numBombs = 1;
            break;
        case 3:
            numTargets = 5;
            numBombs  = 3;
            break;
        case 4:
            numTargets = 6;
            numBombs = 3;
            break;
        case 5:
            numTargets = 7;
            numBombs = 4;
    }
    
    //Create new targets for the new level
    delete[] targets;
    delete[] bombs;
    targets = new Target[numTargets];
    bombs = new Bomb[numBombs];
    
    //Width and Height of the window
    const int HEIGHT = 768, WIDTH = 1024;
    const int RADIUS = 45; //Radius of each circle
    int gap = (RADIUS + 5) * 2; //Ensures no targets are placed offscreen
    
    float x, y;
    
    for (int i = 0; i < numTargets; i ++) { //Creates the desired number of targets objects
        //Generate random coordinates to draw a new circle
        bool valid = false;
        while (!valid) {
            valid = true;
            y = ofRandom(gap,HEIGHT-gap);
            x = ofRandom(gap,WIDTH-gap);
            for (int j = 0; j < numTargets; j++) {
                //Ensures that a circle will not be placed on another circle
                if (targets[j].distance(x,y) < 2 * targets[j].radius) {
                    valid = false;
                    break;
                }
            }
        }
        targets[i] = (Target(x,y,RADIUS));
    }
    
    for (int i = 0; i < numBombs; i ++) { //Creates the desired number of bombs objects
        //Generate random coordinates to draw a new circle
        bool valid = false;
        while (!valid) {
            valid = true;
            y = ofRandom(gap,HEIGHT-gap);
            x = ofRandom(gap,WIDTH-gap);
            for (int j = 0; j < numTargets; j++) {
                //Ensures that a circle will not be placed on another circle
                if (targets[j].distance(x,y) < 2 * targets[j].radius) {
                    valid = false;
                    break;
                }
            }
            for (int j = 0; j < numBombs; j++) {
                //Ensures that a circle will not be placed on another circle
                if (targets[j].distance(x,y) < 2 * targets[j].radius) {
                    valid = false;
                    break;
                }
            }
        }
        bombs[i] = (Bomb(x,y,RADIUS));
    }
}

int Game::getDifficulty() {
    // Determines the difficulty based on the round number
    if (round >= 1 && round <= 2) {
        return 1;
    } else if (round > 2 && round <= 4) {
        return 2;
    } else if (round > 4 && round <= 7){
        return 3;
    } else if (round > 7 && round <= 8) {
        return 4;
    } else {
        return 5;
    }
}

void Game::draw() {
    //Draws score
    ofSetColor(0,0,0);
    ofDrawBitmapString("Score: " + to_string(score), 20, 20);
    ofDrawBitmapString("Lives: " + to_string(lives), 1024 - 100,20);
    
    //Draws all circle objects to the screen
    if (!gameOver) {
        ofSetCircleResolution(100);
        for (int i = 0; i < numTargets; i ++) {
            if (targets[i].isVisible()) { //Check if circle should be hidden
                ofSetColor(targets[i].color); //Chooses the correct color
                ofDrawCircle(targets[i].x, targets[i].y, targets[i].radius); //Draw circle
            }
        }
        
        for (int i = 0; i < numBombs; i ++) {
            if (bombs[i].isVisible()) { //Check if circle should be hidden
                ofSetColor(bombs[i].color); //Chooses the correct color
                ofDrawCircle(bombs[i].x, bombs[i].y, bombs[i].radius); //Draw circle
            }
        }
    } else {
        //If the game is over, draw it to the screen
        ofDrawBitmapString("GAME OVER", 1024/2, 768/2);
        ofDrawBitmapString("Click anywhere to continue", 1024/2, 768/2 + 30);
    }
}

void Game::update() {
    float currTime = ofGetElapsedTimef();
    
    //Check if the player has lost all their lives
    if (lives == 0) {
        gameOver = true;
        return;
    }
    if (!allowClicks && currTime - startTime >= 5) {
        allowClicks = true; //Allow the user to click hidden circles
        for (int i = 0; i < numTargets; i ++) {
            targets[i].setVisibility(false);
        }
        for (int i = 0; i < numBombs; i ++) {
            bombs[i].setVisibility(false);
        }
    }
}

void Game::mousePressed(int x, int y) {
    if (!gameOver) {
        bool clickFound = false;
        if (allowClicks) {
            for (int i = 0; i < numTargets; i ++) {
                //Checks if the mouse click happened inside any of the hidden targets
                if (targets[i].onClick(x,y)) {
                    clickFound = true;
                    updateScore(round * 50);
                    break;
                }
            }
            
            for (int i = 0; i < numBombs; i ++) {
                //Checks if the click happened inside a bomb
                if (bombs[i].onClick(x,y)) {
                    clickFound = true;
                    updateScore(-99);
                    break;
                }
            }
            
            if (!clickFound) {
                updateLives();
            }
            
            if (isFinished()) {
                //When all targets have been clicked, increase the round number and start a new round
                round ++;
                newRound(getDifficulty());
            }
        }
    }
}

void Game::updateScore(int change) {
    //When a target is hit, change the score accordingly
    score += change;
    if (score < 0) {
        //Lowest possible score is 0
        score = 0;
    }
}

bool Game::isFinished() {
    //If all targets have been found, start a new round
    for (int i = 0; i < numTargets; i ++) {
        if (!targets[i].isVisible()) {
            return false;
        }
    }
    return true;
}

void Game::updateLives() {
    lives --;
}
