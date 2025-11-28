#pragma once

#include"MyAudioSink.h"

#include<Audioclient.h>
#include<audiopolicy.h>
#include<mmdeviceapi.h>

#define REFTIMES_PER_SEC  10'000'000
#define REFTIMES_PER_MILLISEC  10'000

#define EXIT_ON_ERROR(hres)  \
              if (FAILED(hres)) { goto Exit; }
#define SAFE_RELEASE(punk)  \
              if ((punk) != NULL)  \
                { (punk)->Release(); (punk) = NULL; }


HRESULT RecordAudioStream(MyAudioSink * pMySink); 