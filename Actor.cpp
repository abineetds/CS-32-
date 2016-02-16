#include "Actor.h"
#include "StudentWorld.h"
#include <iostream>
#include <cstdlib>
////////////////////ACTOR IMPLEMENTATIONS//////////////////
Actor::Actor(int imageID, int startX, int startY, StudentWorld* current, Direction dir)
	:GraphObject(imageID, startX, startY, dir), m_currWorld(current)
{
	setVisible(true);				//Applies to all objects except Exit
}
StudentWorld* Actor::getWorld() const
{
	return m_currWorld;
}

////////////////////CANDIE IMPLEMENTATIONS//////////////////
CanDie::CanDie(int imageID, int startX, int startY, StudentWorld* current, Direction dir)
	: Actor(imageID, startX, startY, current, dir), m_isAlive(true)
{}

bool CanDie::IsAlive() const
{
	return m_isAlive;
}

void CanDie::setDead()
{
	m_isAlive = false;
}

/*
the int CanHeMove function indentifies what object exists at a location and returns its code.
code key:
boulder - 1
hole- 2
jewel - 3
RHgoodie - 4
ELGoodie - 5
AmmoGoodie - 6
object(Wall/Factory) - 7
Living(Robots) - 8
Exit - 9
Bullet - 10
Player - 11
*/
int CanDie::canHeMove(int c, int r, int type)
{
	Actor* ap;
	if (type == 1)
		ap = getWorld()->FindActorAtLocation(c, r);
	else
		ap = getWorld()->FindActorAtLocation(c, r, 2);

	if (ap != nullptr)
	{
		Actor* wp = dynamic_cast<Boulder*>(ap);
		if (wp != nullptr)
		{
			return 1;
		}
		
		wp = dynamic_cast<Hole*>(ap);
		if (wp != nullptr)
		{
			return 2;
		}
		
		wp = dynamic_cast<Jewel*>(ap);
		if (wp != nullptr)
		{
			return 3;
		}

		wp = dynamic_cast<RestoreHealthObj*>(ap);
		if (wp != nullptr)
		{
			return 4;
		}

		wp = dynamic_cast<ExtraLifeObj*>(ap);
		if (wp != nullptr)
		{
			return 5;
		}

		wp = dynamic_cast<AmmoObj*>(ap);
		if (wp != nullptr)
		{
			return 6;
		}

		wp = dynamic_cast<Wall*>(ap);
		KleptoBotFactory* fp = dynamic_cast<KleptoBotFactory*>(ap);
		if (wp != nullptr || fp != nullptr)
		{
			return 7;
		}

		wp = dynamic_cast<Robot*>(ap);
		if (wp != nullptr)
		{
			return 8;
		}

		wp = dynamic_cast<Exit*>(ap);
		if (wp != nullptr)
		{
			return 9;
		}

		wp = dynamic_cast<Bullet*>(ap);
		if (wp != nullptr)
		{
			return 10;
		}

		wp = dynamic_cast<Player*>(ap);
		if (wp != nullptr)
		{
			return 11;
		}
	}

	return 0;
}

void CanDie::getNextCoord(int& currX, int& currY, Direction dir) //Returns Next Coordinates in the direction indicated
{
	switch (dir)
	{
	case left: 
		currX = currX - 1;
		break;
	case right:
		currX = currX + 1;
		break;
	case up:
		currY = currY + 1;
		break;
	case down:
		currY = currY - 1;
		break;
	}
}


////////////////////CANTAKEHTIS IMPLEMENTATIOS/////////////////////////////

CanTakeHits::CanTakeHits(int imageID, int startX, int startY, int HitPoints, StudentWorld* current, Direction dir)
	:CanDie(imageID, startX, startY, current, dir), m_HitPoints(HitPoints)
{

}

bool CanTakeHits::doSomeDamage() //Reduces hit points if an actor gets hit by a bullet
{
	m_HitPoints -= 2;
	if (m_HitPoints <= 0)		//If the actor dies, it sets it to dead
	{
		setDead();
		setVisible(false);
		return true;
	}
	return false;
}

int CanTakeHits::getHitPoints() const
{
	return m_HitPoints;
}

void CanTakeHits::setHitPoints(int points)
{
	m_HitPoints = points;
}

////////////////////PLAYER IMPLEMENTATIONS//////////////////
Player::Player(int col, int row, StudentWorld* current)
	: CanTakeHits(IID_PLAYER, col, row, 20, current, right), m_ammo(20)
{
}

