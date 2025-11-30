#include "MusicNote.h"


#include<cmath>
#include<ranges>

#include"MyException.h"

MusicNote::MusicNote(Duration duration, Loudness loudness, const std::string& name)
	:
	duration(duration),
	loudness(loudness)
{

	//confirm that name is acceptable:
	MusicNoteUtility musicNoteUtility;

	std::vector<std::string> noteNames = musicNoteUtility.get88SharpNotes();

	if (std::ranges::find(noteNames, name) == noteNames.end())
	{
		throw MyException(__FILE__, __LINE__, "note name not found");
	}

	this->name = name; 
}

float MusicNote::getFrequency() const
{

	MusicNoteUtility mnu;
	float frequency = mnu.getNoteFrequency(name); 

	return frequency;
}

float MusicNote::getDurationInSeconds() const
{
	switch (duration)
	{
	case Duration::Eighth: 
		return 0.5f; //arbitary-ish, temporary value until time signature and tempo implemented

	case Duration::Quarter: 
		return 1.0f; 

	case Duration::Half: 
		return 2.0f; 

	case Duration::Whole:
		return 4.0f;
	}

	throw MyException(__FILE__, __LINE__, "unhandled duration");
}

float MusicNote::getNormalizedLoudness() const
{
	switch (loudness)
	{
	case Loudness::Piano:
		return 0.3f; 

	case Loudness::Mezzo: 
		return 0.5f; 

	case Loudness::Forte: 
		return 0.8f; 
	}
}

std::string MusicNote::getName() const
{
	return name; 
}


#pragma region MusicNoteUtility
MusicNoteUtility::MusicNoteUtility()
{
	sharpNoteLetters =
	{
		"C",
		"C#",
		"D",
		"D#",
		"E",
		"F",
		"F#",
		"G",
		"G#",
		"A",
		"A#",
		"B",
	};


	flatNoteLetters = 
	{
		"C",
		"Db",
		"D",
		"Eb",
		"E",
		"F",
		"Gb",
		"G",
		"Ab",
		"A",
		"Bb",
		"B",
	};


	fill88SharpNotes(); 
	fillNotesToFrequencies(); 
}


void MusicNoteUtility::fill88SharpNotes()
{

	all88SharpNotes.push_back("A0");
	all88SharpNotes.push_back("A#0");
	all88SharpNotes.push_back("B0");

	for (int octave = 1; octave <= 7; ++octave)
	{
		for (const std::string& sharpNoteLetter : sharpNoteLetters)
		{
			std::string note = sharpNoteLetter + std::to_string(octave);
			all88SharpNotes.push_back(note);
		}
	}

	//last one: 
	all88SharpNotes.push_back("C8");
}

void MusicNoteUtility::fillNotesToFrequencies()
{
	double a4Frequency = 440.0; 

	double twelthRootOf2 = std::pow(2.0, 0.083333);

	//notesToFrequencies.reserve(all88SharpNotes.size());
	

	for (size_t n = 1; n <= all88SharpNotes.size(); ++n)
	{
		double exponent = n - 49.0; 
		double currentFrequency = std::pow(twelthRootOf2, exponent) * a4Frequency;

		notesToFrequencies.insert(all88SharpNotes[n - 1], currentFrequency);
	}
}

std::vector<std::string> MusicNoteUtility::get88SharpNotes() const
{
	return all88SharpNotes; 
}

float MusicNoteUtility::getNoteFrequency(const std::string noteName)
{
	return notesToFrequencies.at(noteName); 
}

#pragma endregion