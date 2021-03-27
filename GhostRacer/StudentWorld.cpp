#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp
//Constructor
StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath), m_ghostRacer(nullptr), m_actors(), lastWhiteBorderLineAddedYCoord(0)
{}

//Destructor calls the cleanup function
StudentWorld::~StudentWorld()
{
    cleanUp();
}

//initializes all the member variables and adds the border lines
int StudentWorld::init()
{

    m_ghostRacer = new GhostRacer(this);
    m_numberOfSoulsSaved = 0;
    m_isCollisionAvoidenceWortyhActorInLane = false;
    m_bonusPoints = 5000;

    int N = VIEW_HEIGHT / SPRITE_HEIGHT;
    int M = VIEW_HEIGHT / (4*SPRITE_HEIGHT);

    int LEFT_EDGE = ROAD_CENTER - ROAD_WIDTH / 2;
    int RIGHT_EDGE = ROAD_CENTER + ROAD_WIDTH / 2;

    for (int j = 0; j < N; j++)
    {
        int X_leftYellow = LEFT_EDGE;
        int X_rightYellow = RIGHT_EDGE;
        int Y_leftYellow = j * SPRITE_HEIGHT;
        int Y_rightYellow = j * SPRITE_HEIGHT;

        m_actors.push_back(new BorderLine(this, IID_YELLOW_BORDER_LINE, X_leftYellow, Y_leftYellow));
        m_actors.push_back(new BorderLine(this, IID_YELLOW_BORDER_LINE, X_rightYellow, Y_rightYellow));

    }

    for (int j = 0; j < M; j++)
    {
        int X_leftWhite = LEFT_EDGE + ROAD_WIDTH / 3;
        int X_rightWhite = RIGHT_EDGE - ROAD_WIDTH / 3;
        int Y_leftWhite = j * (4 * SPRITE_HEIGHT);
        int Y_rightWhite = j * (4 * SPRITE_HEIGHT);

        m_actors.push_back(new BorderLine(this, IID_WHITE_BORDER_LINE, X_leftWhite, Y_leftWhite));
        m_actors.push_back(new BorderLine(this, IID_WHITE_BORDER_LINE, X_rightWhite, Y_rightWhite));

        lastWhiteBorderLineAddedYCoord = Y_leftWhite;
    }

    return GWSTATUS_CONTINUE_GAME;
}

