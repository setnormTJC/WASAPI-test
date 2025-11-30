#include "AudioManipulation.h"
#include "imgui/imgui.h"

#include <thread>
#include "Synthesizer.h"



//std::thread audioThread; 
//bool audioRunning = false; 


void AudioManipulation::generateTone()
{
	static float targetFrequency = 110.0f; //MUST be static (for now) 

	ImGui::Begin("Audio Generator Control");                          // Create a window

	ImGui::Text("Playback Frequency");                               // Label
	ImGui::Separator();

	// Slider bound to the static frequency variable
	ImGui::SliderFloat("Frequency (Hz)", &targetFrequency, 20.0f, 2000.0f, "%.1f Hz");
	ImGui::Text("Current value: %.1f Hz", targetFrequency);

	// Placeholder for the "Play Tone" button (Next step!)
	if (ImGui::Button("Generate and Play Tone"))
	{

		MyAudioSource myAudioSource(targetFrequency);

		PlayAudioStream(&myAudioSource);

	}

	ImGui::End();
}

void AudioManipulation::generate88Notes()
{
	int lowestNote = 30; 
	int middleNote = 40; 
	int highestNote = 50; 

	static int N1 = lowestNote; 
	static int N2 = highestNote;

	std::vector<std::string> notes = musicNoteUtility.get88SharpNotes();

	ImGui::Begin("Chromatic");
	ImGui::SliderInt("Lowest note", &N1, lowestNote, middleNote - 1);
	ImGui::SliderInt("Highest note", &N2, middleNote, highestNote);
	ImGui::Text("sdfas");
	if (ImGui::Button("Play chromatic - from lowest to highest"))
	{
		for (size_t i = N1; i < N2; ++i)
		{
			float frequency = musicNoteUtility.getNoteFrequency(notes[i]);
			MyAudioSource myAudioSource(frequency);
			PlayAudioStream(&myAudioSource);
		}
	}
	ImGui::End(); 
}

void AudioManipulation::playPureSineNote()
{

	std::vector<std::string> notes = musicNoteUtility.get88SharpNotes(); 

	if (ImGui::TreeNode("Horizontal Scrolling"))
	{
		//ShowHelpMarker("Horizontal scrolling for a window has to be enabled explicitly via the ImGuiWindowFlags_HorizontalScrollbar flag.\n\nYou may want to explicitly specify content width by calling SetNextWindowContentWidth() before Begin().");
		static int lines = 7;
		ImGui::SliderInt("Lines", &lines, 1, 15);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2.0f, 1.0f));
		ImGui::BeginChild("scrolling", ImVec2(0, ImGui::GetFrameHeightWithSpacing() * 7 + 30), true, ImGuiWindowFlags_HorizontalScrollbar);
		for (int line = 0; line < 1; line++)
		{
			// Display random stuff (for the sake of this trivial demo we are using basic Button+SameLine. 
			// If you want to create your own time line for a real application you may be better off
			// manipulating the cursor position yourself, aka using SetCursorPos/SetCursorScreenPos to position 
			// the widgets yourself. You may also want to use the lower-level ImDrawList API)
			int num_buttons = notes.size(); 

			for (int n = 0; n < num_buttons; n++)
			{
				if (n > 0) ImGui::SameLine();
				ImGui::PushID(n + line * 1000);
				char num_buf[16];
				printf(num_buf, "%d", n);
				//const char* label = (!(n % 15)) ? "FizzBuzz" : (!(n % 3)) ? "Fizz" : (!(n % 5)) ? "Buzz" : num_buf;

				const char* label = notes[n].c_str();

				float hue = n * 0.05f;
				ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(hue, 0.6f, 0.6f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(hue, 0.7f, 0.7f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(hue, 0.8f, 0.8f));
				if (ImGui::Button(label, ImVec2(40.0f + sinf((float)(line + n)) * 20.0f, 0.0f)))
				{
					MusicNote musicNote(Duration::Whole, Loudness::Mezzo, label); //passing in a C-string here!
					
					float frequency = musicNote.getFrequency(); 
					float durationInSeconds = musicNote.getDurationInSeconds(); 
					float normalizedLoudness = musicNote.getNormalizedLoudness(); 

					MyAudioSource mas(durationInSeconds, frequency, normalizedLoudness);
					PlayAudioStream(&mas);

				}
				ImGui::PopStyleColor(3);
				ImGui::PopID();
			}
		}
		float scroll_x = ImGui::GetScrollX(), scroll_max_x = ImGui::GetScrollMaxX();
		ImGui::EndChild();
		ImGui::PopStyleVar(2);
		float scroll_x_delta = 0.0f;
		ImGui::SmallButton("<<"); if (ImGui::IsItemActive()) { scroll_x_delta = -ImGui::GetIO().DeltaTime * 1000.0f; } ImGui::SameLine();
		ImGui::Text("Scroll from code"); ImGui::SameLine();
		ImGui::SmallButton(">>"); if (ImGui::IsItemActive()) { scroll_x_delta = +ImGui::GetIO().DeltaTime * 1000.0f; } ImGui::SameLine();
		ImGui::Text("%.0f/%.0f", scroll_x, scroll_max_x);
		if (scroll_x_delta != 0.0f)
		{
			ImGui::BeginChild("scrolling"); // Demonstrate a trick: you can use Begin to set yourself in the context of another window (here we are already out of your child window)
			ImGui::SetScrollX(ImGui::GetScrollX() + scroll_x_delta);
			ImGui::EndChild();
		}
		ImGui::TreePop();
	}

}

