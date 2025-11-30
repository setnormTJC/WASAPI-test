#define _USE_MATH_DEFINES

#include "Synthesizer.h"

Synthesizer::Synthesizer(SynthType synthType, const MusicNote& musicNote)
{
	switch (synthType)
	{
	case SynthType::Piano: 
		fillFrequenciesToAmplitudes(musicNote); 
		fillSynthesizedWave(musicNote);
		applyPianoADSR(musicNote); 
		break; 

	case SynthType::Bass:
		//...to be implemented 
		break;
	}
}

std::vector<float> Synthesizer::getSynthesizedWave()
{
	return synthesizedWave;
}

void Synthesizer::fillFrequenciesToAmplitudes(const MusicNote& musicNote)
{
	MusicNoteUtility mnu; 
	
	float fundamentalFrequency = mnu.getNoteFrequency(musicNote.getName()); 

	constexpr int numberOfOvertonesDesired = 10; 

	std::vector<float> frequencies; 

	for (int i = 2; i <= numberOfOvertonesDesired + 1; ++i)
	{
		frequencies.push_back(fundamentalFrequency * i);
	}

	/*ROUGHLY exponential decay for piano overtone amplitudes*/
	float fundamentalAmplitude = musicNote.getNormalizedLoudness(); 

	std::vector<float> amplitudes; 
	for (int i = 2; i <= numberOfOvertonesDesired + 1; ++i)
	{
		float currentAmplitude = fundamentalAmplitude * std::pow((1.0 / M_E), i - 1); //(1/e)^ (i - 1)
		amplitudes.push_back(currentAmplitude);
	}

	//insert the fundamental and its amp: 
	frequenciesToAmplitudes.insert({ fundamentalFrequency, fundamentalAmplitude });

	for (int i = 0; i < frequencies.size(); ++i)
	{
		frequenciesToAmplitudes.insert({ frequencies[i], amplitudes[i] });
	}
}

void Synthesizer::fillSynthesizedWave(const MusicNote& musicNote)
{
	float durationInSeconds = musicNote.getDurationInSeconds(); 

	constexpr unsigned int samplesPerSecond = 48'000; //danger here if MyAudioSource changes this member var!

	unsigned int sampleCount = durationInSeconds * samplesPerSecond; 

	synthesizedWave.resize(sampleCount, 0.0f);
	
	for (int timePoint = 0; timePoint < sampleCount; ++timePoint)
	{
		for (const auto& [frequency, amplitude] : frequenciesToAmplitudes)
		{
			float sinArg = (frequency * timePoint / samplesPerSecond);

			float theSinePortion = sin(2 * M_PI * sinArg);

			float amp = synthesizedWave[timePoint] + amplitude * theSinePortion; //accumulate (synthetic wave interference)

			amp = std::clamp(amp, -0.99f, +0.99f); //no buzzies

			synthesizedWave[timePoint] = amp;
		}
	}
}

void Synthesizer::applyPianoADSR(const MusicNote& musicNote)
{
	//repeating myself (as in fillSynthesizeWave) here for the first 3 lines ...
	float durationInSeconds = musicNote.getDurationInSeconds();
	constexpr unsigned int samplesPerSecond = 48'000; //danger here if MyAudioSource changes this member var!
	unsigned int sampleCount = durationInSeconds * samplesPerSecond; 


	/*********************ATTACK***********************************/
	float attackTimeInSeconds = durationInSeconds * 0.01; //fast attack (staccato on piano?) 
	int numberOfSamplesInAttackPhase = attackTimeInSeconds * samplesPerSecond;
	float peakLevel = std::ranges::max(synthesizedWave); 

	for (int timePoint = 0; timePoint < numberOfSamplesInAttackPhase; ++timePoint)
	{
		float attackProgress = static_cast<float>(timePoint) / numberOfSamplesInAttackPhase; 
		//linear increase from 0 to 1, I think 
		synthesizedWave[timePoint] = synthesizedWave[timePoint] * attackProgress;
	}

	for (int timePoint = numberOfSamplesInAttackPhase; timePoint < sampleCount; ++timePoint)
	{
		float releaseProgress = static_cast<float>
			(timePoint - numberOfSamplesInAttackPhase) /
			(sampleCount - numberOfSamplesInAttackPhase);

		float amp = peakLevel * pow(1.0f - releaseProgress, 2); //2 means quadratic decay 

		synthesizedWave[timePoint] = synthesizedWave[timePoint] * (amp / peakLevel);

	}

}