void Player::doSomething()	//For Player Movement
{
	if (!IsAlive())
		return;
	int ch;
	int x = getX(), y = getY();
	if (getWorld()->getKey(ch))
	{
		switch (ch)
		{
		case KEY_PRESS_ESCAPE:
			setDead();
			break;
		case KEY_PRESS_LEFT:
			setDirection(left);
			getNextCoord(x, y, getDirection());			// Get Next Coordinate in that direction
			if (PlayerMovement(x, y, left))				// If Move poisible, make move
			{
				moveTo(x, y);
			}
			break;
		case KEY_PRESS_DOWN:
			setDirection(down);
			getNextCoord(x, y, getDirection());
			if (PlayerMovement(x, y, down))
			{
				moveTo(x, y);
			}
			break;
		case KEY_PRESS_RIGHT:
			setDirection(right);
			getNextCoord(x, y, getDirection());
			if (PlayerMovement(x, y, right))
			{
				moveTo(x, y);
			}
			break;
		case KEY_PRESS_UP:
			setDirection(up);
			getNextCoord(x, y, getDirection());
			if (PlayerMovement(x, y, up))
			{
				moveTo(x, y);
			}
			break;
		case KEY_PRESS_SPACE:	
			if (m_ammo != 0)				//if player still has ammo
			{
				m_ammo--;
				getWorld()->playSound(SOUND_PLAYER_FIRE);	
				getNextCoord(x, y, getDirection());
				getWorld()->createBullet(x, y, this);		//Create new Bullet in adjecent location
			}
			break;
		}
	}
}

bool Player::PlayerMovement(int col, int row, Direction dir)
{
	int c = canHeMove(col, row);
	if (c == 2 || c == 7 || c == 8)			//If there is a hole, object or wall, no movement
		return false;
	else if (c == 1)			// If there is a boulder, check if pushable 
	{
		Actor* ad = getWorld()->FindActorAtLocation(col, row);
		if (ad->push(dir))
			return true;
		else
			return false;
	}
	else
		return true;
}

void Player::setAmmo(int ammo)
{
	m_ammo += ammo;
}

int Player::getammo() const
{
	return m_ammo;
}
////////////////////WALL IMPLEMENTATIONS//////////////////
Wall::Wall(int col, int row, StudentWorld* current)
	:Actor(IID_WALL, col, row, current)
{

}

//////////////////BULLET IMPLEMENTATIONS////////////////////
Bullet::Bullet(int col, int row, StudentWorld* current, Direction dir)
	:CanDie(IID_BULLET, col, row, current, dir)
{
	//setVisible(true);
}

void Bullet::doSomething()
{
	if (!IsAlive())
		return;

	if (!BulletMovement(getX(), getY())) //If Bullet cannot move on current location, set dead
	{
		setDead();
	}
	else
	{
		int x = getX(), y = getY();
		getNextCoord(x, y, getDirection());		// If bullet cannot move on next location, set dead
		if (BulletMovement(x,y))
			moveTo(x,y);
		else
			setDead();
	}
}

bool Bullet::BulletMovement(int col, int row)
{
	int a = canHeMove(col, row);
	if (a == 1 || a == 8 || a == 11)  // If Robot, Boulder or Player are present at this location
	{
		Actor* it = getWorld()->FindActorAtLocation(col, row);
		if (!it->doSomeDamage())	//If Actor doesn't die
		{
			if (a == 8)
				getWorld()->playSound(SOUND_ROBOT_IMPACT);
			else if (a == 11)
				getWorld()->playSound(SOUND_PLAYER_IMPACT);
		}
		else
		{
			if (a == 8)			//If Actor does die, play respective sound
			{
				getWorld()->playSound(SOUND_ROBOT_DIE);
				RegularKleptoBot* ut = dynamic_cast<RegularKleptoBot*>(it);
				if (ut != nullptr)
				{
					ut->dropMyGoodie(); //Drop Goodie
					getWorld()->increaseScore(10);
				}
				else
					getWorld()->increaseScore(100);
			}
			else if (a == 11)
				getWorld()->playSound(SOUND_PLAYER_DIE);
		}
		return false;
	}
	else if (canHeMove(col, row) == 7)  //If wall or factory
	{
		return false;
	}
	else
	{
		return true;
	}
}

///////////////////HOLE IMPLEMENTATIONS/////////////////////////
Hole::Hole(int col, int row, StudentWorld* current)
	:CanDie(IID_HOLE, col, row, current)
{

}

void Hole::doSomething()
{

}

