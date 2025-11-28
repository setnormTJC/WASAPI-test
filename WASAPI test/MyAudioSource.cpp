#define _USE_MATH_DEFINES 
#define NOMINMAX

#include "MyAudioSource.h"

#include<algorithm>
#include<cmath> 
#include <ksmedia.h> // Includes GUID definitions like KSDATAFORMAT_SUBTYPE_IEEE_FLOAT


MyAudioSource::MyAudioSource(const std::vector<float>& inputTimeAmplitudes)
{
	timeAmplitudes = std::make_unique<float[]>(sampleCount); //this needs to be modified to inputTimeAmplitudes.size() soon!

	for (size_t i = 0; i < sampleCount; ++i) //also modify this loop termination condition!
	{
		timeAmplitudes[i] = inputTimeAmplitudes[i];
	}

	initialized = true;
}




HRESULT MyAudioSource::SetFormat(WAVEFORMATEX* wfex)
{
	if (wfex == nullptr)
	{
		return E_POINTER; 
	}

	WAVEFORMATEX* pFormat = wfex; //copy of arg.

	WAVEFORMATEXTENSIBLE* pExtFormat = nullptr; //another "possibility"

	if (pFormat->wFormatTag == WAVE_FORMAT_EXTENSIBLE)
	{
		pExtFormat = reinterpret_cast<WAVEFORMATEXTENSIBLE*>(pFormat); 
	}

	else
	{
		//format is the member var of this class
		format.Format = *pFormat; //a copy of the arg.

		format.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE; 
		format.Format.cbSize = sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX);

		format.Samples.wValidBitsPerSample = pFormat->wBitsPerSample; 
		format.dwChannelMask = 0; 

		if (pFormat->wFormatTag == WAVE_FORMAT_IEEE_FLOAT)
		{
			format.SubFormat = KSDATAFORMAT_SUBTYPE_IEEE_FLOAT;
		}

		else if (pFormat->wFormatTag == WAVE_FORMAT_PCM)
		{
			format.SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
		}
		else //MIGHT want to add more here ... 
		{
			return E_FAIL;
		}

		pExtFormat = &format; 
	}

	// Store the final format structure
	format = *pExtFormat;

	std::cout << "Channel count: " << format.Format.nChannels << "\n";
	std::cout << "Samples per second: " << format.Format.nSamplesPerSec << "\n";
	std::cout << "Bits per sample: " << format.Format.wBitsPerSample << "\n";

	if (!initialized) //parameterized constructor can ALSO fill `timeAmplitudes`
	{
		init(); 
	}

	return S_OK; //temp return val.
}

void MyAudioSource::init()
{
	timeAmplitudes = std::make_unique<float[]>(sampleCount);

	const float radianPerSample = 2 * M_PI * frequency / (float)format.Format.nSamplesPerSec; 

	for (unsigned long i  = 0; i < sampleCount; ++i)
	{
		float sampleValue = 0.05f* sin(radianPerSample * (float)i); //0.05f is amplitude

		sampleValue += 0.05f * sin(1.5f * radianPerSample * (float)i); //add some "harmonic" (that might not be harmonious)

		sampleValue += 0.05f * sin(1.75f * radianPerSample * (float)i); //and a third 

		timeAmplitudes[i] = sampleValue;
	}

	initialized = true; 
}


