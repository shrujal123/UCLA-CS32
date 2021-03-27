#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Actor.h"
#include <string>
#include <vector>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath); //StudentWorld constructor
    ~StudentWorld(); //StudentWorld destructor that calls the cleanup function
    virtual int init(); //initializes the actors and member variables
    virtual int move(); //main part of the game that makes all the actors doSomething during each tick
    virtual void cleanUp(); //deletes all the actors on the screen from the vector
    GhostRacer* getGhostRacer() { return m_ghostRacer; } //gets a pointer to the Ghost Racer
    
    void addActor(Actor* a); //adds an actor to the vector if we need to during the game like healing goodies and oil slicks
    void recordSoulSaved(); //records the number of souls saved
    Actor* sprayFirstAppropriateActor(Actor* a); //returns a pointer to the first actor the projectile comes into contact with
    bool overlaps(const Actor* a1, const Actor* a2) const; //checks if 2 actors overlap using the algorithm
    Actor* getOverlappingGhostRacer(Actor* a) const; //checks if any of the actors overlap with the ghost racer
    void setIsCollisionAvoidenceWortyhActorInLane(bool isCollisionAvoidenceWortyhActorInLane) { m_isCollisionAvoidenceWortyhActorInLane = isCollisionAvoidenceWortyhActorInLane; } //sets true if there is a collision avoidence worthy actor in the lane that the zombie cab is in 
    bool getIsCollisionAvoidenceWorthyActorInLane() { return m_isCollisionAvoidenceWortyhActorInLane; }
    void getclosestActorToCab(Actor* a, int &closestYAbove, int &closestYBelow); //returns true or false if there is or isnt a collision avoidence worthy actor in the lane that the zombie cab is in.
    

private:
    GhostRacer* m_ghostRacer;
    std::vector<Actor*> m_actors;
    int lastWhiteBorderLineAddedYCoord;
    int m_numberOfSoulsSaved;
    bool m_isCollisionAvoidenceWortyhActorInLane;
    int m_bonusPoints;
};

#endif // STUDENTWORLD_H_
