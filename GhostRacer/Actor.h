#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class StudentWorld;

class Actor : public GraphObject
{
public:
	Actor(StudentWorld* thisWorld, int imageID, int startX, int startY, int startDirection, double size, int depth, double xSpeed, double ySpeed, bool alive, int hitpoints = 0) //The actor constructor that takes in the parameters to properly construct an actor e.g. zombie cabs, pedestrians, goodies, etc.
		: GraphObject(imageID, startX, startY, startDirection, size, depth), m_xSpeed(xSpeed), m_ySpeed(ySpeed), m_alive(alive), m_hitpoints(hitpoints), thisWorld(thisWorld) {}
	virtual ~Actor() {} //virtual destructor for actor
	virtual void doSomething() = 0; //all actors must do something
	void setXSpeed(int xSpeed) { m_xSpeed = xSpeed; } //sets the horizontal speed
	void setYSpeed(int ySpeed) { m_ySpeed = ySpeed; } //sets the vertical speed
	void setAlive(bool status) { m_alive = status; } //sets status
	virtual void setHitpoints(int hitpoints) //virtual function sets the hitpoints of the actor if its more than 0
	{ 
		m_hitpoints = hitpoints;
		if (m_hitpoints <= 0)
		{
			setAlive(false);
		}
	}
	int getXSpeed() { return m_xSpeed; } //returns the horizontal speed
	int getYSpeed() { return m_ySpeed; } //returns the vertical speed
	bool getAlive() { return m_alive; } //returns the status
	virtual int getHitpoints() { return m_hitpoints; } // virtual function returns the number of hitpoints the actor has
	virtual StudentWorld* getWorld() { return thisWorld; } //returns the Student World
	virtual bool isSprayable() { return false; } //virtual function returns if the actor can be hit with a projectile
	virtual bool getIsCollisionAvoidenceWorthyActor() { return false; } //virtual function returns if the actor is a collision avoidence worthy actor
	virtual void beSprayed() {} //virtual function that sprays the actor that the projectile hits

private:
	StudentWorld* thisWorld;
	double m_xSpeed;
	double m_ySpeed;
	bool m_alive;
	int m_hitpoints;
	virtual void move(){}
};

class BorderLine : public Actor
{
public:
	BorderLine(StudentWorld* thisWorld, int imageID, int x, int y) //constructor for borderline which is derived from Actor
		: Actor(thisWorld, imageID, x, y, 0, 2.0, 2, 0.0, -4.0, true) {}
	void doSomething(); //this function moves the borderlines down the screen

private:
	//no private member variables
};

class GhostRacer : public Actor
{
public:
	GhostRacer(StudentWorld* thisWorld) //constructor that takes the Student World
		: Actor(thisWorld, IID_GHOST_RACER, 128, 32, 90, 4.0, 0, 0.0, 0.0, true, 100), m_holyWater(10) {}
	void doSomething(); //doSomething function for ghost racer which allows it to change direction and speed up/slow down
	void setHolyWater(int holyWater) { m_holyWater = holyWater; } //sets the amount of holy water shots the ghost racer has
	int getHolyWater() { return m_holyWater; } // gets the amount of holy water
	bool getIsCollisionAvoidenceWorthyActor() { return true; } //it is a collision avoidence worthy actor

private:
	int m_holyWater;
	void move();

};

class Pedestrian : public Actor
{
public:
	Pedestrian(StudentWorld* thisWorld, int imageID, int x, int y) // constructor for pedestrian derived from Actor that takes the image ID (either a zombie or a human) and the starting x and y coordinates
		: Actor(thisWorld, imageID, x, y, 0, 2.0, 0, 0.0, -4.0, true, 2), m_movementPlanDistance(0) {}
	virtual void doSomething() {} //implements the move algorithm for the pedestrians
	void setMovementPlanDistance(int movementPlanDistance) { m_movementPlanDistance = movementPlanDistance; } //sets the movement plan distance of the pedestrian
	int getMovementPlanDistance() { return m_movementPlanDistance; } //gets the movement plan distance of the pedestrian
	virtual void move(); //contains the move algorithm for the pedestrians
	virtual bool isSprayable() { return true; } //it is a sprayable actor so it can be hit by a projectile
	bool getIsCollisionAvoidenceWorthyActor() { return true; } //it is a collision avoidence worthy actor

private:
	int m_movementPlanDistance;
	

};

class HumanPedestrian : public Pedestrian
{
public:
	HumanPedestrian(StudentWorld* thisWorld, int x, int y) //takes the StudentWorld and starting  x and y coordinates
		: Pedestrian(thisWorld, IID_HUMAN_PED, x, y) {} 
	void doSomething(); //contains the movement algorithm and overlap algorithm for human pedestrians
	void beSprayed(); //human peds get sprayed but are affected differently than zombie peds

private:
	
};