////////////////BOULDER IMPLEMENTATIONS/////////////////////////
Boulder::Boulder(int col, int row, StudentWorld* current)
	:CanTakeHits(IID_BOULDER, col, row, 10, current)
{
}

void Boulder::doSomething()
{

}

bool Boulder::push(Direction dir)
{
	int x = getX(), y = getY();
	getNextCoord(x, y, dir);	
	if (BoulderMovement(x, y))	//If Boulder can move to next location, move it
	{
		moveTo(x, y);
		return true;
	}
	else
		return false;
}

bool Boulder::BoulderMovement(int col, int row)
{
	if (canHeMove(col, row) == 0) // If empty Space, Move Boulder
	{
		return true;
	}
	else if (canHeMove(col, row) == 2)	//If Hole, Move Boulder, Set Hole to dead and set Boulder to dead
	{
		Actor* hp = getWorld()->FindActorAtLocation(col, row);
		hp->setDead();
		hp->setVisible(false);
		setDead();
		setVisible(false);
		return true;
	}
	else // Any other case, return false
	{
		return false;
	}

}

///////////////////////////JEWEL IMPLETATIONS//////////////////////////////
Jewel::Jewel(int col, int row, StudentWorld* current)
	:CanDie(IID_JEWEL, col, row, current)
{
	
}

void Jewel::doSomething()
{
	if (!IsAlive())
		return;
	if (getWorld()->getPlayer()->getX() == getX() && getWorld()->getPlayer()->getY() == getY())		//If Player is on the Jewel, Do required steps
	{
		getWorld()->increaseScore(50);
		getWorld()->decJewels();
		setDead();
		setVisible(false);
		getWorld()->playSound(SOUND_GOT_GOODIE);
	}
}

///////////////////////////EXTRALIFEOBJ IMPLETATIONS//////////////////////////////
ExtraLifeObj::ExtraLifeObj(int col, int row, StudentWorld* current)
	:CanDie(IID_EXTRA_LIFE, col, row, current)
{
}

void ExtraLifeObj::doSomething()
{
	if (!IsAlive())
		return;
	if (getWorld()->getPlayer()->getX() == getX() && getWorld()->getPlayer()->getY() == getY())	//If Player is on the ExtraLifeObject, Do required steps
	{
		getWorld()->increaseScore(1000);
		setDead();
		setVisible(false);
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->incLives();
	}
}

///////////////////////////RESTOREHEALTHOBJ IMPLETATIONS//////////////////////////////
RestoreHealthObj::RestoreHealthObj(int col, int row, StudentWorld* current)
	:CanDie(IID_RESTORE_HEALTH, col, row, current)
{
}

void RestoreHealthObj::doSomething()
{
	if (!IsAlive())
		return;
	if (getWorld()->getPlayer()->getX() == getX() && getWorld()->getPlayer()->getY() == getY()) //If Player is on the Restore Health Object, Do required steps
	{
		getWorld()->increaseScore(500);
		setVisible(false);
		setDead();
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->getPlayer()->setHitPoints(20);
	}
}

///////////////////////////AMMOOBJ IMPLETATIONS//////////////////////////////
AmmoObj::AmmoObj(int col, int row, StudentWorld* current)
	:CanDie(IID_AMMO, col, row, current)
{
}

void AmmoObj::doSomething()
{
	if (!IsAlive())
		return;
	if (getWorld()->getPlayer()->getX() == getX() && getWorld()->getPlayer()->getY() == getY()) //If Player is on the Ammo Goodie, Do required steps
	{
		getWorld()->increaseScore(100);
		setDead();
		setVisible(false);
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->getPlayer()->setAmmo(20);
	}
}

//////////////////////////////EXIT IMPLEMENTATIONS///////////////////////////////
Exit::Exit(int col, int row, StudentWorld* current)
	:CanDie(IID_EXIT, col, row, current)
{
	//std::cout << "hello";
	setVisible(false);
	m_isVisible = false;
}

void Exit::doSomething()
{
	if (getWorld()->getJewels() == 0) //If all jewels have been taken, Make exit visible
	{
		if (!m_isVisible)
		{
			setVisible(true);
			m_isVisible = true;
			getWorld()->playSound(SOUND_REVEAL_EXIT);
		}

		if (getWorld()->getPlayer()->getX() == getX() && getWorld()->getPlayer()->getY() == getY())	//If Player lands on Exit, Do required steps.
		{
			getWorld()->playSound(SOUND_FINISHED_LEVEL);
			getWorld()->increaseScore(2000);
			getWorld()->LevelComplete(true);
		}
	}
}

