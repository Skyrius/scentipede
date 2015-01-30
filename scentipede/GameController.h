#ifndef _GAMECONTROLLER_H_

#define _GAMECONTROLLER_H_

/* observations:

	http://www.youtube.com/watch?v=SjHec1QJiek
	centipedes will eat a mushroom if they're blocked from moving down:
	  
	 *CCCC    
	  *
	If C is moving left it will eat the bottom *
	Fleas fall down from the top and create mushrooms randomly
	When a centipede segment is shot it may create a new mushroom
	Scorpions randomly convert mushrooms to poison mushrooms (full poison mushrooms)
	Spiders do a saw-shaped motion (diagonal up-down across the screen) then disappear when they hit the other side
*/

#include <string>

#if !defined(MACOSX)  &&  !defined(LINUX)
#include "glut.h"
#else
#include <GLUT/glut.h> 
#endif

#include "GameWorld.h"
#include "GraphObj.h"
#include "GameConstants.h"
#include "SoundFX.h"

enum GC_STATE {welcome,contgame,init,cleanup,moving,animate,gameover,prompt,quit};
#define INVALID_KEY	0

#define VISIBLE_MIN_X					-3.25
#define VISIBLE_MAX_X					3.25
#define VISIBLE_MIN_Y					-2
#define VISIBLE_MAX_Y					2
#define VISIBLE_MIN_Z					-20
#define VISIBLE_MAX_Z					-6

#define SCORE_X							0
#define SCORE_Y							0
#define SCORE_Z							-10 

#define FONT_SCALEDOWN					760.0

#define WINDOW_WIDTH					1024
#define WINDOW_HEIGHT					768

#define PERSPECTIVE_NEAR_PLANE			4
#define PERSPECTIVE_FAR_PLANE			22

#define RATIO_OF_Y_DELTA_TO_Z_DELTA		0//.10		// 10% increase in Y for each increase in Z away from the user

#define MS_PER_FRAME					10

#define NUM_PLAYER_LIVES				3

struct Point
{
	double dx, dy;
};

class GameController
{
public:

	GameController(GameWorld *gc,int testParams[])
	{
		m_gameState = welcome;
		m_score = 0;
		m_playerLives = NUM_PLAYER_LIVES;
		m_lastSpecialKeyHit = INVALID_KEY;
		m_lastKeyHit = INVALID_KEY;
		m_gc = gc;
		m_timeToPlaySound = 0;
		m_gc->setMyController(this);

		gc->setTestParams(testParams);
	}

	void run(const std::string &windowTitle)
	{
		glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
		//glDepthFunc(GL_LESS);
		m_width = WINDOW_WIDTH;
		m_height = WINDOW_HEIGHT;
		glutInitWindowSize (m_width, m_height); 
		glutInitWindowPosition (0, 0); 
		glutCreateWindow (windowTitle.c_str()); 
		//glMatrixMode (GL_PROJECTION); 
		//glClearColor (0.0, 0.0, 0.0, 0.0); 
		//glShadeModel (GL_FLAT); 

		glutDisplayFunc(Action); 
		glutReshapeFunc(reshape); 
		glutKeyboardFunc(keyboardEvent);
		glutSpecialFunc(specialKeyboardEvent);
		//glutIdleFunc(Action);
		glutTimerFunc(MS_PER_FRAME,timerFunc,0);
		glutMainLoop(); 
	}
	
	static bool getLastKey(int &value)
	{
		if (m_lastKeyHit != INVALID_KEY)
		{
			value = m_lastKeyHit;
			m_lastKeyHit = INVALID_KEY;
			return true;
		}

		if (m_lastSpecialKeyHit != INVALID_KEY)
		{
			unsigned int temp = m_lastSpecialKeyHit;
			m_lastSpecialKeyHit = INVALID_KEY;

			switch (temp)
			{
				case GLUT_KEY_LEFT:
					value = KEY_PRESS_LEFT_ARROW;
					return true;
				case GLUT_KEY_RIGHT:
					value = KEY_PRESS_RIGHT_ARROW;
					return true;
				case GLUT_KEY_UP:
					value = KEY_PRESS_UP_ARROW;
					return true;
				case GLUT_KEY_DOWN:
					value = KEY_PRESS_DOWN_ARROW;
					return true;
			}
			return true;
		}

		return false;
	}