// This code is here merely to allow the game to build, run, and terminate after you hit enter.
int StudentWorld::move()
{
    
    // Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
    
    //calls all the actors do something functions
    m_ghostRacer->doSomething();

    //iterates through the actors and checks the level and status of the actors to determine the game status
    vector<Actor*>::iterator p;
    for (p = m_actors.begin(); p != m_actors.end(); p++)
    {
        if ((*p)->getAlive())
        {
            (*p)->doSomething();
            if (!m_ghostRacer->getAlive())
            {
                decLives();
                return GWSTATUS_PLAYER_DIED;
            }
            if (m_numberOfSoulsSaved == getLevel() * 2 + 5)
            {

                advanceToNextLevel();
                increaseScore(m_bonusPoints);
                return GWSTATUS_FINISHED_LEVEL;
            }
        }
    }
    //clear the actors in the vector if they are dead
    for (p = m_actors.begin(); p != m_actors.end();)
    {
        if (!(*p)->getAlive())
        {
            delete *p;
            p = m_actors.erase(p);
        }
        else
        {
            p++;
        }
    }

    //animates the borders coming down the screen
    int new_border_y = VIEW_HEIGHT - SPRITE_HEIGHT;
    lastWhiteBorderLineAddedYCoord -= 4 + getGhostRacer()->getYSpeed();
    int delta_y = new_border_y - lastWhiteBorderLineAddedYCoord;

    if (delta_y >= SPRITE_HEIGHT)
    {

        int LEFT_EDGE = ROAD_CENTER - ROAD_WIDTH / 2;
        int RIGHT_EDGE = ROAD_CENTER + ROAD_WIDTH / 2;

        m_actors.push_back(new BorderLine(this, IID_YELLOW_BORDER_LINE, LEFT_EDGE, new_border_y));
        m_actors.push_back(new BorderLine(this, IID_YELLOW_BORDER_LINE, RIGHT_EDGE, new_border_y));

    }

    if (delta_y >= 4*SPRITE_HEIGHT)
    {

        int LEFT_EDGE = ROAD_CENTER - ROAD_WIDTH / 2 + ROAD_WIDTH / 3;
        int RIGHT_EDGE = ROAD_CENTER + ROAD_WIDTH / 2 - ROAD_WIDTH / 3;

        m_actors.push_back(new BorderLine(this, IID_WHITE_BORDER_LINE, LEFT_EDGE, new_border_y));
        m_actors.push_back(new BorderLine(this, IID_WHITE_BORDER_LINE, RIGHT_EDGE, new_border_y));

        lastWhiteBorderLineAddedYCoord = new_border_y;
    }
    
    //gets the chance of adding actors to the screen based on the level and pushing it onto the vector
    int chanceHumanPed = max(200-getLevel()*10, 30);
    int humRand = randInt(0, chanceHumanPed - 1);
    if (humRand == 0)
    {
        m_actors.push_back(new HumanPedestrian(this, randInt(0, VIEW_WIDTH), VIEW_HEIGHT));
    }

    int chanceZombiePed = max(200-getLevel()*10, 30);
    int zombieRand = randInt(0, chanceZombiePed - 1);
    if (zombieRand == 0)
    {
        m_actors.push_back(new ZombiePedestrian(this, randInt(0, VIEW_WIDTH), VIEW_HEIGHT));
    }

    int LEFT_EDGE = ROAD_CENTER - ROAD_WIDTH / 2;
    int RIGHT_EDGE = ROAD_CENTER + ROAD_WIDTH / 2;

    int chanceOilSlick = max(100-getLevel()*10, 20);
    int oilSlickRand = randInt(0, chanceOilSlick - 1);
    if (oilSlickRand == 0)
    {
        m_actors.push_back(new OilSlick(this, randInt(LEFT_EDGE, RIGHT_EDGE), VIEW_HEIGHT));
    }

    int chanceOfLostSoul = 100;
    int lostSoulRand = randInt(0, chanceOfLostSoul - 1);
    if (lostSoulRand == 0)
    {
        m_actors.push_back(new SoulGoodie(this, randInt(LEFT_EDGE, RIGHT_EDGE), VIEW_HEIGHT));
    }

    int chanceOfHolyWater = 100 + 10*1;
    int holyWaterRand = randInt(0, chanceOfHolyWater - 1);
    if (lostSoulRand == 0)
    {
        m_actors.push_back(new HolyWaterGoodie(this, randInt(LEFT_EDGE, RIGHT_EDGE), VIEW_HEIGHT));
    }

    //more complicated chance for adding zombie cabs 
    int LEFT_EDGE_MIDDLE = ROAD_CENTER - ROAD_WIDTH / 2 + ROAD_WIDTH / 3;
    int RIGHT_EDGE_MIDDLE = ROAD_CENTER + ROAD_WIDTH / 2 - ROAD_WIDTH / 3;

    int chanceVehicle = max(100-getLevel()*10, 20);
    int vehicleRand = randInt(0, chanceVehicle - 1);
    if (vehicleRand == 0)
    {
        int cur_lane = randInt(0, 2);
        int startX = 0;
        int startY = 0;
        int startYSpeed = 0;

        //loop through the lanes and actors to decide when to add a cab to the lane
        for (int i = 0; i < 3; i++)
        {
            bool foundLane = false;
            int lowestY = VIEW_HEIGHT - 1;
            int highestY = -1;
            for (p = m_actors.begin(); p != m_actors.end(); p++)
            {
                switch (cur_lane)
                {
                case 0:
                    if ((*p)->getX() >= LEFT_EDGE && (*p)->getX() < LEFT_EDGE_MIDDLE)
                    {
                    
                        if ((*p)->getY() < lowestY)
                        {
                            lowestY = (*p)->getY();
                        }
                        if ((*p)->getY() > highestY)
                        {
                            highestY = (*p)->getY();
                        }
                    }
                    startX = ROAD_CENTER - ROAD_WIDTH / 3;
                    break;
                case 1:
                    if ((*p)->getX() >= LEFT_EDGE_MIDDLE && (*p)->getX() < RIGHT_EDGE_MIDDLE)
                    {
                        if ((*p)->getY() < lowestY)
                        {
                            lowestY = (*p)->getY();
                        }
                        if ((*p)->getY() > highestY)
                        {
                            highestY = (*p)->getY();
                        }
                    }
                    startX = ROAD_CENTER;
                    break;
                case 2:
                    if ((*p)->getX() >= RIGHT_EDGE_MIDDLE && (*p)->getX() < RIGHT_EDGE)
                    {
                        if ((*p)->getY() < lowestY)
                        {
                            lowestY = (*p)->getY();
                        }
                        if ((*p)->getY() > highestY)
                        {
                            highestY = (*p)->getY();
                        }
                    }
                    startX = ROAD_CENTER + ROAD_WIDTH / 3;
                    break;
                }
                if (lowestY < VIEW_HEIGHT - 1 && (*p)->getY() >= (VIEW_HEIGHT / 3))
                {
                    foundLane = true;
                    m_isCollisionAvoidenceWortyhActorInLane = true;
                    startY = SPRITE_HEIGHT / 2;
                    startYSpeed = m_ghostRacer->getYSpeed() + randInt(2, 4);
                    break;
                }
                else if (highestY > -1 && (*p)->getY() <= (VIEW_HEIGHT * 2/3))
                {
                    foundLane = true;
                    m_isCollisionAvoidenceWortyhActorInLane = true;
                    startY = VIEW_HEIGHT - SPRITE_HEIGHT / 2;
                    startYSpeed = m_ghostRacer->getYSpeed() - randInt(2, 4);
                    break;
                }
                //if not top or bottom just break
            }
            if (foundLane)
            {
                m_actors.push_back(new ZombieCab(this, startX, startY, startYSpeed));
            }
            else 
            {
                if (cur_lane == 0)
                {
                    cur_lane = 1;
                }
                else if (cur_lane == 1)
                {
                    cur_lane = 2;
                }
                else if (cur_lane == 2)
                {
                    cur_lane == 0;
                }
                lowestY = VIEW_HEIGHT - 1;
                highestY = -1;
            }
            break;
        }
    }

    //determines the bonus points
    if (m_bonusPoints > 0)
    {
        m_bonusPoints--;
    }

    //game status line using string stream
    ostringstream oss;

    oss.fill('0');
    oss << "Score: ";
    oss << setw(6) << getScore() << "  ";

    oss << "Lvl: ";
    oss << getLevel() << "  ";

    oss << "Souls2Save: ";
    oss << m_numberOfSoulsSaved << "  ";

    oss << "Lives: ";
    oss << getLives() << "  ";

    oss << "Health: ";
    oss << m_ghostRacer->getHitpoints() << "  ";

    oss << "Sprays: ";
    oss << m_ghostRacer->getHolyWater() << "  ";

    oss << "Bonus: ";
    oss << m_bonusPoints << endl;

    setGameStatText(oss.str());

    return GWSTATUS_CONTINUE_GAME;
}

