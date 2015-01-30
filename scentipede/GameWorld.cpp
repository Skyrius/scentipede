#include "GameWorld.h"
#include "GameController.h"

void GameWorld::increaseScore(unsigned int howMuch)
{
	m_myController->increaseScore(howMuch);
}

bool GameWorld::getKey(int &value)
{
	return m_myController->getLastKey(value);
}

unsigned int GameWorld::getPlayerLives()
{
	return m_myController->getPlayerLives();
}

void GameWorld::decPlayerLives()
{
	return m_myController->decPlayerLives();
}

unsigned int GameWorld::getPlayerScore()
{
	return m_myController->getScore();
}

int GameWorld::getTestParam(int paramID) const
{
	if (paramID >= NUM_TEST_PARAMS)
		return 0;

	return m_testParams[paramID];
}

bool GameWorld::testParamsProvided() const
{
	for (int i=0;i<NUM_TEST_PARAMS;i++)
	{
		if (m_testParams[i])
			return true;
	}

	return false;
}