	static void increaseScore(unsigned int howMuch)
	{
		m_score += howMuch;
	}

	static unsigned int getScore() 
	{
		return m_score;
	}

	static unsigned int getPlayerLives() 
	{
		return m_playerLives;
	}

	static void decPlayerLives() 
	{
		m_playerLives--;
	}

	static void timerFunc(int val)
	{
		Action();
		glutTimerFunc(MS_PER_FRAME,timerFunc,0);
	}

	// used for Glut's IDLE function
	static void Action()
	{
		int result;
		char temp[256];

		switch (m_gameState)
		{
			case welcome:
				m_mainMessage = "Welcome to Centipede!";
				m_secondMessage = "Press Enter to begin play...";
				SoundFX::playClip("theme.wav");
				m_gameState = prompt;
				m_nextState = init;
				break;
			case contgame:
				m_mainMessage = "You lost a life!";
				m_secondMessage = "Press Enter to continue playing...";
				m_gameState = prompt;
				m_nextState = cleanup;
				break;
			case moving:
				m_gameState = animate;
				m_curIntraFrameTick = ANIMATION_POSITIONS_PER_TICK;
				result = m_gc->move();
				if (result == GWSTATUS_PLAYER_DIED)
				{
					m_gc->decPlayerLives();			// reduce # of player lives
					SoundFX::playClip("die.wav");
					if (getPlayerLives() > 0)
						m_gameState = contgame;
					else
						m_gameState = gameover;
				}
				break;
			case animate:
				displayGameplay();
				if (m_curIntraFrameTick-- <= 0)
					m_gameState = moving;
				break;
			case cleanup:
				m_gc->cleanUp();
				m_gameState = init;
				break;
			case gameover:
				sprintf(temp,"%d",m_score);
				m_mainMessage = "Game Over! Your final score was " + std::string(temp) + "!";
				m_secondMessage = "Press Enter to quit...";
				m_gameState = prompt;
				m_nextState = quit;
				break;
			case prompt:
				promptUser();
				break;
			case init:
				m_gc->init();
				m_gameState = moving;
				break;
			case quit:
				exit(0);
		}
	}


	// callback
	static void keyboardEvent(unsigned char key,int x, int y)
	{
		m_lastKeyHit = key;
	}

	// callback
	static void specialKeyboardEvent(int key, int x, int y)
	{
		m_lastSpecialKeyHit = key;
	}

	static void reshape (int w, int h) 
	{ 
		//double m_aspectRatio = (double)w / (double)h; 
		//double m_width = w;
		//double m_height = h;

		glViewport (0, 0, (GLsizei) w, (GLsizei) h); 
		glMatrixMode (GL_PROJECTION); 
		glLoadIdentity ();
		setViewVolume();
		glMatrixMode (GL_MODELVIEW); 
	} 

	static void outputStroke(GLfloat x, GLfloat y, GLfloat z, GLfloat size, char const *str, bool center = false)
	{
		glPushMatrix();
		glLoadIdentity();
		glTranslatef(x,y,z);
		glScalef(size/FONT_SCALEDOWN,size/FONT_SCALEDOWN,size/FONT_SCALEDOWN);

		//glColor3f(1,1,1);
		for (;*str;str++)
			glutStrokeCharacter(GLUT_STROKE_ROMAN,*str);
		glPopMatrix();
	}

	static void outputStrokeCentered(GLfloat y, GLfloat z, char const *str)
	{
		float size = 1;
		glPushMatrix();
		glLoadIdentity();
		float lengthOfString = glutStrokeLength(GLUT_STROKE_ROMAN,(unsigned char *)str)/FONT_SCALEDOWN;
		glTranslatef(-lengthOfString/2,y,z);
		glScalef(size/FONT_SCALEDOWN,size/FONT_SCALEDOWN,size/FONT_SCALEDOWN);
		for (;*str;str++)
			glutStrokeCharacter(GLUT_STROKE_ROMAN,*str);
		glPopMatrix();
	}

private:

