#include "PlayAudioStream.h"


#define EXIT_ON_ERROR(hr) \
        if (FAILED(hr)) {goto Exit;}


//__uuidof is an operator defined by Microsoft that returns a Global Unique ID
const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator); //a class
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator); //a struct
const IID IID_IAudioClient = __uuidof(IAudioClient);
const IID IID_IAudioRenderClient = __uuidof(IAudioRenderClient);


/*Implementation of this function was taken, for the most part, from: 
https://learn.microsoft.com/en-us/windows/win32/coreaudio/rendering-a-stream*/
HRESULT PlayAudioStream(MyAudioSource* pMySource)
{
	HRESULT hr; 
	REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC;
	REFERENCE_TIME hnsActualDuration; 
	IMMDeviceEnumerator* pEnumerator = NULL; 
	IMMDevice* pDevice = NULL; 
	IAudioClient* pAudioClient = NULL; 
	IAudioRenderClient* pRenderClient = NULL; 
	WAVEFORMATEX* pwfx = NULL; 
	UINT32 bufferFrameCount; 
	UINT32 numFramesAvailable; 
	UINT32 numFramesPadding; 
	BYTE* pData; 
	DWORD flags = 0; 

	//1. init COM: 
	hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);//"apartment" model
	EXIT_ON_ERROR(hr);

	//2. fill pEnumerator 
	hr = CoCreateInstance(
		CLSID_MMDeviceEnumerator, NULL, 
		CLSCTX_ALL, IID_IMMDeviceEnumerator, 
		(void**)&pEnumerator
	);
	EXIT_ON_ERROR(hr);

	//3: fill pDevice
	hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice); 
	EXIT_ON_ERROR(hr);

	//4: fill pAudioClient
	hr = pDevice->Activate(
		IID_IAudioClient, CLSCTX_ALL,
		NULL, (void**)&pAudioClient
	);
	EXIT_ON_ERROR(hr);

	//5.1 get device's default "shared mix format" 
	hr = pAudioClient->GetMixFormat(&pwfx);
	EXIT_ON_ERROR(hr);

	//5.2: fill pwfx
	hr = pAudioClient->Initialize(
		AUDCLNT_SHAREMODE_SHARED, 
		0, 
		hnsRequestedDuration,
		0, 
		pwfx,
		NULL
	);
	EXIT_ON_ERROR(hr); 

	//6: set format for audio source: 
	hr = pMySource->SetFormat(pwfx); //implemented in MyAudioSource.cpp
	EXIT_ON_ERROR(hr);

	//7: get buffer size: 
	hr = pAudioClient->GetBufferSize(&bufferFrameCount);
	EXIT_ON_ERROR(hr);

	//8: fill render client: 
	hr = pAudioClient->GetService(IID_IAudioRenderClient, (void**)&pRenderClient);
	EXIT_ON_ERROR(hr); 

	//9:get buffer
	hr = pRenderClient->GetBuffer(bufferFrameCount, &pData);
	EXIT_ON_ERROR(hr); 

	//10: load INITIAL data into buffer
	hr = pMySource->LoadData(bufferFrameCount, pData, &flags);
	EXIT_ON_ERROR(hr);

	//11: RELEASE!
	hr = pRenderClient->ReleaseBuffer(bufferFrameCount, flags);
	EXIT_ON_ERROR(hr);

	//12: calc the "actual" duration of the buffer: 
	hnsActualDuration = (double)REFTIMES_PER_SEC *
		bufferFrameCount / pwfx->nSamplesPerSec;

	hr = pAudioClient->Start(); //play that tune
	EXIT_ON_ERROR(hr); 

	while (flags != AUDCLNT_BUFFERFLAGS_SILENT) //flags gets set to this EVENTUALLY in MyAudioSource::LoadData
	{
		//sleep for half the buffer duration (might modify this later?)
		Sleep((DWORD)(hnsActualDuration / REFTIMES_PER_MILLISEC/2));

		//how much space is available? 
		hr = pAudioClient->GetCurrentPadding(&numFramesPadding);
		EXIT_ON_ERROR(hr);

		numFramesAvailable = bufferFrameCount - numFramesPadding;

		// Grab all the available space in the shared buffer.
		hr = pRenderClient->GetBuffer(numFramesAvailable, &pData); 
		EXIT_ON_ERROR(hr);

		// Get next 1/2-second of data from the audio source.
		hr = pMySource->LoadData(numFramesAvailable, pData, &flags);
		EXIT_ON_ERROR(hr);

		hr = pRenderClient->ReleaseBuffer(numFramesAvailable, flags);
		EXIT_ON_ERROR(hr);

	}
	
	// Wait for last data in buffer to play before stopping.
	Sleep((DWORD)(hnsActualDuration / REFTIMES_PER_MILLISEC / 2));

	hr = pAudioClient->Stop();  // Stop playing.
	EXIT_ON_ERROR(hr)

Exit:
	SAFE_RELEASE(pEnumerator); 
	SAFE_RELEASE(pDevice);
	SAFE_RELEASE(pAudioClient);
	SAFE_RELEASE(pRenderClient); 

	return hr; 

}
