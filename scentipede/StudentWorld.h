#ifndef _StudentWorld_H_
#define _StudentWorld_H_

#include <vector>
#include "actor.h"
#include "GameWorld.h"

using namespace std;

class GameObject;

class StudentWorld : public GameWorld
{
public:
	StudentWorld::StudentWorld();
	~StudentWorld();
	virtual void init();
	virtual int move();
	virtual void cleanUp();

	//Checking for objects
	bool isPoisonMushroom(int x, int y);
	bool isMushroom(int x, int y); 
	bool isInsect(int x, int y);
	bool isPlayer(int x, int y);
	bool isScentSeg(int x, int y);

	//Object functions
	void createObj(GameObject* temp); 
	void attackObj(int x, int y); 
	void removeObj(int x, int y);
	void createMushroom(int x, int y); 
	void poisonMushroom(int x, int y);

	//Adding insects
	void addInsects();
	void addScentipede();
	void addFlea();
	void addScorpion();
	void addSpider();
private:
	vector<GameObject*> m_actor;
	int m_counter;
	int randInt(int lowest, int highest); 
};

#endif // #ifndef _StudentWorld_H_