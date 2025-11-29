#include "audioManipulation.h"
#include "imgui/imgui.h"
#include "MyAudioSource.h"
#include "MyAudioSource.h"

#include <thread>


//std::thread audioThread; 
//bool audioRunning = false; 


void doTheThingOfInterest()
{
	static float targetFrequency = 110.0f; //MUST be static (for now) 

	ImGui::Begin("Audio Generator Control");                          // Create a window

	ImGui::Text("Playback Frequency");                               // Label
	ImGui::Separator();

	// Slider bound to the static frequency variable
	ImGui::SliderFloat("Frequency (Hz)", &targetFrequency, 20.0f, 20000.0f, "%.1f Hz");
	ImGui::Text("Current value: %.1f Hz", targetFrequency);

	// Placeholder for the "Play Tone" button (Next step!)
	if (ImGui::Button("Generate and Play Tone"))
	{

		MyAudioSource myAudioSource(targetFrequency);

		PlayAudioStream(&myAudioSource);

	}

	ImGui::End();
}
