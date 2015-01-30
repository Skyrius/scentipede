#include "GameController.h"

GC_STATE				GameController::m_gameState;
GC_STATE				GameController::m_nextState;
char					GameController::m_lastKeyHit;
int						GameController::m_lastSpecialKeyHit;
GameWorld				*GameController::m_gc;
std::string 			GameController::m_mainMessage;
std::string 			GameController::m_secondMessage;
double					GameController::m_aspectRatio, GameController::m_width, GameController::m_height;
int						GameController::m_curIntraFrameTick;
unsigned int			GameController::m_score;
unsigned int			GameController::m_playerLives;
int						GameController::m_timeToPlaySound;
