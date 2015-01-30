#ifndef _GAMEWORLD_H_

#define _GAMEWORLD_H_

#include "GameConstants.h"

class GameController;

class GameWorld
{
public:

	virtual ~GameWorld()
	{
		// nothing to delete here
	}
	
	void setMyController(GameController *myController)
	{
		m_myController = myController;
	}

	void setTestParams(int testParams[])
	{
		for (int i=0;i<NUM_TEST_PARAMS;i++)
			m_testParams[i] = testParams[i];
	}


	virtual void init() = 0;
	virtual int move() = 0;
	virtual void cleanUp() = 0;

	void increaseScore(unsigned int howMuch);
	unsigned int getPlayerScore();
	unsigned int getPlayerLives();
	void decPlayerLives();
	bool getKey(int &value);
	int getTestParam(int paramID) const;
	bool testParamsProvided() const;

private:

	GameController *myController() 
	{ 
		return m_myController;
	}


private:
	GameController *m_myController;
	int				m_testParams[NUM_TEST_PARAMS];
};

#endif // #ifndef _GAMEWORLD_H_
