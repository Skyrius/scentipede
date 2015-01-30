#ifndef _ACTOR_H_
#define _ACTOR_H_

#include "GraphObj.h"
#include "GameConstants.h"
#include "StudentWorld.h"
#include "GameWorld.h"

class StudentWorld;

class GameObject : public GraphObject
{
public:
	GameObject(int id, StudentWorld* world);
	GameObject(int id, StudentWorld* world, int x, int y);
	virtual ~GameObject();
	virtual void Action() = 0;
	virtual bool isAlive();
	virtual void setDead();
	StudentWorld* getWorld() const;
	int randInt(int lowest, int highest); 
private:
	StudentWorld* myWorld;
	bool m_alive;
};

//////////////////////////////////////////////////

class Player : public GameObject
{
public:
	Player(StudentWorld* world);
	~Player();
	virtual void Action();
	void movePlayer(int dir);
private:
	bool shootPhase;
};

//////////////////////////////////////////////////

class Mushroom : public GameObject
{
public:
	Mushroom(StudentWorld* world, int x, int y);
	~Mushroom();
	virtual void Action();
	int getHealth() const;
	void decreaseHealth();
private:
	int m_health;
};

//////////////////////////////////////////////////

class WaterDroplet : public GameObject
{
public:
	WaterDroplet(StudentWorld* world, int x, int y);
	~WaterDroplet();
	virtual void Action();
private:
	int m_ticks;
};

//////////////////////////////////////////////////

class Insect : public GameObject
{
public:
	Insect(int id, StudentWorld* world, int x, int y);
	virtual ~Insect();
	virtual void Action() = 0;
private:
};

//////////////////////////////////////////////////

class Scentipede : public Insect
{
public:
	Scentipede(StudentWorld* world, int x, int y);
	~Scentipede();
	virtual void Action();
	virtual void setDead();
private:
	bool moveDown;
	bool moveRight;
	bool isPoisoned;
};

//////////////////////////////////////////////////

class Flea : public Insect
{
public:
	Flea(StudentWorld* world, int x, int y);
	~Flea();
	virtual void Action();
private:
};

//////////////////////////////////////////////////

class Spider : public Insect
{
public:
	Spider(StudentWorld* world, int x, int y);
	~Spider();
	virtual void Action();
private:
	bool actionRest;
	int m_distance;
	bool moveDown;
	bool moveRight;
};

//////////////////////////////////////////////////

class Scorpion : public Insect
{
public:
	Scorpion(StudentWorld* world, int x, int y);
	~Scorpion();
	virtual void Action();
private:
};

//////////////////////////////////////////////////

#endif // #ifndef _ACTOR_H_
