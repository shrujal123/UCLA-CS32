#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

class GhostRacer;

//move algorithm for border lines
void BorderLine::doSomething()
{
	int vert_speed = getYSpeed() - getWorld()->getGhostRacer()->getYSpeed();
	int horiz_speed = getXSpeed();

	int new_y = getY() + vert_speed;
	int new_x = getX() + horiz_speed;

	moveTo(new_x, new_y);

	if (getX() < 0 || getY() < 0 || getX() > VIEW_WIDTH || getY() > VIEW_HEIGHT)
	{
		setAlive(false);
		return;
	}
	
}


void GhostRacer::doSomething()
{

	if (!getAlive())
	{
		return;
	}

	int ch;
	int LEFT_EDGE = ROAD_CENTER - ROAD_WIDTH / 2;
	int RIGHT_EDGE = ROAD_CENTER + ROAD_WIDTH / 2;

	if (getX() <= LEFT_EDGE)
	{
		if (getDirection() > 90)
		{
			setHitpoints(getHitpoints() - 10);
			setDirection(82);
		}

	}
	else if (!(getX() <= LEFT_EDGE) && getX() >= RIGHT_EDGE)
	{
		if (getDirection() < 90)
		{
			setHitpoints(getHitpoints() - 10);
			setDirection(98);
		}
	}

	//checks the key pressed and correspondingally moves the ghost racer
	if (getWorld()->getKey(ch))
	{
		switch (ch)
		{
		case KEY_PRESS_LEFT:
			if (getDirection() < 114)
			{
				setDirection(getDirection() + 8);
			}
			break;
		case KEY_PRESS_RIGHT:
			if (getDirection() > 66)
			{
				setDirection(getDirection() - 8);
			}
			break;
		case KEY_PRESS_UP:
			if (getYSpeed() < 5)
			{
				setYSpeed(getYSpeed() + 1);
			}
			break;
		case KEY_PRESS_DOWN:
			if (getYSpeed() > -1)
			{
				setYSpeed(getYSpeed() - 1);
			}
			break;
		//holy water spray algorithm
		case KEY_PRESS_SPACE:
			if (m_holyWater >= 1)
			{
				int delta_x = cos(getDirection() * (3.1415 / 180)) * SPRITE_HEIGHT;
				int delta_y = sin(getDirection() * (3.1415 / 180)) * SPRITE_HEIGHT;
				
				getWorld()->addActor(new HolyWaterProjectile(getWorld(), getX() + delta_x, getY() + delta_y, getDirection()));
				getWorld()->playSound(SOUND_PLAYER_SPRAY);
				m_holyWater--;
			}
			break;
		default:
			break;
		}
	}
	move();

}

void HumanPedestrian::doSomething()
{
	if (!getAlive())
	{
		return;
	}
	//checks overlap
	if (getWorld()->getOverlappingGhostRacer(this) != nullptr)
	{
		getWorld()->getGhostRacer()->setAlive(false);
		setAlive(false);
		return;
	}

	//movement algorithm for human pedestrian
	move();
	setMovementPlanDistance(getMovementPlanDistance() - 1);
	if (getMovementPlanDistance() > 0)
	{
		return;
	}
	else 
	{
		int newXSpeed = 0;
		do
		{
			newXSpeed = randInt(-3, 3);
		} while (newXSpeed == 0);
		setXSpeed(newXSpeed);
		setMovementPlanDistance(randInt(4, 32));

		if (getXSpeed() < 0)
		{
			setDirection(180);
		}
		else if (getXSpeed() > 0)
		{
			setDirection(0);
		}
	}
}

void ZombiePedestrian::doSomething()
{
	if (!getAlive())
	{
		return;
	}
	//checks overlap
	if (getWorld()->getOverlappingGhostRacer(this) != nullptr)
	{
		getWorld()->getGhostRacer()->setHitpoints(getWorld()->getGhostRacer()->getHitpoints() - 5);
		setHitpoints(getHitpoints() - 2);
		return;
	}

	//movement algorithm for zombie pedestrian
	if ((getWorld()->getGhostRacer()->getX() - getX() < 30 && getX() - getWorld()->getGhostRacer()->getX() < 30) && getY() > getWorld()->getGhostRacer()->getY())
	{
		setDirection(270);
		if (getX() < getWorld()->getGhostRacer()->getX())
		{
			setXSpeed(1);
		}
		else if (getX() > getWorld()->getGhostRacer()->getX())
		{
			setXSpeed(-1);
		}
		else
		{
			setXSpeed(0);
		}
	}
	m_numberOfTicksBeforeGrunt--;

	if (m_numberOfTicksBeforeGrunt <= 0)
	{
		getWorld()->playSound(SOUND_ZOMBIE_ATTACK);
		m_numberOfTicksBeforeGrunt = 20;
	}

	move();

	if (getMovementPlanDistance() > 0)
	{
		setMovementPlanDistance(getMovementPlanDistance() - 1);
		return;
	}
	else
	{
		int newXSpeed = 0;
		do
		{
			newXSpeed = randInt(-3, 3);
		} while (newXSpeed == 0);
		setXSpeed(newXSpeed);
		setMovementPlanDistance(randInt(4, 32));

		if (getXSpeed() < 0)
		{
			setDirection(180);
		}
		else if (getXSpeed() > 0)
		{
			setDirection(0);
		}
	}
}

