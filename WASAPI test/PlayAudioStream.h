#pragma once


#include<Audioclient.h>
#include<audiopolicy.h>
#include<mmdeviceapi.h>

#include "MyAudioSource.h"

#define REFTIMES_PER_SEC 10'000'000 //not sure what this will mean wrt performance 

#define REFTIMES_PER_MILLISEC 10'000

#define EXIT_ON_ERROR (hr) \
							if (FAILED(hr))\
							{\
								goto Exit;\
							}

#define SAFE_RELEASE(punk) if((punk) != NULL) {(punk)->Release(); (punk) = NULL;}


HRESULT PlayAudioStream(MyAudioSource* pMySource);

