#include "SoundFX.h"

HANDLE SoundFX::m_threadHandle, SoundFX::m_mutex, SoundFX::m_semaphore;
DWORD SoundFX::m_threadID;
std::string SoundFX::m_clip;