	static void setViewVolume()
	{
		// projection transformation
		gluPerspective(45.0, (float)m_width / m_height, PERSPECTIVE_NEAR_PLANE, PERSPECTIVE_FAR_PLANE);
	}

	
	static void convertCoords(double inx, double iny, double &outx, double &outy, double &outz)
	{
		inx /= GARDEN_WIDTH;
		iny /= GARDEN_HEIGHT;

		outx = VISIBLE_MIN_X + inx * (VISIBLE_MAX_X - VISIBLE_MIN_X);
		outy = VISIBLE_MIN_Y + iny * (VISIBLE_MAX_Y - VISIBLE_MIN_Y) * RATIO_OF_Y_DELTA_TO_Z_DELTA;
		outz = VISIBLE_MIN_Z + (1-iny) * (VISIBLE_MAX_Z - VISIBLE_MIN_Z);		// largest z value is the most negative (and far away from the user) value
	}

/*
	static void convertCoords(double inx, double iny, double &outx, double &outy, double &outz)
	{
		inx /= GARDEN_WIDTH;
		iny /= GARDEN_HEIGHT;

		outx = VISIBLE_MIN_X + inx * (VISIBLE_MAX_X - VISIBLE_MIN_X);
		outy = VISIBLE_MIN_Y + iny* (VISIBLE_MAX_Y - VISIBLE_MIN_Y) / 2;
		outz = VISIBLE_MAX_Z - iny* (VISIBLE_MAX_Y - VISIBLE_MIN_Y) * .6;		// largest z value is the most negative (and far away from the user) value
	}
*/


	static void drawCent(GraphObject *go)
	{
		glPushMatrix();

		glColor3f (1.0, 1.0, 1.0);
		glLineWidth(1);

		double x, y;
		go->getAnimationLocation(x,y);

		Point base[] = {{0,.25},{0,.75},{.25,1},{.75,1},{1,.75},{1,.25},{.75,0},{.25,0}};
		drawPolyFromBaseXY(x,y,base,sizeof(base)/sizeof(Point));



		switch(go->getAnimationNumber() % 3)
		{
			case 0:
				{
					// top leg
					Point leg1a1[] = {{.2,.5},{.2,1.1},{.3,1.1},{.3,.5}};
					drawPolyFromBaseXY(x,y,leg1a1,sizeof(leg1a1)/sizeof(Point));

					Point leg1a2[] = {{.2,1.1},{-.1,1.1},{-.1,1.3},{.2,1.3}};
					drawPolyFromBaseXY(x,y,leg1a2,sizeof(leg1a2)/sizeof(Point));

					// bottom leg
					Point leg2a1[] = {{.8,.5},{.8,-.1},{.7,-.1},{.7,.5}};
					drawPolyFromBaseXY(x,y,leg2a1,sizeof(leg2a1)/sizeof(Point));

					Point leg2a2[] = {{.8,-.1},{1.1,-.1},{1.1,-.3},{.8,-.3}};
					drawPolyFromBaseXY(x,y,leg2a2,sizeof(leg2a2)/sizeof(Point));

				}
				break;
			case 1:
				{
					Point leg1c[] = {{.4,.5},{.4,1.2},{.6,1.2},{.6,.5}};
					drawPolyFromBaseXY(x,y,leg1c,sizeof(leg1c)/sizeof(Point));

					Point leg2c[] = {{.4,.5},{.4,-.2},{.6,-.2},{.6,.5}};
					drawPolyFromBaseXY(x,y,leg2c,sizeof(leg2c)/sizeof(Point));
				}
				break;
			case 2:
				{
					// top leg
					Point leg1b1[] = {{.8,.5},{.8,1.1},{.7,1.1},{.7,.5}};
					drawPolyFromBaseXY(x,y,leg1b1,sizeof(leg1b1)/sizeof(Point));

					Point leg1b2[] = {{.8,1.1},{1.1,1.1},{1.1,1.3},{.8,1.3}};
					drawPolyFromBaseXY(x,y,leg1b2,sizeof(leg1b2)/sizeof(Point));

					// bottom leg
					Point leg2b1[] = {{.2,.5},{.2,-.1},{.3,-.1},{.3,.5}};
					drawPolyFromBaseXY(x,y,leg2b1,sizeof(leg2b1)/sizeof(Point));

					Point leg2b2[] = {{.2,-.1},{-.1,-.1},{-.1,-.3},{.2,-.3}};
					drawPolyFromBaseXY(x,y,leg2b2,sizeof(leg2b2)/sizeof(Point));
				}
				break;
		}

		glPopMatrix();

	}

