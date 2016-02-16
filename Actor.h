#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
class StudentWorld;

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class Actor : public GraphObject
{
	StudentWorld* m_currWorld;
public:
	Actor(int imageID, int startX, int startY, StudentWorld* current = nullptr, Direction dir = none);
	StudentWorld* getWorld() const;
	virtual void doSomething() = 0;
	virtual bool IsAlive() const { return true; }
	virtual bool doSomeDamage(){ return true; }
	virtual bool push(Direction dir) { return false; }
	virtual void setDead(){}
	virtual void setHitPoints(int points){}
	virtual int getHitPoints() const { return 0; }
	virtual int getammo() const { return 0; }
	virtual void setAmmo(int ammo) {}
	virtual ~Actor(){}
};

class CanDie : public Actor
{
	bool m_isAlive;
public:
	CanDie(int imageID, int startX, int startY, StudentWorld* current = nullptr, Direction dir = none);
	virtual bool IsAlive() const;
	virtual	void setDead();
	virtual int canHeMove(int c, int r, int type = 1);
	virtual void doSomething() = 0;
	virtual bool doSomeDamage(){ return true; }
	virtual void setHitPoints(int points){}
	virtual bool push(Direction dir){ return false; }
	virtual void setAmmo(int ammo){}
	virtual int getHitPoints() const { return 0; }
	virtual int getammo() const{ return 0; }
	virtual ~CanDie(){}
	void getNextCoord(int& currX, int& currY, Direction dir);
};

class CanTakeHits : public CanDie
{
	int m_HitPoints;
public:
	CanTakeHits(int imageID, int startX, int startY, int HitPoints, StudentWorld* current = nullptr, Direction dir = none);
	virtual void doSomething()=0;
	bool doSomeDamage();
	virtual bool push(Direction dir){return true;}
	void setHitPoints(int points);
	virtual void setAmmo(int ammo){};
	int getHitPoints() const;
	virtual int getammo() const{ return 0; }
	virtual ~CanTakeHits(){}
};

class Player : public CanTakeHits
{
	int m_ammo;
public:
	Player(int col, int row, StudentWorld* current);
	virtual void doSomething();
	bool PlayerMovement(int col, int row, Direction dir);
	void setAmmo(int ammo);
	int getammo() const;
	virtual ~Player(){}
};

class Boulder : public CanTakeHits
{
public:
	Boulder(int col, int row, StudentWorld* current);
	virtual void doSomething();
	bool push(Direction dir);
	bool BoulderMovement(int col, int row);
	virtual ~Boulder(){}
};

class Robot : public CanTakeHits
{
	int m_maxTicks;
	int m_currTicks;
public:
	Robot(int imageID, int startX, int startY, int HitPoints, StudentWorld* current = nullptr, Direction dir = none);
	virtual void doSomething();
	bool canHeFire();
	void reverseDirection();
	bool tickModifier();
};

class RegularKleptoBot : public Robot
{
	int m_distanceBeforeTurning;
	bool m_haveGoodie;
	int m_myGoodie;
	bool m_amIAngry;
public:
	RegularKleptoBot(int c, int r, StudentWorld* curr, bool angry = false, int hp = 5);
	virtual void doSomething();
	bool hasItBeenUsed(int a[], int n, int item);
	void setNewDir(int newdir, Direction& newDir);
	void dropMyGoodie();
};

class AngryKleptoBot : public RegularKleptoBot
{
public:
	AngryKleptoBot(int c, int r, StudentWorld* curr);
};

class HSnarlBot : public Robot
{
public:
	HSnarlBot(int c, int r, StudentWorld* current);
};

class VSnarlBot : public Robot
{
public:
	VSnarlBot(int c, int r, StudentWorld* current);
};

class KleptoBotFactory : public Actor
{
	int m_type;
public:
	KleptoBotFactory(int c, int r, StudentWorld* current, int type);
	virtual void doSomething();
};

class Hole : public CanDie
{
public:
	Hole(int col, int row, StudentWorld* current);
	virtual void doSomething();
	virtual ~Hole(){}
};

class Wall : public Actor
{
public:
	Wall(int c, int r, StudentWorld* current);
	void doSomething(){}
	virtual ~Wall(){}
};

class Jewel : public CanDie
{
public:
	Jewel(int c, int r, StudentWorld* current);
	void doSomething();
	virtual ~Jewel(){}
};

class ExtraLifeObj : public CanDie
{
public:
	ExtraLifeObj(int c, int r, StudentWorld* current);
	void doSomething();
	virtual ~ExtraLifeObj(){}
};

class RestoreHealthObj : public CanDie
{
public:
	RestoreHealthObj(int c, int r, StudentWorld* current);
	void doSomething();
	virtual ~RestoreHealthObj(){}
};

class AmmoObj : public CanDie
{
public:
	AmmoObj(int c, int r, StudentWorld* current);
	void doSomething();
	virtual ~AmmoObj(){}
};

class Exit : public CanDie
{
	bool m_isVisible;
public:
	Exit(int c, int r, StudentWorld* current);
	void doSomething();
	virtual ~Exit(){}
};


class Bullet : public CanDie
{
public:
	Bullet(int col, int row, StudentWorld* current, Direction dir);
	virtual void doSomething();
	bool BulletMovement(int col, int row);
	virtual ~Bullet(){}
};


#endif // ACTOR_H_
