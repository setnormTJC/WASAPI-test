#pragma once



#include<Audioclient.h>
#include<memory>
#include<mmdeviceapi.h>
#include<iostream>
#include<vector> 
#include<Windows.h>

/*forward dec. for friend func.*/
HRESULT PlayAudioStream(class MyAudioSource* pMySource);

/*Class impl "foundation" taken partially from: 
https://gist.github.com/mhamilt/859e57f064e4d5b2bb8e78ae55439d70
*/
class MyAudioSource
{
public: 
	MyAudioSource() = default; 

	MyAudioSource(const std::vector<float>& inputTimeAmplitudes);

	friend HRESULT PlayAudioStream(MyAudioSource* pMySource);
private: 
	HRESULT LoadData(UINT32 totalFrames, BYTE* dataOut, DWORD* flags);
	HRESULT SetFormat(WAVEFORMATEX* wfex);

	void init(); //fills pPCMAudio with a sine wave
	bool initialized = false; 

	WAVEFORMATEXTENSIBLE format; 

	unsigned int pcmPos = 0; 
	UINT32 bufferSize; 
	UINT32 bufferPos = 0; 

	static constexpr unsigned int samplesPerSecond = 48'000; 

	//unsigned int duration = 5;
	unsigned int sampleCount;// = samplesPerSecond * duration; //5 seconds of audio (48K is actually perhaps more common than 44.1K)

	float frequency = 110.0f; 

	std::unique_ptr<float[]> timeAmplitudes; /*This is the sound wave data in the time domain*/
};