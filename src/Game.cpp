//
//  Game.cpp
//  circleGame
//
//  Created by Derek Xu on 2017-05-23.
//
//

#include "ofMain.h"
#include "Game.hpp"
#include "Target.hpp"
#include <algorithm>
#include "ofApp.hpp"

Game::Game() {
    
}

Game::Game(ofApp * app_) {
    app = app_;
    //Prevents error; when round starts the pointer must be allocated to be deleted
    targets = new Target;
    bombs = new Bomb;
    gill.load("../../fonts/gill.ttf", 30);
    gillsmaller.load("../../fonts/gill.ttf", 26);
    calibri.load("../../fonts/Calibri.ttf", 20);
}

void Game::reset() {
    //Resets all varaibles in the game
    round = 1;
    score = 0;
    gameOver = false;
    openFiles();
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
            roundTimer = 5;
            break;
        case 2:
            numTargets = 4;
            numBombs = 1;
            roundTimer = 3;
            break;
        case 3:
            numTargets = 5;
            numBombs  = 3;
            roundTimer = 3;
            break;
        case 4:
            numTargets = 6;
            numBombs = 3;
            roundTimer = 2.5;
            break;
        case 5:
            numTargets = 7;
            numBombs = 4;
            roundTimer = 2.2;
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
        //Randomly generate how dangerous the bomb will be
        int danger = ofRandom(1,round);
        bombs[i] = (Bomb(x,y,RADIUS, danger));
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
    calibri.drawString("Score: " + to_string(score), 20, 40);
    calibri.drawString("Lives: " + to_string(lives), 1024 - 100,40);
    
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
                ofSetColor(0,0,0);
                int currWidth = calibri.stringWidth(to_string(bombs[i].getDanger()));
                int currHeight = calibri.stringHeight(to_string(bombs[i].getDanger()));
                calibri.drawString(to_string(bombs[i].getDanger()), bombs[i].x - currWidth/2, bombs[i].y + currHeight/2);
            }
        }
    } else {
        //If the game is over, draw it to the screen
        ofSetColor(0,0,0);
        int currWidth;
        gill.drawString("GAME OVER", 1024/2 - gill.stringWidth("GAME OVER")/2, 90);
        calibri.drawString("Click anywhere to continue", 1024 - 20 - calibri.stringWidth("Click anywhere to continue"), 768-30);
        
        gillsmaller.drawString("High Scores", 1024/2 - gillsmaller.stringWidth("High Scores")/2, 150);
        
        for (int i = 1; i <= leaderboard.size(); i ++) {
            currWidth = calibri.stringWidth(to_string(i) + ". " + to_string(leaderboard[i-1]));
            calibri.drawString(to_string(i) + ". " + to_string(leaderboard[i-1]), 1024/2 - currWidth/2, 160 + 30*i);
        }
    }
}

void Game::update() {
    float currTime = ofGetElapsedTimef();
    
    //Check if the player has lost all their lives
    if (lives == 0 && !gameOver) {
        gameOver = true;
        //Write the newest score to the list of scores
        oLeaderboard << score << "\n";
        oLeaderboard.close();
        loadScores();
        return;
    }
    if (!allowClicks && currTime - startTime >= roundTimer) {
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
                    updateScore(round * 50);
                    clickFound = true;
                    break;
                }
            }
            
            for (int i = 0; i < numBombs; i ++) {
                //Checks if the click happened inside a bomb
                if (bombs[i].onClick(x,y)) {
                    //Decrease the score by 100 when hitting a bomb
                    updateScore(-25 * bombs[i].getDanger());
                    //Remove a life for hitting a bomb
                    updateLives();
                    clickFound = true;
                    break;
                }
            }
            
            if (!clickFound) {
                //If the player missed, reduce the number of lives
                updateLives();
            }
            
            if (isFinished()) {
                //When all targets have been clicked, increase the round number and start a new round
                round ++;
                newRound(getDifficulty());
            }
        }
    } else {
        //When the game is over, clicking anywhere returns the player to the menu
        app->changeScreen(0);
    }
}

void Game::openFiles() {
    iLeaderboard.open("scores.txt");
    oLeaderboard.open("scores.txt", ios::app);
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

void Game::loadScores() {
    //Loads all historical scores in order to prepare a leaderboard
    leaderboard.clear();
    while (true) {
        int score;
        iLeaderboard >> score;
        if (iLeaderboard.eof()) {
            break;
        }
        leaderboard.push_back(score);
    }
    iLeaderboard.close();
    sortScores();
}

void Game::sortScores() {
    //Performs insertion sort on the leaderboard
    for (int i = 1; i < leaderboard.size(); i ++) {
        int item = leaderboard[i];
        int j = i;
        while (j > 0 && leaderboard[j-1] < item) {
            leaderboard[j] = leaderboard[j-1];
            j --;
        }
        leaderboard[j] = item;
    }
}
