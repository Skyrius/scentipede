#ifndef _GRAPHOBJ_H_

#define _GRAPHOBJ_H_

#include <set>
#include <math.h>
 
#define EPSILON							.0001
#define ANIMATION_POSITIONS_PER_TICK	3

class GraphObject
{
public:

	GraphObject(int imageID)
	{
		m_displayMe = false;
		m_graphObjects.insert(this);
		m_x = m_y = 0;
		m_destX = m_destY = 0;
		m_imageID = imageID;
		m_brightness = 1.0;
		m_animationNumber = 0;
	}

	virtual ~GraphObject()
	{
		m_displayMe = false;
		m_graphObjects.erase(this);
	}

	void displayMe(bool shouldIDisplay)
	{
		m_displayMe = shouldIDisplay;
	}

	void setInitialLocation(int x, int y)
	{
		m_destX = m_x = x;
		m_destY = m_y = y;
	}

	void moveTo(int x, int y)
	{
		m_destX = x;
		m_destY = y;
	}

	unsigned int getAnimationNumber() const
	{
		return m_animationNumber;
	}

	void getLocation(int &x, int &y) const
	{
		if (m_destX != m_x || m_destY != m_y)
		{
			// if this guy has already been moved but has not been animated yet, then use his new location anyway

			roundUp(m_destX,m_destY,x,y);
		}
		else
		{
			roundUp(m_x,m_y,x,y);
		}
	}

	// should only be used by our framework - not by the student
	void getAnimationLocation(double &x, double &y) const
	{
		x = m_x;
		y = m_y;
	}

	unsigned int getID() const
	{
 		return m_imageID;
	}

	void setID(unsigned int imageID)
	{
		m_imageID = imageID;
	}

	void setBrightness(double brightness)
	{
		m_brightness = brightness;
	}

	double getBrightness() const
	{
		return m_brightness;
	}

	// not to be used by students

	void animate()
	{
		m_animationNumber++;

		double dx = m_destX-m_x;
		double absdx = fabs(dx);
		double dy = m_destY-m_y;
		double absdy = fabs(dy);

		if (absdx >= EPSILON)
		{
			if (dx < 0)
				m_x -= 1.0/ANIMATION_POSITIONS_PER_TICK;
			else
				m_x += 1.0/ANIMATION_POSITIONS_PER_TICK;
		}

		if (absdy >= EPSILON)
		{
			if (dy < 0)
				m_y -= 1.0/ANIMATION_POSITIONS_PER_TICK;
			else
				m_y += 1.0/ANIMATION_POSITIONS_PER_TICK;
		}
	}

	static std::set<GraphObject *>	*getGraphObjects()
	{
		return &m_graphObjects;
	}

private:

	void roundUp(double xin, double yin, int &xout, int &yout) const
	{
		if (xin - (int)xin >= .5)
			xout = (int)xin + 1;
		else
			xout = (int)xin;

		if (yin - (int)yin >= .5)
			yout = (int)yin + 1;
		else
			yout = (int)yin;

	}

private:
	int										m_imageID;
	bool									m_displayMe;
	double									m_x, m_y;
	double									m_destX,m_destY;
	double									m_brightness;
	int										m_animationNumber;
	static std::set<GraphObject *>			m_graphObjects;
};

#endif //#ifndef _GRAPHOBJ_H_