	// x,y is the middle of the object
	static void drawMushroom(GraphObject *go)
	{
		glPushMatrix();

		double brightness = go->getBrightness();
		if (go->getID() == IID_MUSHROOM)
			glColor3f (0, brightness, brightness);
		else
			glColor3f (brightness, brightness,0);

		glLineWidth(1);

		double x, y;
		go->getAnimationLocation(x,y);

		Point base[] = {{.3,0},{.3,.4},{.7,.4},{.7,0}}; 
		drawPolyFromBaseXY(x,y,base,sizeof(base)/sizeof(Point));
		 
		Point capleft[] = {{.3,.4},{.05,.2},{0,.2},{0,.45},{.3,.8},{.5,.8},{.5,.4}};
		drawPolyFromBaseXY(x,y,capleft,sizeof(capleft)/sizeof(Point));

		Point capright[] = {{.7,.4},{.95,.2},{1.0,.2},{1.0,.45},{.7,.8},{.5,.8},{.5,.4}};
		drawPolyFromBaseXY(x,y,capright,sizeof(capright)/sizeof(Point));

		glPopMatrix();
	}

	// x,y is the middle of the object
	static void drawPlayer(GraphObject *go)
	{
		glPushMatrix();

		glColor3f (.5, .5, .25);
		glLineWidth(1);

		double x, y;
		go->getAnimationLocation(x,y);

		Point playertop[] = {{.4,.5},{.5,.8},{.6,.5}};
		drawPolyFromBaseXYFlat(x,y,playertop,sizeof(playertop)/sizeof(Point));

		Point playerbot[] = {{.2,.2},{.5,.6},{.8,.2}};
		drawPolyFromBaseXYFlat(x,y,playerbot,sizeof(playerbot)/sizeof(Point));

		glPopMatrix();
	}


	// x,y is the middle of the object

	static void drawSpider(GraphObject *go)
	{
		glPushMatrix();

		glColor3f (.25, .25, 0.5);
		glLineWidth(1);

		double x, y;
		go->getAnimationLocation(x,y);
		double wx,wy,wz;
		convertCoords(x,y,wx,wy,wz);
		glTranslatef(wx,wy+.15,wz);
		glutWireSphere((VISIBLE_MAX_X-VISIBLE_MIN_X)/(double)GARDEN_WIDTH/6,10,5);
		glTranslatef(0,-.15,0);
		glutWireSphere((VISIBLE_MAX_X-VISIBLE_MIN_X)/(double)GARDEN_WIDTH/2,10,5);
		glLineWidth(2);
		for (int i=0;i<4;i++)
		{

			glBegin(GL_POLYGON);
			glVertex3f(0,0,0);
			glVertex3f(-.2,.15*i-.2+rand() % 10 / 200.0,0);
			glVertex3f(0,0.03,0);
			glEnd();

			glBegin(GL_POLYGON);
			glVertex3f(0,0,0);
			glVertex3f(.2,.15*i-.2+rand() % 10 / 200.0,0);
			glVertex3f(0,0.03,0);
			glEnd();


/*			glBegin(GL_LINE);
			glVertex3f(0,0,0);
			glVertex3f(-.2,.15*i-.2+rand() % 10 / 200.0,0);
			glEnd();
			
			glBegin(GL_LINE);
			glVertex3f(0,0,0);
			glVertex3f(.2,.15*i-.2+rand() % 10 / 200.0,0);
			glEnd();
*/
		}

		glPopMatrix();
	}

