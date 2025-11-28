#pragma once

#include<Audioclient.h>
#include<iostream>
#include<memory>
#include<mmdeviceapi.h>
#include<vector> 
#include<Windows.h>


HRESULT RecordAudioStream(class MyAudioSink* pMySink);/*forward declaration for friend function*/

class MyAudioSink
{

public: 
	MyAudioSink();
	/*Note that this function calls the private func `normalize`*/
	std::vector<float> getTimeAmplitudes(); 

	friend HRESULT RecordAudioStream(MyAudioSink* pMySink);
private: 
	HRESULT SetFormat(WAVEFORMATEX* pwfx);
	HRESULT CopyData(BYTE* pData, UINT32 numFramesAvailable, BOOL* bDone);
	/*sets amps to values between +/- 1.0f*/
	void normalize(); 


private: 
	std::vector<float> timeAmplitudes; /*NOTE that these values will be "normalized" between -1.0*/

	WAVEFORMATEXTENSIBLE format;

	unsigned int pcmPos = 0;

	static constexpr unsigned int samplesPerSecond = 48'000;

	static constexpr unsigned int duration = 5; //in the FUTURE, it will likely be desirable for this to vary with input 
	static const unsigned int sampleCount = samplesPerSecond * duration; //5 seconds of audio (48K is actually perhaps more common than 44.1K)


};