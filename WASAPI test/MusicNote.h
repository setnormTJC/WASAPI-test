#pragma once
#include <string>
#include <vector>

#include"InsertionOrderedMap.h"

enum class Duration
{
	Eighth,
	Quarter,
	Half,
	Whole
};

enum class Loudness
{
	Piano,
	Mezzo,
	Forte
};


class MusicNote
{

private: 
	Duration duration;
	Loudness loudness; 
	std::string name;/*ex: A#3, Bb7, G4 - range: from A0 to C8*/

public:
	MusicNote() = delete;
	MusicNote(Duration duration, Loudness loudness, const std::string& name);

	float getFrequency() const;

	float getDurationInSeconds() const;
	float getNormalizedLoudness() const;

	std::string getName() const; 

private: 
};

class MusicNoteUtility
{

private:

	std::vector<std::string> sharpNoteLetters;
	std::vector<std::string> flatNoteLetters;

	std::vector<std::string> all88SharpNotes; 
	//InsertionOrderedSet all88SharpNotes;

	//std::vector<std::pair<std::string, float>> notesToFrequencies; 
	InsertionOrderedMap<std::string, float> notesToFrequencies; 

private:
	void fill88SharpNotes(); 
	/*For calc. details, see: https://en.wikipedia.org/wiki/Piano_key_frequencies*/
	void fillNotesToFrequencies(); 
	
public:
	MusicNoteUtility();

	std::vector<std::string> get88SharpNotes() const;

	float getNoteFrequency(const std::string noteName); 
};