class ZombiePedestrian : public Pedestrian
{
public:
	ZombiePedestrian(StudentWorld* thisWorld, int x, int y) //takes the StudentWorld and starting  x and y coordinates 
		: Pedestrian(thisWorld, IID_ZOMBIE_PED, x, y), m_numberOfTicksBeforeGrunt(0) {}
	void doSomething(); //contains the movement algorithm and overlap algorithm for zombie pedestrians
	void beSprayed(); //zombie peds beSprayed function which clears the zombie ped and adds a healing goodie randomly sometimes

private:
	int m_numberOfTicksBeforeGrunt;
};

class GhostRacerActivatedObject : public Actor
{
public:
	GhostRacerActivatedObject(StudentWorld* thisWorld, int imageID, int x, int y, int direction, double size, int depth, double xSpeed, double ySpeed, bool alive, int hitpoints = 0) // constructor for object actors that depend on ghostRacers activity 
		: Actor(thisWorld, imageID, x, y, direction, size, depth, 0.0, -4.0, true) {}
	virtual void doSomething() {} //all these objects do something when they are activated
	virtual void move(); //they all move similarly down the screen
	virtual bool isSprayable() { return true; } //base class isSprayable
	void beSprayed(); //function that says what to do when sprayed


private:

};

class OilSlick : public GhostRacerActivatedObject
{
public:
	OilSlick(StudentWorld* thisWorld, int x, int y) //constructor for OilSlicks only needs a random starting x and y position
		: GhostRacerActivatedObject(thisWorld, IID_OIL_SLICK, x, y, 0, randInt(2, 5), 2, 0.0, -4.0, true) {}
	void doSomething(); //affects the direction of the ghost racer
	virtual bool isSprayable() { return false; } //it is not a sprayable object

private:

};

class HealingGoodie : public GhostRacerActivatedObject
{
public:
	HealingGoodie(StudentWorld* thisWorld, int x, int y) //constructor for HealingGoodies only needs a random starting x and y position
		: GhostRacerActivatedObject(thisWorld, IID_HEAL_GOODIE, x, y, 0, 1.0, 2, 0.0, -4.0, true) {}
	void doSomething(); //it is a sprayable object and it heals the ghost racer

private:
};

class HolyWaterGoodie : public GhostRacerActivatedObject
{
public:
	HolyWaterGoodie(StudentWorld* thisWorld, int x, int y) //constructor for HolyWater Goodies only needs a random starting x and y position
		: GhostRacerActivatedObject(thisWorld, IID_HOLY_WATER_GOODIE, x, y, 90, 2.0, 2, 0.0, -4.0, true) {}
	void doSomething(); //refills the ghost racers holy water spray supply

private:
};

class SoulGoodie : public GhostRacerActivatedObject
{
public:
	SoulGoodie(StudentWorld* thisWorld, int x, int y) //constructor for Soul Goodies only needs a random starting x and y position
		: GhostRacerActivatedObject(thisWorld, IID_SOUL_GOODIE, x, y, 0, 4.0, 2, 0.0, -4.0, true) {}
	void doSomething(); //if you collect enough souls you level up so it records the number
	virtual bool isSprayable() { return false; } //they are not sprayable

private:

};

class ZombieCab : public Actor
{
public:
	ZombieCab(StudentWorld* thisWorld, int x, int y, int ySpeed) //constructor for Zombie Cabs only needs a random starting x and y position and vertical speed that we calculate based on the actors in each lane at the time
		: Actor(thisWorld, IID_ZOMBIE_CAB, x, y, 90, 4.0, 0, 0.0, ySpeed, true, 3), m_planLength(0), m_alreadyDamagedGR(false) {}
	void doSomething(); //moves forward as an obstacle to the ghostRacer
	void setPlanLength(int planLength) { m_planLength = planLength; } //sets the distance the zombie cab will travel
	int getPlanLength() { return m_planLength; } //gets the plan distance
	bool getIsCollisionAvoidenceWorthyActor() { return true; } //it is a collision avoidence worthy actor
	virtual bool isSprayable() { return true; } //it is sprayable
	void beSprayed(); //the zombie cab takes damage from the projectiles and might turn into an oilslick when it loses all health

private:
	int m_planLength;
	bool m_alreadyDamagedGR;
	void move();

};

class HolyWaterProjectile : public Actor
{
public:
	HolyWaterProjectile(StudentWorld* thisWorld, int x, int y, int direction) //constructor for Holy Water Projectile only needs a random starting x and y position and the direction all based on the ghost racers position
		: Actor(thisWorld, IID_HOLY_WATER_PROJECTILE, x, y, direction, 1.0, 1, 0.0, 0.0, true), m_travelDistance(160) {}
	void doSomething(); //shoots forward and hits objects
	bool getIsCollisionAvoidenceWorthyActor() { return false; } //it is not a collision avoidence worthy actor

private:
	int m_travelDistance;

};


#endif // ACTOR_H_