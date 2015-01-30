#include "actor.h"
#include "StudentWorld.h"
#include "GameController.h"

#include <iostream> 

GameObject::GameObject(int id, StudentWorld* world)
	: GraphObject(id), myWorld(world), m_alive(true)
{
	displayMe(true);
}

GameObject::GameObject(int id, StudentWorld* world, int x, int y)
	: GraphObject(id), myWorld(world), m_alive(true)
{
	setInitialLocation(x, y); 
	displayMe(true);
}

GameObject::~GameObject()
{
	displayMe(false);
}

bool GameObject::isAlive()
{
	return m_alive; 
}

void GameObject::setDead()
{
	m_alive = false; 
}

StudentWorld* GameObject::getWorld() const
{
	return myWorld;
}

int GameObject::randInt(int lowest, int highest)
{
	if (highest < lowest)
		swap(highest, lowest);
	return lowest + (rand() % (highest - lowest + 1)); 
}

//////////////////////////////////////////////////

Player::Player(StudentWorld* world)
	: GameObject(IID_PLAYER, world, 15, 0)
{
	shootPhase = true; 
}

Player::~Player()
{}

void Player::Action()
{
	int key;
	StudentWorld *student_world = getWorld();

	if (student_world->getKey(key))
	{
		switch (key)
		{
		case KEY_PRESS_SPACE:
			//If the player can shoot
			if (shootPhase)
			{

				int x, y;
				getLocation(x, y);

				int dropX = x;
				int dropY = y + 1;

				while (dropX <= GARDEN_WIDTH - 1 && dropY <= GARDEN_HEIGHT - 1)
				{
					if (getWorld()->isInsect(dropX, dropY) || getWorld()->isMushroom(dropX, dropY))
					{
						getWorld()->attackObj(dropX, dropY);
						break;
					}
					else
					{
						WaterDroplet* water = new WaterDroplet(getWorld(), dropX, dropY);
						getWorld()->createObj(water);
						dropY++; 
					}
				}
				//Can only shoot once every two rounds
				shootPhase = false; 
			}
			else
				shootPhase = true;
			break;
		default:
			movePlayer(key); 
			shootPhase = true;
			break;
		}
	}
	else
		shootPhase = true;

	int move_x, move_y;
	getLocation(move_x, move_y);

	//if player moved onto an insect
	if (getWorld()->isInsect(move_x, move_y))
		setDead();

}

void Player::movePlayer(int dir)
{
	int curr_x, curr_y;
	getLocation(curr_x, curr_y); 

	//move if within bounds
	if (dir == KEY_PRESS_LEFT_ARROW && curr_x != 0) 
	{
		if (getWorld()->isInsect(curr_x - 1, curr_y))
			setDead();

		if (!getWorld()->isMushroom(curr_x - 1, curr_y))
			moveTo(curr_x - 1, curr_y);
	}
	else if (dir == KEY_PRESS_RIGHT_ARROW && curr_x != GARDEN_WIDTH - 1)
	{
		if (getWorld()->isInsect(curr_x + 1, curr_y))
			setDead();

		if (!getWorld()->isMushroom(curr_x + 1, curr_y))
			moveTo(curr_x + 1, curr_y);
	}
	else if (dir == KEY_PRESS_UP_ARROW && curr_y != 3)
	{
		if (getWorld()->isInsect(curr_x, curr_y + 1))
			setDead();

		if (!getWorld()->isMushroom(curr_x, curr_y + 1))
			moveTo(curr_x, curr_y + 1);
	}
	else if (dir == KEY_PRESS_DOWN_ARROW && curr_y != 0)
	{
		if (getWorld()->isInsect(curr_x, curr_y - 1))
			setDead();

		if (!getWorld()->isMushroom(curr_x, curr_y - 1))
			moveTo(curr_x, curr_y - 1);
	}
}

//////////////////////////////////////////////////

Mushroom::Mushroom(StudentWorld* world, int x, int y)
	: GameObject(IID_MUSHROOM, world, x, y)
{
	m_health = 4;
}

Mushroom::~Mushroom()
{}

void Mushroom::Action()
{
	if (m_health <= 0) 
		setDead(); 
}

int Mushroom::getHealth() const
{
	return m_health; 
}

void Mushroom::decreaseHealth()
{
	m_health--; 
	setBrightness(getBrightness() - 0.25);
}

//////////////////////////////////////////////////

WaterDroplet::WaterDroplet(StudentWorld* world, int x, int y)
	: GameObject(IID_WATER_DROPLET, world, x, y)
{
	m_ticks = 2; 
}

WaterDroplet::~WaterDroplet()
{}

//Water droplets disappear after two frames
void WaterDroplet::Action()
{
	m_ticks--; 
	if (m_ticks <= 0) 
		setDead();
}

//////////////////////////////////////////////////

Insect::Insect(int id, StudentWorld* world, int x, int y)
	: GameObject(id, world, x, y)
{}

Insect::~Insect()
{}

//////////////////////////////////////////////////

Scentipede::Scentipede(StudentWorld* world, int x, int y)
	: Insect(IID_SCENTIPEDE_SEGMENT, world, x, y)
{
	moveDown = true;
	moveRight = true;
	isPoisoned = false; 
}

Scentipede::~Scentipede()
{}

