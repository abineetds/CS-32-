#include "StudentWorld.h"
#include <string>
#include <algorithm>
#include <sstream>
#include <iomanip>
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

int StudentWorld::init() //Initialize values to all variables
{
	m_bonus = 1000;
	m_levelComplete = false;
	m_jewels = 0;
	m_badFormat = false;
	m_gameFinished = false;
	LoadLevel();
	if (m_gameFinished)
		return GWSTATUS_PLAYER_WON;
	else if (m_badFormat)
		return GWSTATUS_LEVEL_ERROR;
	else
		return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{	
	m_player->doSomething();		//Give Player a chance tto do something

	list<Actor*>::iterator it = m_actors.begin();
	while (it != m_actors.end())
	{
			if ((*it)->IsAlive())			//Give alll living actors a chance to do somet
			{
				(*it)->doSomething();
				it++;
			}								//Otherwise, Delete the dead actor
			else
			{
				delete (*it);
				it = m_actors.erase(it);
			}
			if (!m_player->IsAlive())		//if player has died, decrease lives of player and stop game
			{
				decLives();
				return GWSTATUS_PLAYER_DIED;
			}
			if (m_levelComplete)			//if player has landed on exit, increase score and proceed to next level
			{
				increaseScore(m_bonus);
				return GWSTATUS_FINISHED_LEVEL;
			}
	}
	if (m_bonus > 0)		//Reduce bonus by 1
		m_bonus--;
	setDisplayText();		//Set Display text
	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::setDisplayText()		//Use stringstreams to add status bar to the top of the game, updated after every teck
{
	int score = getScore();
	int level = getLevel();
	unsigned int bonus = m_bonus;
	int livesLeft = getLives();

	ostringstream statusbar;
	statusbar << "Score: ";
	statusbar.fill('0');
	statusbar << setw(7) << score;
	statusbar << "  Level: ";

	statusbar.fill('0');
	statusbar << setw(2) << getLevel();

	statusbar << "  Lives: " << livesLeft;
	statusbar << "  Health: " << (static_cast<double>(m_player->getHitPoints() / 20.00)) * 100 << "%  Ammo: " << m_player->getammo();
	statusbar << "  Bonus: ";
	statusbar.fill(' ');
	statusbar << setw(4) << m_bonus;

	string s = statusbar.str();
	setGameStatText(s);
}

void StudentWorld::cleanUp()
{
	delete m_player;		//Delete Player
	m_player = nullptr;
	list<Actor*>::iterator p = m_actors.begin();
	while ( p != m_actors.end())					//Delete all actors individually
	{
		delete (*p);
		p = m_actors.erase(p);
	}

}

StudentWorld::~StudentWorld()
{
	cleanUp();
}

Actor* StudentWorld::FindActorAtLocation(int c, int r, int type)		//There  are 2 types of findActorAtLocation, one thatsearches from the front and the other from the back
{
	if (m_player->getX() == c && m_player->getY() == r)					//If player is present at the location, return him
		return m_player;
	if (type == 1)
	{
		for (list<Actor*>::iterator p = m_actors.begin(); p != m_actors.end(); p++)		//Find Actor from the beginning and check if it's coordinates match
		{
			if ((*p)->getX() == c && (*p)->getY() == r)
			{
				return (*p);
			}
		}
	}
	else
	{
		for (list<Actor*>::iterator p = m_actors.end(); p != m_actors.begin(); p--)		//Find Actor from the end and check if it's coordinates match
		{
			if (p == m_actors.end())
				p--;

			if ((*p)->getX() == c && (*p)->getY() == r)
			{
				return (*p);
			}
		}
	}
	return nullptr;
}

void StudentWorld::LoadLevel()
{
	Level lev(assetDirectory());				//Use stringstreams to let program load next level
	ostringstream level;
	level << "level";
	if (getLevel() >= 0 && getLevel() <= 9)
		level << "0" << getLevel();
	else
		level << getLevel();
	level << ".dat";
	string finalLevel = level.str();
	Level::LoadResult result = lev.loadLevel(finalLevel);
	if (result == Level::load_fail_file_not_found || getLevel() > 99)
	{
		//cerr << "Could not find level00.dat data file\n";
		m_gameFinished = true;
	}
	else if (result == Level::load_fail_bad_format)
	{
		//cerr << "Your level was improperly formatted\n";
		m_badFormat = true;
	}
	else if (result == Level::load_success)
	{
		cerr << "Successfully loaded level\n";
		for (int c = 0; c < VIEW_WIDTH; c++)
			for (int r = 0; r < VIEW_HEIGHT; r++)
			{
			/*
			ALL MOVING OBJECTS THAT HAVE TO BE HIT BY BULLETS ARE PUSHED FRONT(except walls and factories)
			ALL STATIONARY OBJECTS THAT BULLETS HAVE TO PASS THROUGH ARE PUSHED BACK
			*/
				Level::MazeEntry ge = lev.getContentsOf(c, r); 
				switch (ge)
				{
				case Level::empty:
				//	cout << "Location 5,10 is empty\n";
					break;
				case Level::boulder:
					m_actors.push_front(new Boulder(c, r, this));
					break;
				case Level::jewel:
					m_actors.push_back(new Jewel(c, r, this));
					m_jewels++;
					break;
				case Level::extra_life:
					m_actors.push_back(new ExtraLifeObj(c, r, this));
					break;
				case Level::restore_health:
					m_actors.push_back(new RestoreHealthObj(c, r, this));
					break;
				case Level::ammo:
					m_actors.push_back(new AmmoObj(c, r, this));
					break;
				case Level::hole:
					m_actors.push_back(new Hole(c, r, this));
					break;
				case Level::exit:
					m_actors.push_back(new Exit(c, r, this));
				//	cout << "Location 5,10 is where the exit is\n";
					break;
				case Level::player:
					m_player = new Player(c, r, this);
				//	cout << "Location 5,10 is where the player starts\n";
					break;
				case Level::horiz_snarlbot:
					m_actors.push_front(new HSnarlBot(c, r, this));
					//	cout << "Location 5,10 starts with a horiz. SnarlBot\n";
					break;
				case Level::vert_snarlbot:
					m_actors.push_front(new VSnarlBot(c, r, this));
				//	cout << "Location 5,10 starts with a vertical SnarlBot\n";
					break;
				case Level::kleptobot_factory:
					m_actors.push_back(new KleptoBotFactory(c, r, this, 1));
				//	cout << "Location 5,10 holds a KleptoBot Factory\n";
					break;
				case Level::angry_kleptobot_factory:
					m_actors.push_back(new KleptoBotFactory(c, r, this, 2));
				//	cout << "Location 5,10 holds an Angry KleptoBot Factory\n";
					break;
				case Level::wall:
					m_actors.push_back(new Wall(c, r, this));
				//	cout << "Location 5,10 holds a Wall\n";
					break;
				}
			}
	}
}

void StudentWorld::createBullet(int c, int r, Actor* current)
{
	m_actors.push_back(new Bullet(c, r, this, current->getDirection()));
}


Actor* StudentWorld::getPlayer() const
{
	return m_player;
}

int StudentWorld::getJewels() const
{
	return m_jewels;
}

void StudentWorld::LevelComplete(bool input)
{
	m_levelComplete = input;
}

void StudentWorld::createGoodie(int c, int r, int type)		// Create's goodie of specified type, 4: Restore Health, 5: Extra Life, 6: Ammo
{
	switch (type)
	{
	case 4:
		m_actors.push_back(new RestoreHealthObj(c, r, this));
		break;
	case 5:
		m_actors.push_back(new ExtraLifeObj(c, r, this));
		break;
	case 6:
		m_actors.push_back(new AmmoObj(c, r, this));
		break;
	}
}

void StudentWorld::createKlepto(int c, int r, int type)		//Creates KleptoBot of appropriate type, 1: Regular, 2: Angry
{
	switch (type)
	{
	case 1:
		m_actors.push_front(new RegularKleptoBot(c, r, this));
		break;
	case 2:
		m_actors.push_front(new AngryKleptoBot(c, r, this));
	}
}