void OilSlick::doSomething()
{
	move();

	//checks if overlaps with ghost racer and changes ghost racer direction
	if (getWorld()->getOverlappingGhostRacer(this) != nullptr)
	{
		getWorld()->playSound(SOUND_OIL_SLICK);
		int randAdj = randInt(5, 20);
		int randDirection = randInt(0, 1);
		switch (randDirection)
		{
		case 0:
			randAdj = -randAdj;
			break;
		case 1:
			randAdj = randAdj;
			break;
		default:
			break;
		}
		if (getWorld()->getGhostRacer()->getDirection() + randAdj <= 60)
		{
			getWorld()->getGhostRacer()->setDirection(60);
		}
		else if (getWorld()->getGhostRacer()->getDirection() + randAdj >= 120)
		{
			getWorld()->getGhostRacer()->setDirection(120);
		}
		else
		{
			getWorld()->getGhostRacer()->setDirection(getWorld()->getGhostRacer()->getDirection() + randAdj);
		}
	}

}

void HealingGoodie::doSomething()
{
	move();

	//checks if overlaps with ghost racer and adds 10 health
	if (getWorld()->getOverlappingGhostRacer(this) != nullptr)
	{
		if (getWorld()->getGhostRacer()->getHitpoints() + 10 >= 100)
		{
			getWorld()->getGhostRacer()->setHitpoints(100);
		}
		else
		{
			getWorld()->getGhostRacer()->setHitpoints(getWorld()->getGhostRacer()->getHitpoints() + 10);
		}
		setAlive(false);
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->increaseScore(250);
	}
}

void HolyWaterGoodie::doSomething()
{
	move();

	//checks if overlaps with ghost racer and adds 10 holy water shots
	if (getWorld()->getOverlappingGhostRacer(this) != nullptr)
	{
		getWorld()->getGhostRacer()->setHolyWater(getWorld()->getGhostRacer()->getHolyWater() + 10);
		setAlive(false);
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->increaseScore(50);

	}

}

void SoulGoodie::doSomething()
{
	move();

	//checks if overlaps with ghost racer and adds soul saved to the record

	if (getWorld()->getOverlappingGhostRacer(this) != nullptr)
	{
		getWorld()->recordSoulSaved();
		setAlive(false);
		getWorld()->playSound(SOUND_GOT_SOUL);
		getWorld()->increaseScore(100);
	}

	setDirection(getDirection() - 10);
}

void ZombieCab::doSomething()
{
	if (!getAlive())
	{
		return;
	}
	//checks if hit by ghost racer, then makes it swerve
	if (getWorld()->getOverlappingGhostRacer(this) != nullptr)
	{
		if (!m_alreadyDamagedGR)
		{
			getWorld()->playSound(SOUND_VEHICLE_CRASH);
			getWorld()->getGhostRacer()->setHitpoints(getWorld()->getGhostRacer()->getHitpoints() - 20);
			if (getX() <= getWorld()->getGhostRacer()->getX())
			{
				setXSpeed(-5);
				setDirection(120 + randInt(0, 19));
			}
			if (getX() > getWorld()->getGhostRacer()->getX())
			{
				setXSpeed(5);
				setDirection(60 - randInt(1, 19));
			}
			m_alreadyDamagedGR = true;
		}

	}

	//movement and speed algorithm
	move();

	//zombie cab collision avoidence worthy actor (step 4, 5)
	int closestYAbove = VIEW_HEIGHT - 1;
	int closestYBelow = 1;

	getWorld()->getclosestActorToCab(this, closestYAbove, closestYBelow);

	if ((getYSpeed() > getWorld()->getGhostRacer()->getYSpeed()) && getWorld()->getIsCollisionAvoidenceWorthyActorInLane())
	{
		if (closestYAbove < 96 && closestYAbove > 0)
		{
			setYSpeed(getYSpeed() - 0.5);
			return;
		}
	}
	if ((getYSpeed() <= getWorld()->getGhostRacer()->getYSpeed()) && getWorld()->getIsCollisionAvoidenceWorthyActorInLane())
	{
		if (closestYBelow < 96 && closestYAbove > 0)
		{
			setYSpeed(getYSpeed() + 0.5);
			return;
		}
	}


	m_planLength--;
	if (m_planLength > 0)
	{
		return;
	}
	else
	{
		m_planLength = randInt(4, 32);
		setYSpeed(getYSpeed() + randInt(-2, 2));
	}

}