//deletes all the actors from the vector
void StudentWorld::cleanUp()
{
    delete m_ghostRacer;
    vector<Actor*>::iterator p;
    for (p = m_actors.begin(); p != m_actors.end(); p++)
    {
        delete *p;
    }
    m_actors.clear();

}

//adds an actor to the vector
void StudentWorld::addActor(Actor* a)
{
    m_actors.push_back(a);
}

//uses overlap algorithm to determine if 2 actors overlap eachother
bool StudentWorld::overlaps(const Actor* a1, const Actor* a2) const
{

    double delta_x = abs(a2->getX() - a1->getX());
    double delta_y = abs(a2->getY() - a1->getY());
    double radius_sum = a1->getRadius() + a2->getRadius();

    if (delta_x < radius_sum*0.25 && delta_y < radius_sum*0.6)
    {
        return true;
    }
    return false;

}

//uses overlap function to check if the ghostracer comes into contact with another actor
Actor* StudentWorld::getOverlappingGhostRacer(Actor* a) const
{
    if (m_ghostRacer->getAlive() && overlaps(a, m_ghostRacer))
    {
        return m_ghostRacer;
    }
    return nullptr;
}

//counts the number of souls saved
void StudentWorld::recordSoulSaved()
{
    m_numberOfSoulsSaved++;
}