void AudioManipulation::playSynthesizedNote()
{
	std::vector<std::string> notes = musicNoteUtility.get88SharpNotes();

	if (ImGui::TreeNode("Horizontal Scrolling"))
	{
		//ShowHelpMarker("Horizontal scrolling for a window has to be enabled explicitly via the ImGuiWindowFlags_HorizontalScrollbar flag.\n\nYou may want to explicitly specify content width by calling SetNextWindowContentWidth() before Begin().");
		static int lines = 7;
		ImGui::SliderInt("Lines", &lines, 1, 15);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2.0f, 1.0f));
		ImGui::BeginChild("scrolling", ImVec2(0, ImGui::GetFrameHeightWithSpacing() * 7 + 30), true, ImGuiWindowFlags_HorizontalScrollbar);
		for (int line = 0; line < 1; line++)
		{
			// Display random stuff (for the sake of this trivial demo we are using basic Button+SameLine. 
			// If you want to create your own time line for a real application you may be better off
			// manipulating the cursor position yourself, aka using SetCursorPos/SetCursorScreenPos to position 
			// the widgets yourself. You may also want to use the lower-level ImDrawList API)
			int num_buttons = notes.size();

			for (int n = 0; n < num_buttons; n++)
			{
				if (n > 0) ImGui::SameLine();
				ImGui::PushID(n + line * 1000);
				char num_buf[16];
				printf(num_buf, "%d", n);
				//const char* label = (!(n % 15)) ? "FizzBuzz" : (!(n % 3)) ? "Fizz" : (!(n % 5)) ? "Buzz" : num_buf;

				const char* label = notes[n].c_str();

				float hue = n * 0.05f;
				ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(hue, 0.6f, 0.6f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(hue, 0.7f, 0.7f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(hue, 0.8f, 0.8f));
				if (ImGui::Button(label, ImVec2(40.0f + sinf((float)(line + n)) * 20.0f, 0.0f)))
				{
					MusicNote musicNote(Duration::Whole, Loudness::Mezzo, label); //passing in a C-string here!

					Synthesizer synth(SynthType::Piano, musicNote);

					std::vector<float> timeAmplitudes = synth.getSynthesizedWave(); 

					MyAudioSource mas(timeAmplitudes);

					PlayAudioStream(&mas);

				}
				ImGui::PopStyleColor(3);
				ImGui::PopID();
			}
		}
		float scroll_x = ImGui::GetScrollX(), scroll_max_x = ImGui::GetScrollMaxX();
		ImGui::EndChild();
		ImGui::PopStyleVar(2);
		float scroll_x_delta = 0.0f;
		ImGui::SmallButton("<<"); if (ImGui::IsItemActive()) { scroll_x_delta = -ImGui::GetIO().DeltaTime * 1000.0f; } ImGui::SameLine();
		ImGui::Text("Scroll from code"); ImGui::SameLine();
		ImGui::SmallButton(">>"); if (ImGui::IsItemActive()) { scroll_x_delta = +ImGui::GetIO().DeltaTime * 1000.0f; } ImGui::SameLine();
		ImGui::Text("%.0f/%.0f", scroll_x, scroll_max_x);
		if (scroll_x_delta != 0.0f)
		{
			ImGui::BeginChild("scrolling"); // Demonstrate a trick: you can use Begin to set yourself in the context of another window (here we are already out of your child window)
			ImGui::SetScrollX(ImGui::GetScrollX() + scroll_x_delta);
			ImGui::EndChild();
		}
		ImGui::TreePop();
	}
}
