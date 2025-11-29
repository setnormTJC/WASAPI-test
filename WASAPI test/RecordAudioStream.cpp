#include "RecordAudioStream.h"

#define EXIT_ON_ERROR(hr) \
        if (FAILED(hr)) {goto Exit;} //go to!

#include <ksmedia.h>


const IID IID_IAudioCaptureClient = __uuidof(IAudioCaptureClient); //new global here (others in PlayAudioStream)

/*Implementation was taken (primarily) from: 
https://learn.microsoft.com/en-us/windows/win32/coreaudio/capturing-a-stream*/
HRESULT RecordAudioStream(MyAudioSink* pMySink)
{
	HRESULT hr; 
	REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC;
	REFERENCE_TIME hnsActualDuration; 
	IMMDeviceEnumerator* pEnumerator = NULL; 
	IMMDevice* pDevice = NULL; 
	IAudioClient* pAudioClient = NULL;
	IAudioCaptureClient* pCaptureClient = NULL; //capture
	WAVEFORMATEX* pwfx = NULL;
	UINT32 bufferFrameCount; 
	UINT32 numFramesAvailable; 
	UINT32 packetLength = 0; //packet length
	BOOL bDone = FALSE; 
	BYTE* pData; 
	DWORD flags; 

	/*These last 2 are not in Microsoft's example documentation but are perhaps necessary for some of my goals:*/
	UINT64 u64DevicePosition = 0; //
	UINT64 u64QPCPosition = 0; //"Query Performance Counter" 

	hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);//"apartment" model
	EXIT_ON_ERROR(hr);

	hr = CoCreateInstance(
		CLSID_MMDeviceEnumerator, NULL, //NOTE that this type was declared GLOBALLY in PlayAudioStream.cpp
		CLSCTX_ALL, IID_IMMDeviceEnumerator, 
		(void**)&pEnumerator
		);
	EXIT_ON_ERROR(hr); 

	hr = pEnumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &pDevice); 
	EXIT_ON_ERROR(hr); 

	hr = pDevice->Activate(IID_IAudioClient, CLSCTX_ALL, NULL, (void**) &pAudioClient);
	EXIT_ON_ERROR(hr);

	hr = pAudioClient->GetMixFormat(&pwfx);
	EXIT_ON_ERROR(hr);

	hr = pAudioClient->Initialize(
		AUDCLNT_SHAREMODE_SHARED,
		0, 
		hnsRequestedDuration, 
		0, 
		pwfx, 
		NULL
	);
	EXIT_ON_ERROR(hr);

	hr = pAudioClient->GetBufferSize(&bufferFrameCount);
	EXIT_ON_ERROR(hr)

	hr = pAudioClient->GetService(
		IID_IAudioCaptureClient,
		(void**)&pCaptureClient);
	EXIT_ON_ERROR(hr);

	hr = pMySink->SetFormat(pwfx); 
	EXIT_ON_ERROR(hr); 

	hnsActualDuration = (double)REFTIMES_PER_SEC *
		bufferFrameCount / pwfx->nSamplesPerSec;

	std::cout << "Beginning recording in \n";
	std::cout << "3...";
	Sleep(1000); 
	std::cout << "2...";
	Sleep(1000);
	std::cout << "1...";
	Sleep(1000);
	std::cout << "0";
	//Sleep(1000);

	hr = pAudioClient->Start();  // Start recording.
	EXIT_ON_ERROR(hr);

	while (bDone == FALSE)
	{
		// Sleep for half the buffer duration.
		Sleep(hnsActualDuration / REFTIMES_PER_MILLISEC / 2);

		hr = pCaptureClient->GetNextPacketSize(&packetLength); //480 from my mic
		EXIT_ON_ERROR(hr); 

		while (packetLength != 0)
		{
			hr = pCaptureClient->GetBuffer(
				&pData,//gets filled here
				&numFramesAvailable,//also gets filled here
				&flags, &u64DevicePosition, &u64QPCPosition //the last 2 here are set to NULL in Microsoft's example 
			);
			EXIT_ON_ERROR(hr); 

			if (flags & AUDCLNT_BUFFERFLAGS_SILENT)
			{
				pData = NULL; //Tells MyAudioSink::CopyData() to write SILENCE
			}

			hr = pMySink->CopyData(
				pData, numFramesAvailable, &bDone //pData gets sent to (and stored in) a member var of `MyAudioSink` here!
			);					

			EXIT_ON_ERROR(hr);

			hr = pCaptureClient->ReleaseBuffer(numFramesAvailable);
			EXIT_ON_ERROR(hr);

			hr = pCaptureClient->GetNextPacketSize(&packetLength);
			EXIT_ON_ERROR(hr);
		}
	}

	hr = pAudioClient->Stop();  // Stop recording.
	EXIT_ON_ERROR(hr)



Exit:
	CoTaskMemFree(pwfx); //why only on this guy? 
	
	SAFE_RELEASE(pEnumerator); 
	SAFE_RELEASE(pDevice);
	SAFE_RELEASE(pAudioClient);
	SAFE_RELEASE(pCaptureClient);

	return hr; 
}
