#pragma once



#include<Audioclient.h>
#include<memory>
#include<mmdeviceapi.h>
#include<iostream>
#include<Windows.h>

/*Class impl "foundation" taken primarily from: 
https://gist.github.com/mhamilt/859e57f064e4d5b2bb8e78ae55439d70
*/
class MyAudioSource
{
public: 
	MyAudioSource() = default; 

	~MyAudioSource() = default;

	HRESULT LoadData(UINT32 totalFrames, BYTE* dataOut, DWORD* flags);
	HRESULT SetFormat(WAVEFORMATEX* wfex);
private: 
	void init(); //fills pPCMAudio with a sine wave
	bool initialized = false; 
	WAVEFORMATEXTENSIBLE format; 

	unsigned int pcmPos = 0; 
	UINT32 bufferSize; 
	UINT32 bufferPos = 0; 

	static constexpr unsigned int samplesPerSecond = 48'000; 

	static constexpr unsigned int duration = 5;
	static const unsigned int sampleCount = samplesPerSecond * duration; //5 seconds of audio (48K is actually perhaps more common than 44.1K)

	float frequency = 110.0f; 

	std::unique_ptr<float[]> pPCMAudio; 
};