void HolyWaterProjectile::doSomething()
{
	if (!getAlive())
	{
		return;
	}
	//check if activated

	if (getWorld()->sprayFirstAppropriateActor(this) != nullptr)
	{
		getWorld()->sprayFirstAppropriateActor(this)->beSprayed();
	}

	//holy water projectile movement algorithm
	GraphObject::moveForward(SPRITE_HEIGHT);
	m_travelDistance -= 10;
	if (getX() < 0 || getY() < 0 || getX() > VIEW_WIDTH || getY() > VIEW_HEIGHT)
	{
		setAlive(false);
		return;
	}

	if (m_travelDistance == 0)
	{
		setAlive(false);
		m_travelDistance = 160;
	}

}

//move helper function for ghost racer
void GhostRacer::move()
{
	double max_shift_per_tick = 4.0;
	int direction = getDirection();
	double delta_x = cos(direction * (3.1415 / 180)) * max_shift_per_tick;
	moveTo(getX() + delta_x, getY());
}

//public move function for Pedestrian Base class (Human and Zombie)
void Pedestrian::move()
{
	int vert_speed = getYSpeed() - getWorld()->getGhostRacer()->getYSpeed();
	int horiz_speed = getXSpeed();

	int new_y = getY() + vert_speed;
	int new_x = getX() + horiz_speed;

	moveTo(new_x, new_y);

	if (getX() < 0 || getY() < 0 || getX() > VIEW_WIDTH || getY() > VIEW_HEIGHT)
	{
		setAlive(false);
		return;
	}
}

//public move function for GhostRacerActivatedObjects so they can all access this movement algorithm
void GhostRacerActivatedObject::move()
{
	int vert_speed = getYSpeed() - getWorld()->getGhostRacer()->getYSpeed();
	int horiz_speed = getXSpeed();

	int new_y = getY() + vert_speed;
	int new_x = getX() + horiz_speed;

	moveTo(new_x, new_y);

	if (getX() < 0 || getY() < 0 || getX() > VIEW_WIDTH || getY() > VIEW_HEIGHT)
	{
		setAlive(false);
		return;
	}
}

//move helper function for zombie cab
void ZombieCab::move()
{
	int vert_speed = getYSpeed() - getWorld()->getGhostRacer()->getYSpeed();
	int horiz_speed = getXSpeed();

	int new_y = getY() + vert_speed;
	int new_x = getX() + horiz_speed;

	moveTo(new_x, new_y);

	if (getX() < 0 || getY() < 0 || getX() > VIEW_WIDTH || getY() > VIEW_HEIGHT)
	{
		setAlive(false);
		return;
	}
}

//spray function for Human Pedestrian
void HumanPedestrian::beSprayed()
{
	setXSpeed(-1*getXSpeed());
	switch (getDirection())
	{
	case 180:
		setDirection(0);
		break;
	case 0:
		setDirection(180);
		break;
	default:
		break;
	}
	getWorld()->playSound(SOUND_PED_HURT);
}

//spray function for Zombie Pedestrian
void ZombiePedestrian::beSprayed()
{
	setHitpoints(getHitpoints() - 1);
	if (getHitpoints() <= 0)
	{
		setAlive(false);
		getWorld()->playSound(SOUND_PED_DIE);
		if (getWorld()->getOverlappingGhostRacer(this) == nullptr)
		{
			int chanceHealingGoodie = randInt(0, 4);
			if (chanceHealingGoodie == 0)
			{
				getWorld()->addActor(new HealingGoodie(getWorld(), getX(), getY()));
			}
		}
		getWorld()->increaseScore(150);
	}
	else
	{
		getWorld()->playSound(SOUND_PED_HURT);
	}
}

//spray function for all GhostRacerActivated objects (they just dissapear)
void GhostRacerActivatedObject::beSprayed()
{
	setAlive(false);
}

//spray function for zombie cab
void ZombieCab::beSprayed()
{
	setHitpoints(getHitpoints() - 1);
	if (getHitpoints() <= 0)
	{
		setAlive(false);
		getWorld()->playSound(SOUND_VEHICLE_DIE);
		if (getWorld()->getOverlappingGhostRacer(this) == nullptr)
		{
			int chanceOilSlick = randInt(0, 4);
			if (chanceOilSlick == 0)
			{
				getWorld()->addActor(new OilSlick(getWorld(), getX(), getY()));
			}
		}
		getWorld()->increaseScore(200);
		return;
	}
	else
	{
		getWorld()->playSound(SOUND_VEHICLE_HURT);
	}
}