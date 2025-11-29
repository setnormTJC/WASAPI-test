// WASAPI test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>


#include"ImguiManager.h"

#include"PlayAudioStream.h"
#include"RecordAudioStream.h"

int main()
{

	ImguiManager imgui; 

	MyAudioSink myAudioSink; 

	RecordAudioStream(&myAudioSink);

	std::cout << "\nDone recording\n";

	std::vector<float> timeAmplitudes = myAudioSink.getTimeAmplitudes(); 

	//now feed to MyAudioSource: 
	MyAudioSource myAudioSource(timeAmplitudes);

	PlayAudioStream(&myAudioSource);
}