//////////////////////ROBOT IMPLEMENTATIONS////////////////////////////////

Robot::Robot(int imageID, int startX, int startY, int HitPoints, StudentWorld* current , Direction dir)
	:CanTakeHits(imageID, startX, startY, HitPoints, current, dir), m_currTicks(0)
{
	m_maxTicks = (28 - getWorld()->getLevel()) / 4;			//Get value for max number of ticks that a robot has to rest

	if (m_maxTicks < 3)
		m_maxTicks = 3;
}

void Robot::doSomething()
{
	if (!IsAlive())
		return;

	if (tickModifier())			// If Robot has to move in current Tick
	{
		if (canHeFire())
		{
			return;
		}
		else
		{
			int x = getX(), y = getY();
			getNextCoord(x, y, getDirection());
			if (canHeMove(x, y) == 1 || canHeMove(x, y) == 7 || canHeMove(x, y) == 8 || canHeMove(x, y) == 2)	//If there is an object at next location, reverse direction 
			{
				reverseDirection();
			}
			else
				moveTo(x, y);	// Move Snarlbot
		}
	}
}

void Robot::reverseDirection() // Function  to return opposite direction
{
	switch (getDirection())
	{
	case up:
		setDirection(down);
		break;
	case down:
		setDirection(up);
		break;
	case left:
		setDirection(right);
		break;
	case right:
		setDirection(left);
		break;
	}
}

bool Robot::canHeFire()
{
	int x = getX(), y = getY();
	while (true)	//Keep checking adjacent locations in given direction till either player or aberration is found
	{
		getNextCoord(x, y, getDirection());
		if (getWorld()->getPlayer()->getX() == x && getWorld()->getPlayer()->getY() == y)	//If Player found
		{
			int x1 = getX(), y1 = getY();
			getNextCoord(x1, y1, getDirection());
			getWorld()->createBullet(x1, y1, this); //Fire Bullet
			getWorld()->playSound(SOUND_ENEMY_FIRE);
			return true;
		}
		if (canHeMove(x, y) == 1 || canHeMove(x, y) == 7 || canHeMove(x, y) == 8)	// If Something else found
		{
			return false;
		}
	}
}

bool Robot::tickModifier() //adds 1 to number of Ticks at that time, or tells caller to move in current tick
{
	if (m_currTicks != m_maxTicks)
	{
		m_currTicks++;
		return false;
	}
	else
	{
		m_currTicks = 0;
		return true;
	}
}

//////////////////////HSNARLBOT IMPLEMENTATIONS////////////////////////////

HSnarlBot::HSnarlBot(int c, int r, StudentWorld* current)
	: Robot(IID_SNARLBOT, c, r, 10, current, right)
{

}

//////////////////////VSNARLBOT IMPLEMENTATIONS////////////////////////////

VSnarlBot::VSnarlBot(int c, int r, StudentWorld* current)
	: Robot(IID_SNARLBOT, c, r, 10, current, down)
{

}

/////////////////////KLEPTOBOT IMPLEMENTATIONS////////////////////////////

RegularKleptoBot::RegularKleptoBot(int c, int r, StudentWorld* current, bool angry, int hp)
	: Robot(IID_KLEPTOBOT, c, r, hp, current, right), m_haveGoodie(false), m_myGoodie(0), m_amIAngry(angry)
{
	m_distanceBeforeTurning = (rand() % 6) + 1;		//Randomize distance before turning
}