//checks if the closest actor is sprayable and returns it
Actor* StudentWorld::sprayFirstAppropriateActor(Actor* a)
{
    vector<Actor*>::iterator p;
    for (p = m_actors.begin(); p != m_actors.end(); p++)
    {
        if ((*p)->isSprayable())
        {
            if (overlaps(a, *p))
            {
                a->setAlive(false);
                return *p;
                break;
            }
        }
    }
    return nullptr;
}

//algorithm to determine how to affect the speed of the zombie cab
void StudentWorld::getclosestActorToCab(Actor* a, int &closestYAbove, int &closestYBelow)
{
    int LEFT_EDGE = ROAD_CENTER - ROAD_WIDTH / 2;
    int RIGHT_EDGE = ROAD_CENTER + ROAD_WIDTH / 2;
    int LEFT_EDGE_MIDDLE = ROAD_CENTER - ROAD_WIDTH / 2 + ROAD_WIDTH / 3;
    int RIGHT_EDGE_MIDDLE = ROAD_CENTER + ROAD_WIDTH / 2 - ROAD_WIDTH / 3;

    int cabLane = 0;
    if (a->getX() >= LEFT_EDGE && a->getX() < LEFT_EDGE_MIDDLE)
    {
        cabLane = 1;
    }
    else if (a->getX() >= LEFT_EDGE_MIDDLE && a->getX() < RIGHT_EDGE_MIDDLE)
    {
        cabLane = 2;
    }
    else if(a->getX() >= RIGHT_EDGE_MIDDLE && a->getX() < RIGHT_EDGE)
    {
        cabLane = 3;
    }

    vector<Actor*>::iterator p;
    for (p = m_actors.begin(); p != m_actors.end(); p++)
    {
        if (!(*p)->getIsCollisionAvoidenceWorthyActor())
        {
            continue;
        }
        switch (cabLane)
        {
        case 1:
            if ((*p)->getX() >= LEFT_EDGE && (*p)->getX() < LEFT_EDGE_MIDDLE)
            {
                if ((*p)->getY() < closestYAbove)
                {
                    closestYAbove = (*p)->getY();
                }
                if ((*p)->getY() > closestYBelow)
                {
                    closestYBelow = (*p)->getY();
                }
            }
            break;
        case 2:
            if ((*p)->getX() >= LEFT_EDGE_MIDDLE && (*p)->getX() < RIGHT_EDGE_MIDDLE)
            {
                if ((*p)->getY() < closestYAbove)
                {
                    closestYAbove = (*p)->getY();
                }
                if ((*p)->getY() > closestYBelow)
                {
                    closestYBelow = (*p)->getY();
                }
            }
            break;
        case 3:
            if ((*p)->getX() >= RIGHT_EDGE_MIDDLE && (*p)->getX() < RIGHT_EDGE)
            {
                if ((*p)->getY() < closestYAbove)
                {
                    closestYAbove = (*p)->getY();
                }
                if ((*p)->getY() > closestYBelow)
                {
                    closestYBelow = (*p)->getY();
                }
            }
            break;
        }

    }
}