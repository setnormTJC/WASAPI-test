#pragma once
#include "MusicNote.h"

#include<algorithm>
#include<cmath>
#include<map> 

enum class SynthType
{
	Piano,
	Bass
};

class Synthesizer
{
public: 
	Synthesizer(SynthType synthType, const MusicNote& musicNote); 

	std::vector<float> getSynthesizedWave(); 
private: 
	void fillFrequenciesToAmplitudes(const MusicNote& musicNote);
	void fillSynthesizedWave(const MusicNote& musicNote);
	void applyPianoADSR(const MusicNote& musicNote);

private: 
	std::vector<float> synthesizedWave; 

	std::map<float, float> frequenciesToAmplitudes; 
};