	// x,y is the middle of the object
	static void drawFlea(GraphObject *go)
	{
		glPushMatrix();

		glColor3f (1.0, .7, 0.3);
		glLineWidth(1);

		double x, y;
		go->getAnimationLocation(x,y);
		double wx,wy,wz;
		convertCoords(x,y,wx,wy,wz);
		glTranslatef(wx,wy,wz);

		glutWireSphere((VISIBLE_MAX_X-VISIBLE_MIN_X)/(double)GARDEN_WIDTH/4.0,5,5);
		glTranslatef(0,-.06,0);
		glutWireSphere((VISIBLE_MAX_X-VISIBLE_MIN_X)/(double)GARDEN_WIDTH/6.0,5,5);
		glTranslatef(0,-.06,0);
		glutWireSphere((VISIBLE_MAX_X-VISIBLE_MIN_X)/(double)GARDEN_WIDTH/8.0,5,5);
		glTranslatef(0,.06,0);

		unsigned int animNum = go->getAnimationNumber();
		for (int i=0;i<4;i++)
		{
			double vertDelta = (animNum % 3) * .02;

			glBegin(GL_POLYGON);
			glVertex3f(0,0,0);
			glVertex3f(-.1,.10*i-.2+vertDelta,0);
			glVertex3f(0,0.03,0);
			glEnd();

			glBegin(GL_POLYGON);
			glVertex3f(0,0,0);
			glVertex3f(.1,.10*i-.2+vertDelta,0);
			glVertex3f(0,0.03,0);
			glEnd();

			/*
			glBegin(GL_LINE);
			glVertex3f(0,0,0);
			glVertex3f(-.1,.10*i-.2+vertDelta,0);
			glEnd();

			glBegin(GL_LINE);
			glVertex3f(0,0,0);
			glVertex3f(.1,.10*i-.2+vertDelta,0);
			glEnd();
			*/
		}

		glPopMatrix();
	}

	// x,y is the middle of the object
	static void drawScorpion(GraphObject *go)
	{
		double x, y;

		glPushMatrix();

		go->getAnimationLocation(x,y);
		glLineWidth(1);

		//convertCoords(x,y,wx,wy,wz);
		//glTranslatef(wx,wy,wz);
		 
		glColor3f (1.0, 0.2, 0.3);

		double scale = 3;

		Point scorpbase[] = {{.2,.4},{.6,.35},{.6,.65},{.2,.6}};
		drawPolyFromBaseXYFlat(x,y,scorpbase,sizeof(scorpbase)/sizeof(Point),scale );

		Point scorptail1[] = {{.2,.4},{-.1,.7},{.2,.6}};
		drawPolyFromBaseXYFlat(x,y,scorptail1,sizeof(scorptail1)/sizeof(Point),scale );

		Point scorptail2[] = {{-.12,.65},{.1,.8},{-.08,.65}};
		drawPolyFromBaseXYFlat(x,y,scorptail2,sizeof(scorptail2)/sizeof(Point),scale );

		Point claw1a[] = {{.6,.35},{.8,.2},{.6,.4}};
		drawPolyFromBaseXYFlat(x,y,claw1a,sizeof(claw1a)/sizeof(Point),scale );

		Point claw1b[] = {{.8,.2},{.7,.35},{.8,.25}};
		drawPolyFromBaseXYFlat(x,y,claw1b,sizeof(claw1b)/sizeof(Point),scale );

		Point claw2a[] = {{.6,.65},{.8,.8},{.6,.6}};
		drawPolyFromBaseXYFlat(x,y,claw2a,sizeof(claw2a)/sizeof(Point),scale );

		Point claw2b[] = {{.8,.8},{.7,.65},{.8,.75}};
		drawPolyFromBaseXYFlat(x,y,claw2b,sizeof(claw2b)/sizeof(Point),scale );

		glPopMatrix();
	}


	// x,y is the middle of the object
	static void drawWaterDroplet(GraphObject *go)
	{
		glColor3f (1.0, 0, 0);
		glLineWidth(1);

		double x, y;
		go->getAnimationLocation(x,y);

		Point bullet[] = {{.4,.5},{.5,.6},{.6,.5},{.5,.4}};
		drawPolyFromBaseXY(x,y,bullet,sizeof(bullet)/sizeof(Point));

		/*
		double sx,sy,sz;
		convertCoords(x+.5,y,sx,sy,sz);

		double ex,ey,ez;
		convertCoords(x+.5,y+1,ex,ey,ez);

		glBegin(GL_LINE);
			glVertex3f(sx,sy,sz);
			glVertex3f(ex,ey,ez);
		glEnd();
		*/
	}

