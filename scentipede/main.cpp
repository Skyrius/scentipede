

#if !defined(MACOSX)  &&  !defined(LINUX)
#include "glut.h"
#else
#include <GLUT/glut.h>
#endif


#include "GameController.h"
#include "StudentWorld.h"				
#include <ctime>
#include <cstdlib>
using namespace std;

void getTestParams(int testParams[], int argc, char *argv[])
{
	for (int i=0;i<NUM_TEST_PARAMS;i++)
	{
		if (i+1 < argc)
			testParams[i] = atoi(argv[i+1]);
		else
			testParams[i] = 0;
	}
}

int main(int argc, char** argv)  
{ 
	glutInit(&argc, argv); 
	GameWorld *gcl = new StudentWorld;

	int testParams[NUM_TEST_PARAMS];
	getTestParams(testParams,argc,argv);
	GameController gco(gcl,testParams);

	srand(time(NULL));

	gco.run("Scentipede");

	SoundFX::destruct();

	return 0; 
} 
