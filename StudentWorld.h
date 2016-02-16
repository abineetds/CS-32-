#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Actor.h"
#include "GameConstants.h"
#include "Level.h"
#include <list>
#include <string>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
	std::list<Actor*> m_actors;
	Actor* m_player;
	int m_bonus;
	int m_jewels;
	bool m_levelComplete;
	bool m_gameFinished;
	bool m_badFormat;
	
public:
	StudentWorld(std::string assetDir)
	 : GameWorld(assetDir)
	{
	}

	Actor* FindActorAtLocation(int c, int r, int type = 1);
	
	void LoadLevel();
	virtual int init();
	
	virtual int move();

	virtual void cleanUp();
	void decJewels(){ m_jewels--;}

	Actor* getPlayer() const;
	int getJewels() const;
	void createBullet(int c, int r, Actor* current);
	void createGoodie(int c, int r, int type);
	void createKlepto(int c, int r, int type);
	void setDisplayText();
	void LevelComplete(bool input);
	~StudentWorld();
};

#endif // STUDENTWORLD_H_