	static void drawPolyFromBaseXY(double x, double y, Point arr[], unsigned int points)
	{
		double newx,newy,newz;

		//glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		
		//y += .5;		// place object in the middle of the square distance wise from the viewer
		convertCoords(x,y,newx,newy,newz);

		double xmult = (double)(VISIBLE_MAX_X - VISIBLE_MIN_X) / GARDEN_WIDTH;
		double ymult = (double)(VISIBLE_MAX_Y - VISIBLE_MIN_Y) / GARDEN_HEIGHT;

		glBegin(GL_POLYGON);
		for (unsigned int i=0;i<points;i++)
			glVertex3f(newx + xmult*(arr[i].dx-.5),newy + ymult*(arr[i].dy-.5),newz);
		glEnd();
	}

	
	static void drawPolyFromBaseXYFlat(double x, double y, Point arr[], unsigned int points, double scale=1.0)
	{
		double newx,newy,newz;

		//glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		
		glBegin(GL_POLYGON);
		for (unsigned int i=0;i<points;i++)
		{
			convertCoords(x+arr[i].dx*scale-.5,y+ arr[i].dy*scale-.5,newx,newy,newz);
			glVertex3f(newx,newy,newz);
		}
		glEnd();
	}
	

	static void displayGameplay()
	{
		// show OpenGL objects
		std::set<GraphObject*> *graphObjects = GraphObject::getGraphObjects();
		std::set<GraphObject*>::iterator it;

		// show text of prompt
		glEnable(GL_DEPTH_TEST);		// must be done each time before displaying graphics or gets disabled for some reason
		glLoadIdentity();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		gluLookAt(0,0,0,0,0,-1,0,1,0);

		bool centOnScreen = false;
		bool WaterDropletOnScreen = false;
		bool fleaOnScreen = false;
		bool spiderOnScreen = false;

		for (it = graphObjects->begin();it != graphObjects->end();it++)
		{
			GraphObject *cur = *it;
				
			cur->animate();
			switch(cur->getID()) 
			{
				case IID_SCENTIPEDE_SEGMENT:
					drawCent(cur);
					centOnScreen = true;
					break;
				case IID_SPIDER:
					drawSpider(cur);
					spiderOnScreen = true;
					break;

				case IID_FLEA:
					drawFlea(cur);
					fleaOnScreen = true;
					break;
					
				case IID_SCORPION:
					drawScorpion(cur);
					break;

				case IID_PLAYER:
					drawPlayer(cur);
					break;

				case IID_MUSHROOM:
				case IID_POISON_MUSHROOM:
					drawMushroom(cur);
					break;

				case IID_WATER_DROPLET:
	 				WaterDropletOnScreen = true;
					drawWaterDroplet(cur);
					break;

			}
		}

		drawScoreAndLives();

		m_timeToPlaySound--;
		if (fleaOnScreen && m_timeToPlaySound < 20)
		{
			SoundFX::playClip("flea.wav");
			m_timeToPlaySound = 100;
		}

		if (WaterDropletOnScreen)
		{
			SoundFX::playClip("laser.wav");
			m_timeToPlaySound = 20;
		}

		if (m_timeToPlaySound <= 0)
		{
			if (spiderOnScreen)
			{
				m_timeToPlaySound = 30;
				SoundFX::playClip("spider.wav");
			}
			else if (centOnScreen)
			{
				m_timeToPlaySound = 10;
				SoundFX::playClip("cent.wav");
			}
		}

		glutSwapBuffers();	
	}


	static void drawScoreAndLives()
	{
		char score[32];
		sprintf(score,"Lives: %d     Score: %08d",m_playerLives,m_score);
		glColor3f ((rand() % 100)/100.0 , (rand() % 100)/100.0, (rand() % 100)/100.0);
		outputStrokeCentered(SCORE_Y, SCORE_Z, score);
	}

	static void promptUser()
	{
		// show text of prompt
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glColor3f (1.0, 1.0, 1.0);
		glLoadIdentity (); 

		outputStrokeCentered(1,-5,m_mainMessage.c_str());
		outputStrokeCentered(-1,-5,m_secondMessage.c_str());
		int key;
		if (getLastKey(key) && key == 13)
			m_gameState = m_nextState;

		glutSwapBuffers();
	}
	 
private:
	static GC_STATE			m_gameState;
	static GC_STATE			m_nextState;
	static std::string 		m_mainMessage;
	static std::string 		m_secondMessage;
	static char				m_lastKeyHit;
	static int				m_lastSpecialKeyHit;
	static GameWorld		*m_gc;
	static double			m_aspectRatio, m_width, m_height;
	static int				m_curIntraFrameTick;
	static unsigned int		m_score;
	static unsigned int		m_playerLives;
	static int				m_timeToPlaySound;
};

#endif // #ifndef _GAMECONTROLLER_H_