/*Chromatic tomfoolery*/
//void MyAudioSource::init()
//{
//	// Ensure this is defined if not available via <cmath>
//	const float PI = 3.14159265358979323846f;
//
//	// 1. Setup Constants
//	pPCMAudio = std::make_unique<float[]>(sampleCount);
//
//	const float sampleRate = (float)format.Format.nSamplesPerSec;
//
//	// The ratio for one semitone (half-step): the twelfth root of 2
//	const float semitoneRatio = std::pow(2.0f, 1.0f / 12.0f);
//
//	// The number of samples to play for each of the 12 notes
//	const UINT32 samplesPerNote = sampleCount / 12;
//
//	// 2. Loop Through the 12 Notes of the Chromatic Scale
//	float currentFrequency = frequency; // Start at 110.0 Hz (A2)
//	unsigned int currentSampleIndex = 0;
//
//	for (int note = 0; note < 12; ++note)
//	{
//		// 2.1 Calculate parameters for the current note
//		const float radianPerSample = 2 * PI * currentFrequency / sampleRate;
//
//		// Define the stop index for the current note
//		UINT32 noteEndIndex = currentSampleIndex + samplesPerNote;
//
//		// Ensure the last note fills the buffer completely (avoids rounding error silence)
//		if (note == 11) {
//			noteEndIndex = sampleCount;
//		}
//
//		// 2.2 Generate the Sine Wave for the current note segment
//		for (unsigned int i = currentSampleIndex; i < noteEndIndex; ++i)
//		{
//			// Calculate the position in the current wave cycle
//			float phase = radianPerSample * (float)(i - currentSampleIndex);
//
//			// Generate a simple sine wave (you can add your harmonics back later)
//			float sampleValue = 0.5f * std::sin(phase); // Increased amplitude slightly for clarity
//
//			// Add a simple 3rd harmonic for a richer sound
//			sampleValue += 0.2f * std::sin(3.0f * phase);
//
//			// Clamp the value to the normalized range [-1.0, 1.0] if necessary (optional here)
//			// sampleValue = std::max(-1.0f, std::min(1.0f, sampleValue));
//
//			pPCMAudio[i] = sampleValue;
//		}
//
//		// 2.3 Move to the next frequency and starting sample index
//		currentFrequency *= semitoneRatio;
//		currentSampleIndex = noteEndIndex;
//
//		// Optional: print the frequency to verify the progression
//		// std::cout << "Note " << note + 1 << ": " << currentFrequency << " Hz\n";
//	}
//
//	// 3. Finalization
//	initialized = true;
//}




HRESULT MyAudioSource::LoadData(UINT32 totalFrames, BYTE* dataOut, DWORD* flags)
{
	if (!initialized)
	{
		return E_UNEXPECTED; //cute
	}

	const UINT16 channels = format.Format.nChannels; //"often" 2
	const UINT16 bytesPerSample = format.Format.wBitsPerSample / 8; //"often" 4 to 2, I think (32 or 16 bits)

	const UINT32 totalSamplesNeeded = totalFrames * channels; 
	
	UINT32 samplesToCopy = totalSamplesNeeded; 
	if (pcmPos + samplesToCopy > sampleCount)
	{
		samplesToCopy = sampleCount - pcmPos; 
		*flags = AUDCLNT_BUFFERFLAGS_SILENT; //signals the end of the stream
	}

	UINT framesToCopy = samplesToCopy / channels; 

	//fill with silence if no more is left:
	if (samplesToCopy == 0)
	{
		memset(dataOut, 0, totalFrames * channels * bytesPerSample);
		return S_OK; //too early here? 
	}

	/*Possible data conversion (if PCM -> IEEE float or vice versa, for example) and copy loop*/
	if (IsEqualGUID(format.SubFormat, KSDATAFORMAT_SUBTYPE_IEEE_FLOAT))
	{
		const float* pSource = &timeAmplitudes[pcmPos]; //no .get needed here on unique_ptr 
		memcpy(dataOut, pSource, samplesToCopy * sizeof(float));
	}

	else if (IsEqualGUID(format.SubFormat, KSDATAFORMAT_SUBTYPE_PCM) && bytesPerSample == 2) //magic number 2 ->  16 bit PCM
	{
		short* pDest = reinterpret_cast<short*>(dataOut);

		for (UINT32 i = 0; i < samplesToCopy; ++i)
		{
			float fSample = timeAmplitudes[pcmPos + i];

			// Convert float (-1.0 to 1.0) to short (-32768 to 32767)
			// Clamp value to prevent overflow during conversion
			fSample = std::max(-1.0f, std::min(1.0f, fSample)); //#define NOMINMAX! (macro otherwise!)
			pDest[i] = static_cast<short>(fSample * 32767.0f);
		}
	}

	//It MIGHT be necessary to add other formats here!

	//update pos: 
	pcmPos += samplesToCopy;

	// Fill the rest of the buffer space with silence if we ran out of data
	if (framesToCopy < totalFrames)
	{
		// Offset to the start of the silent region
		BYTE* pSilence = dataOut + (framesToCopy * channels * bytesPerSample);

		// Total bytes of silence to write
		UINT32 silenceBytes = (totalFrames - framesToCopy) * channels * bytesPerSample;
		memset(pSilence, 0, silenceBytes);
	}

	return S_OK;
}