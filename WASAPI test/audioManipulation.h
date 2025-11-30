#pragma once

#include "MyAudioSource.h"
#include "MusicNote.h"

class AudioManipulation
{
private: 
	MusicNoteUtility musicNoteUtility;
public: 
	void generateTone(); 

	void generate88Notes();

	void playPureSineNote(); 

	void playSynthesizedNote(); 


};
