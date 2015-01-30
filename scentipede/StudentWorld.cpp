#include <vector>
#include "actor.h"
#include "StudentWorld.h"
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <iostream> //for testing purposes (cerr)

using namespace std;

StudentWorld::StudentWorld()
	: m_counter(0)
{}

StudentWorld::~StudentWorld()
{
	//make sure nothing is left over
	for (int i = 0; i<m_actor.size(); i++)
		delete m_actor.at(i);
	m_actor.clear(); 
}

void StudentWorld::init()
{
	srand(static_cast<unsigned int>(time(0)));

	Player* playerShip = new Player(this); 
	m_actor.push_back(playerShip);

	int numMushrooms;
	if (testParamsProvided())
		numMushrooms = getTestParam(TEST_PARAM_STARTING_MUSHROOMS);
	else
		numMushrooms = 25;

	for (int i = 0; i<numMushrooms; i++)
	{
		int randx = randInt(0, GARDEN_WIDTH - 1);
		int randy = randInt(1, GARDEN_HEIGHT - 2);

		while (isMushroom(randx, randy)) 
		{
			randx = randInt(0, GARDEN_WIDTH - 1);
			randy = randInt(1, GARDEN_HEIGHT - 2);
		}

		Mushroom* mushroom = new Mushroom(this, randx, randy); 
		m_actor.push_back(mushroom);
	}
}

int StudentWorld::move()
{
	m_counter++;
	addInsects();

	//Everyone who is alive takes some action
	for (int i = 0; i<m_actor.size(); i++)
		if (m_actor.at(i)->isAlive())
			m_actor.at(i)->Action();

	bool playerIsDead = false; 

	//Remove dead objects
	for (int i = 0; i<m_actor.size(); i++)
		if (!m_actor.at(i)->isAlive())
		{
			if (m_actor.at(i)->getID() == IID_PLAYER)
				playerIsDead = true;

			delete m_actor.at(i);

			//Remove element from vector
			m_actor.erase(m_actor.begin() + i);
			i--;

		}

	if (playerIsDead)
		return GWSTATUS_PLAYER_DIED;

	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
	for (int i = 0; i<m_actor.size(); i++)
		delete m_actor.at(i); 
	m_actor.clear();
}

void StudentWorld::createObj(GameObject* obj)
{
	m_actor.push_back(obj); 
}

//Check the object being attacked, increment score, and destroy object
void StudentWorld::attackObj(int x, int y)
{
	for (int i = 0; i<m_actor.size(); i++)
	{
		int s_x, s_y;
		GameObject* target = m_actor.at(i);

		target->getLocation(s_x, s_y);
		if (s_x == x && s_y == y)
		{
			if (target->getID() == IID_MUSHROOM || target->getID() == IID_POISON_MUSHROOM)
			{
				increaseScore(1);
				((Mushroom*)target)->decreaseHealth(); 
			}
			else
			{
				int id = target->getID();
				switch (id)
				{
					case IID_SCENTIPEDE_SEGMENT:
						increaseScore(10);
						break;
						
					case IID_FLEA:
						increaseScore(50);
						break;

					case IID_SPIDER:
						increaseScore(20);
						break;

					case IID_SCORPION:
						increaseScore(25);
						break;
				}

				target->setDead();
			}
		}
	}
}

void StudentWorld::removeObj(int x, int y)
{
	int s_x, s_y;
	for (int i = 0; i<m_actor.size(); i++)
	{
		m_actor.at(i)->getLocation(s_x, s_y);
		if (s_x == x && s_y == y)
			m_actor.at(i)->setDead();
	}
}

//check if the object is a mushroom
bool StudentWorld::isMushroom(int x, int y)
{
	for (int i = 0; i<m_actor.size(); i++)
	{
		int id = m_actor.at(i)->getID();

		if (id == IID_MUSHROOM || id == IID_POISON_MUSHROOM)
		{
			int mush_x, mush_y;
			m_actor.at(i)->getLocation(mush_x, mush_y);
			if (x == mush_x && y == mush_y) 
				return true;
		}
	}
	return false;
}

//check if the object is a poison mushroom
bool StudentWorld::isPoisonMushroom(int x, int y)
{
	for (int i = 0; i<m_actor.size(); i++)
	{
		int id = m_actor.at(i)->getID();

		if (id == IID_POISON_MUSHROOM)
		{
			int pmush_x, pmush_y;
			m_actor.at(i)->getLocation(pmush_x, pmush_y);
			if (x == pmush_x && y == pmush_y) 
				return true;
		}
	}
	return false;
}

//check if the object is an insect
bool StudentWorld::isInsect(int x, int y)
{
	for (int i = 0; i<m_actor.size(); i++)
	{
		int id = m_actor.at(i)->getID();

		if (id == IID_SCENTIPEDE_SEGMENT || id == IID_SPIDER || id == IID_FLEA || id == IID_SCORPION) 
		{
			int insect_x, insect_y;
			m_actor.at(i)->getLocation(insect_x, insect_y);
			if (x == insect_x && y == insect_y) 
				return true;
		}
	}
	return false;
}

//check if the object is the player
bool StudentWorld::isPlayer(int x, int y)
{
	for (int i = 0; i<m_actor.size(); i++)
	{
		int id = m_actor.at(i)->getID();

		if (id == IID_PLAYER) 
		{
			int player_x, player_y;
			m_actor.at(i)->getLocation(player_x, player_y);
			if (x == player_x && y == player_y)
				return true;
		}
	}
	return false;
}

