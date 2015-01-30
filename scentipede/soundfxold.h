#ifndef _SOUNDFX_H_
#define _SOUNDFX_H_

#include <string>
#if !defined(MACOSX)  &&  !defined(LINUX)
#include <windows.h>
#else
typedef int HANDLE;
typedef unsigned long DWORD;
#define WINAPI
#define LPVOID
#endif

class SoundFX
{
  public:

	static void init()
	{
		m_mutex = CreateMutex(NULL,false,NULL);
		m_semaphore = CreateSemaphore(NULL,0,1,NULL);

		m_threadHandle = CreateThread( 
            NULL,                   // default security attributes
            0,                      // use default stack size  
            soundPlayer,			// thread function name
            NULL,					// argument to thread function 
            0,                      // use default creation flags 
            &m_threadID);   // returns the thread identifier 
	}

	static void shutDown()
	{
		TerminateThread(m_threadHandle,0);
		CloseHandle(m_mutex);
		CloseHandle(m_semaphore);
	}

	static void playClip(const std::string &mp3File)
	{
#if !defined(MACOSX)  &&  !defined(LINUX)
		WaitForSingleObject(m_mutex,INFINITE);
		m_clip = mp3File;
		ReleaseMutex(m_mutex);
		ReleaseSemaphore(m_semaphore,1,NULL);
#endif
    }


private:

	static DWORD WINAPI soundPlayer( LPVOID lpParam )
	{
		for (;;)
		{
			WaitForSingleObject(m_semaphore,INFINITE);
			WaitForSingleObject(m_mutex,INFINITE);
			//PlaySound(m_clip.c_str(), NULL, SND_ASYNC);
			Beep(18000, 300);
			ReleaseMutex(m_mutex);
		}
	}

	static HANDLE	m_threadHandle, m_mutex, m_semaphore;
	static DWORD	m_threadID;
	static std::string m_clip;
};

#endif // _SOUNDFX_H_