void Scentipede::Action()
{
	if (isPoisoned)
	{
		int x, y;
		getLocation(x, y);
		if (y == 0)
			isPoisoned = false; //if the scentipede is at the bottom of the garden, it is no longer poisoned
		else 
		{
			//check what the poisoned scentipede will land on, kill mushrooms, player, and other scentipedes
			if (getWorld()->isMushroom(x, y - 1))
				getWorld()->removeObj(x, y - 1);

			if (getWorld()->isPlayer(x, y-1))
				getWorld()->removeObj(x, y - 1);

			if (getWorld()->isScentSeg(x, y - 1))
				getWorld()->removeObj(x, y - 1); 

			moveTo(x, y - 1);

			//check if still poisoned after moving; if no, move to next step, else return
			if (y == 0)
				isPoisoned = false;
			else
				return;
		}
	}

	int newx, newy;
	getLocation(newx, newy);

	//check what the new horizontal location will be after moving
	if (moveRight)
		newx++;
	else
		newx--;

	//change direction if necessary
	if (newx<0)
		moveRight = true; 
	else if (newx == GARDEN_WIDTH)
	{
		moveRight = false; 
	}
	else
	{
		//if no collisions
		if (!getWorld()->isMushroom(newx, newy) && !getWorld()->isPlayer(newx, newy) && !getWorld()->isScentSeg(newx, newy))
		{
			moveTo(newx, newy);
			return;
		}
		else if (getWorld()->isPlayer(newx, newy))
		{
			getWorld()->removeObj(newx, newy); 
			return;
		}
		else if (getWorld()->isPoisonMushroom(newx, newy))
		{
			isPoisoned = true;
			moveDown = true;
		}
	}

	//If we get to this section, we've hit a wall or mushroom so the segment must move down/up
	if (moveDown)
		newy--;
	else
		newy++;

	//change directions if necessary
	if (newy<0)
	{
		moveDown = false; 
		newy = 1;
	}
	else if (newy == GARDEN_HEIGHT)
	{
		moveDown = true;
		newy = GARDEN_HEIGHT - 2;
	}

	//check collisions
	if (!getWorld()->isMushroom(newx, newy) && !getWorld()->isPlayer(newx, newy) && !getWorld()->isScentSeg(newx, newy))
	{
		moveTo(newx, newy); 
		return;
	}
	else if (getWorld()->isPoisonMushroom(newx, newy))
	{
		isPoisoned = true;
		moveDown = true;
		newy--;
	}

	//we've hit something trying to move vertically, KILL IT
	if (getWorld()->isMushroom(newx, newy)) 
		getWorld()->removeObj(newx, newy);
	if (getWorld()->isPlayer(newx, newy))
		getWorld()->removeObj(newx, newy);
	if (getWorld()->isScentSeg(newx, newy))
		getWorld()->removeObj(newx, newy);

	moveTo(newx, newy);
}

void Scentipede::setDead()
{
	int x, y;
	getLocation(x, y);

	GameObject::setDead();

	if (randInt(1, 3) == 1)
		getWorld()->createMushroom(x, y); //33% chance of mushroom
}

//////////////////////////////////////////////////

Flea::Flea(StudentWorld* world, int x, int y)
	: Insect(IID_FLEA, world, x, y)
{}

Flea::~Flea()
{}

void Flea::Action()
{
	int x, y;
	getLocation(x, y); 

	if (!getWorld()->isMushroom(x, y) && y > 0 && y < GARDEN_HEIGHT - 1)
	{
		if (randInt(1, 4) == 1)
			getWorld()->createMushroom(x, y);
	}

	y--; 
	if (y<0)
		setDead();
	else
		moveTo(x, y);

	if (getWorld()->isPlayer(x, y))
		getWorld()->removeObj(x, y);
}

//////////////////////////////////////////////////

Spider::Spider(StudentWorld* world, int x, int y)
	: Insect(IID_SPIDER, world, x, y)
{
	m_distance = randInt(1, 4);
	actionRest = false; 
	moveDown = true; 
	if (x == 0)
		moveRight = true;
	else
		moveRight = false;

}

Spider::~Spider()
{}

void Spider::Action()
{
	//Spiders move every other frame
	if (actionRest)
	{
		actionRest = false; 
		return;
	}

	int x, y;
	getLocation(x, y); 

	if (m_distance == 0) 
	{
		if (!moveDown) 
		{
			moveDown = true;
			m_distance = randInt(1, y - 1);
		}
		else 
		{
			moveDown = false;
			m_distance = randInt(1, GARDEN_HEIGHT - y - 1);
		}
	}

	int new_x, new_y;

	if (moveDown)
		new_y = y - 1;
	else
		new_y = y + 1;

	if (moveRight)
		new_x = x + 1;
	else
		new_x = x - 1;

	m_distance--; 

	//spiders destroy mushrooms
	if (getWorld()->isMushroom(new_x, new_y))
		getWorld()->removeObj(new_x, new_y); 

	if (new_x<0 || new_x >= GARDEN_WIDTH || new_y<0 || new_y >= GARDEN_HEIGHT)
		setDead();
	else
		moveTo(new_x, new_y); 

	if (getWorld()->isPlayer(new_x, new_y))
		getWorld()->removeObj(new_x, new_y); 

	actionRest = true; 
}

//////////////////////////////////////////////////

Scorpion::Scorpion(StudentWorld* world, int x, int y)
	: Insect(IID_SCORPION, world, x, y)
{}

Scorpion::~Scorpion()
{}

void Scorpion::Action()
{
	int x, y;
	getLocation(x, y);

	//33% chance of poisoning mushrooms
	if (getWorld()->isMushroom(x, y) && randInt(1, 3) == 1)
		getWorld()->poisonMushroom(x, y); 

	int new_x = x + 1;

	if (new_x < GARDEN_WIDTH - 1) 
		moveTo(new_x, y);
	else
		setDead();

	if (getWorld()->isPlayer(new_x, y))
		getWorld()->removeObj(new_x, y); 
}