void RegularKleptoBot::doSomething()
{
	if (!IsAlive())
		return;
	if (!tickModifier())
	{
		return;
	}
	else
	{
		if (m_amIAngry)		//If it is an angry KleptoBot, see if bullet is to be fired
		{
			if (canHeFire())
			{
				return;
			}
		}


		int typeOfGoodie = canHeMove(getX(), getY(), 2);
		if (typeOfGoodie == 4 || typeOfGoodie == 5 || typeOfGoodie == 6)	// If Robot is at the same location as a goodie
		{
			int willPick = (rand() % 10) + 1;	//1 in 10 probability that he will pick it up
			if (willPick == 1)
			{
				m_haveGoodie = true;
				Actor* it = getWorld()->FindActorAtLocation(getX(), getY(), 2);
				it->setDead();		// Set Goodie to dead
				it->setVisible(false);
				getWorld()->playSound(SOUND_ROBOT_MUNCH);
				switch (typeOfGoodie)	// Mark type of goodie picked up RHgoodie - 4, ELGoodie - 5, AmmoGoodie - 6
				{
				case 4:
					m_myGoodie = 4;
					break;
				case 5:
					m_myGoodie = 5;
					break;
				case 6:
					m_myGoodie = 6;
					break;
				}
				return;
			}
		}

		bool hasMoved = false;		
		if (m_distanceBeforeTurning != 0)	//If Klepto can still move
		{
			int x = getX(), y = getY();
			getNextCoord(x, y, getDirection());
			int isMovePossible = canHeMove(x, y);
			if (isMovePossible != 1 && isMovePossible != 2 && isMovePossible != 7 && isMovePossible != 8 && isMovePossible != 11) // If nothing is obstructing its way
			{
				m_distanceBeforeTurning--;		//Move the KleptoBot
				moveTo(x, y);
				hasMoved = true;
				return;
			}
		}
		if (m_distanceBeforeTurning == 0 || !hasMoved)	// If Hasn't moved
		{
			m_distanceBeforeTurning = (rand() % 6) + 1;	//Randomize distance before turning again
			int directions[4];
			int count = 0;
			Direction newDir;
			while (count < 4)
			{
				int newdir = (rand() % 4) + 1;		//Randomize direction to turn, create an array to store the directions that have already been used
				if (!hasItBeenUsed(directions, count, newdir))		// If direction hasn't yet been used
				{
					directions[count++] = newdir;		//Save currennt direction onto the array
					setNewDir(newdir, newDir);
					int x = getX(), y = getY();
					getNextCoord(x, y, newDir);
					int isMovePossible = canHeMove(x, y);		// Check if Move is Possible and act accordingly
					if (isMovePossible != 1 && isMovePossible != 2 && isMovePossible != 7 && isMovePossible != 8 && isMovePossible != 11)
					{
						setDirection(newDir);
						moveTo(x, y);
						return;
					}
				}
			}
			setNewDir(directions[0], newDir); //Otherwise set robot direction to the one first randomized
			setDirection(newDir);
			return;
 		}
	}

}

void RegularKleptoBot::setNewDir(int newdir, Direction& newDir)	// Assign direction values to corresponding integer codes
{
	switch (newdir)
	{
	case 1:
		newDir = up;
		break;
	case 2:
		newDir = down;
		break;
	case 3:
		newDir = left;
		break;
	case 4:
		newDir = right;
		break;
	}
}

bool RegularKleptoBot::hasItBeenUsed(int a[], int n, int item)	//Check if direction has already been used
{
	for (int i = 0; i < n; i++)
	{
		if (a[i] == item)
		{
			return true;
		}
	}
	return false;
}
			
void RegularKleptoBot::dropMyGoodie()
{
	if (m_haveGoodie)
	{
		getWorld()->createGoodie(getX(), getY(), m_myGoodie);
	}
}

/////////////////////////ANGRYKLEPTOBOT IMPLEMENTATIONS////////////////////////////////

AngryKleptoBot::AngryKleptoBot(int c, int r, StudentWorld* current)
	: RegularKleptoBot(c, r, current, true, 8)
{

}

/////////////////////////KLEPTOBOTFACTORY IMPLEMENTATIONS//////////////////////////////

KleptoBotFactory::KleptoBotFactory(int c, int r, StudentWorld* curr, int type)
	:Actor(IID_ROBOT_FACTORY, c, r, curr), m_type(type)
{

}

void KleptoBotFactory::doSomething()
{
	int kleptoCount = 0;
	bool botOnFactory = false;
	for (int c = getX() - 3; c <= getX() + 3; c++)		//Count the number of KleptoBots in the 7 x 7 square around the factory
	{
		for (int r = getY() - 3; r <= getY() + 3; r++)
		{
			if (c >= 0 && c < VIEW_WIDTH && r >= 0 && r < VIEW_HEIGHT)
			{
				Actor* ap = getWorld()->FindActorAtLocation(c, r);
				RegularKleptoBot* up = dynamic_cast<RegularKleptoBot*>(ap);
				if (up != nullptr)
				{
					kleptoCount++;
					if (c == getX() && r == getY())		//Check if there is a Kleptobot on the factory
					{
						botOnFactory = true;
					}
				}
			}
		}
	}

	if (kleptoCount < 3 && !botOnFactory)		//If There is no Bot on factory and count is less than 3
	{
		int a = (rand() % 50) + 1;
		if (a == 1)
		{
			getWorld()->playSound(SOUND_ROBOT_BORN);
			getWorld()->createKlepto(getX(), getY(), m_type);
		}
	}
}
