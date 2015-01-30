#ifndef _SOUNDFX_H_
#define _SOUNDFX_H_

#include <string>
#include <map>

#if !defined(MACOSX)  &&  !defined(LINUX)
#include <windows.h>
#include <stdio.h>
#include <io.h>
#endif

class SoundFX
{
  public:

	  static void destruct()
	  {
		  std::map<std::string, char *>::iterator it;
		  for (it = m_loadedSounds.begin();it!= m_loadedSounds.end();it++)
			  delete it->second;
	  }

    static void playClip(const std::string &mp3File)
    {
#if !defined(MACOSX)  &&  !defined(LINUX)
		if (m_loadedSounds.find(mp3File) == m_loadedSounds.end())
		{
			FILE *stream = fopen(mp3File.c_str(),"rb");
			if (stream)
			{
				unsigned int mp3Length = _filelength(_fileno(stream));
				if (mp3Length > 0)
				{
					char *buffer = new char[mp3Length];
					fread(buffer,1,mp3Length,stream);
					m_loadedSounds[mp3File] = buffer;
				}
				fclose(stream);
			}
		}
#ifndef DONT_PLAY_SOUND
		PlaySound(m_loadedSounds[mp3File], NULL, SND_ASYNC|SND_MEMORY);
#endif
#endif
    }

    static void playTone(int hertz, int milliseconds)
    {
#if !defined(MACOSX)  &&  !defined(LINUX)
        Beep(hertz, milliseconds);
#endif
    }
private:

	static std::map<std::string, char *>		m_loadedSounds;
};

#endif // _SOUNDFX_H_