//check if the object is a scentipede segment
bool StudentWorld::isScentSeg(int x, int y)
{
	for (int i = 0; i<m_actor.size(); i++)
	{
		int id = m_actor.at(i)->getID();

		if (id == IID_SCENTIPEDE_SEGMENT)
		{
			int scent_x, scent_y;
			m_actor.at(i)->getLocation(scent_x, scent_y); 
			if (x == scent_x && y == scent_y) 
				return true;
		}
	}
	return false;
}

void StudentWorld::createMushroom(int x, int y)
{
	Mushroom* mush = new Mushroom(this, x, y); 
	m_actor.push_back(mush);
}


void StudentWorld::poisonMushroom(int x, int y)
{
	int s_x, s_y;
	for (int i = 0; i<m_actor.size(); i++)
	{
		m_actor.at(i)->getLocation(s_x, s_y);
		if (s_x == x && s_y == y && m_actor.at(i)->getID() == IID_MUSHROOM) 
			m_actor.at(i)->setID(IID_POISON_MUSHROOM); 
	}
}


void StudentWorld::addInsects()
{

	if (testParamsProvided() && m_counter>0)
	{
		if (getTestParam(TEST_PARAM_CENT_CHANCE_INDEX) > 0 && m_counter % getTestParam(TEST_PARAM_CENT_CHANCE_INDEX) == 0)
			addScentipede(); 
		if (getTestParam(TEST_PARAM_SPIDER_CHANCE_INDEX) > 0 && m_counter % getTestParam(TEST_PARAM_SPIDER_CHANCE_INDEX) == 0)
			addSpider(); 
		if (getTestParam(TEST_PARAM_FLEA_CHANCE_INDEX) > 0 && m_counter % getTestParam(TEST_PARAM_FLEA_CHANCE_INDEX) == 0)
			addFlea();
		if (getTestParam(TEST_PARAM_SCORPION_CHANCE_INDEX) > 0 && m_counter % getTestParam(TEST_PARAM_SCORPION_CHANCE_INDEX) == 0)
			addScorpion(); 
	}
	else
	{
		int level = getPlayerScore() / 500; 
		int numScentipede = 0;
		int numFlea = 0;
		int numSpider = 0;
		int numScorpion = 0;
		bool isTopRowScentipede = false;

		for (int i = 0; i<m_actor.size(); i++)
		{
			int id = m_actor.at(i)->getID();

			switch (id)
			{
			case IID_SCENTIPEDE_SEGMENT:
				numScentipede++;
				if (isTopRowScentipede == false)
				{
					int x, y;
					m_actor.at(i)->getLocation(x, y);

					if (y == GARDEN_HEIGHT - 1)
						isTopRowScentipede = true;
				}
				break;

			case IID_SPIDER:
				numSpider++;
				break;

			case IID_FLEA:
				numFlea++;
				break;

			case IID_SCORPION:
				numScorpion++;
				break;
			}
		}

		if (numScentipede < min(20, 3 * (level + 1)) && !isTopRowScentipede)
		{
			if (numScentipede == 0)
			{
				if (randInt(1, 20) == 1)
				{
					addScentipede();
				}
			}
			else if (randInt(1, max(300 - 30 * level, 50)) == 1)
			{
				addScentipede();
			}
		}

		if (numFlea + numSpider + numScorpion < level / 2 + 1)
		{
			if (randInt(1, max(300 - level * 30, 100)) == 1)
			{
				addFlea();
			}
			else if (randInt(1, max(350 - level * 30, 100)) == 1)
			{
				addSpider();
			}
			else if (randInt(1, max(400 - level * 30, 100)) == 1)
			{
				addScorpion();
			}
		}
	}
}

void StudentWorld::addScentipede()
{
	int numToAdd = randInt(6, 12); //randomly determine how many segments (6-12) to add

	for (int i = numToAdd - 1; i >= 0; i--)
	{
		Scentipede* s = new Scentipede(this, i, GARDEN_HEIGHT - 1); //for each segment place it in the right spot, head first
		m_actor.push_back(s);
	}
}

void StudentWorld::addFlea()
{
	int x = randInt(0, GARDEN_WIDTH - 1);
	int y = GARDEN_HEIGHT - 1;
	Flea* f = new Flea(this, x, y); //add a flea where it has any x coordinate at the very top
	m_actor.push_back(f);

}

void StudentWorld::addScorpion()
{
	int x = 0;
	int y = randInt(4, GARDEN_HEIGHT - 1);
	Scorpion* s = new Scorpion(this, x, y); //add a scorpion where it has any y coordinate above 3 at the very left
	m_actor.push_back(s);
}

void StudentWorld::addSpider()
{
	int x = 0;
	if (randInt(1, 2) == 2)
		int x = GARDEN_WIDTH - 1;
	int y = randInt(5, GARDEN_HEIGHT - 1); //changed due to new specs

	Spider* s = new Spider(this, x, y); //add a spider starting either on the left or right side, with y coordinate above 3
	m_actor.push_back(s);
}

int StudentWorld::randInt(int lowest, int highest)
{
	if (highest < lowest)
		swap(highest, lowest); //organize the numbers in times of size
	return lowest + (rand() % (highest - lowest + 1)); //return the corresponding random integer
}