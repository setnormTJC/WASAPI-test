#define NOMINMAX

#include "MyAudioSink.h"

#include<algorithm>

MyAudioSink::MyAudioSink()
{
	timeAmplitudes.reserve(sampleCount); 
}

std::vector<float> MyAudioSink::getTimeAmplitudes()
{
	normalize(); 
	return timeAmplitudes;
}


HRESULT MyAudioSink::SetFormat(WAVEFORMATEX* wfex)
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

	return S_OK; 
}

HRESULT MyAudioSink::CopyData(BYTE* pData, UINT32 numFramesAvailable, BOOL* bDone)
{
	//pcmPos starts at 0 when this class is instantiated
	// - it should increase by numFramesAvailable every time this function is called (I think)

	if (pcmPos >= sampleCount)
	{
		*bDone = TRUE;
		return S_OK; 
	}

	UINT32 framesToCopy = std::min(numFramesAvailable, (sampleCount - pcmPos));

	const UINT16 channels = format.Format.nChannels; //typically 2, I suppose (for OUTPUT in "surround sound", may be 5)
	const UINT16 bytesPerSample = format.Format.wBitsPerSample / 8; 

	//total samples for this packet: 
	const UINT32 totalSamples = framesToCopy * channels; 

	if (pData != NULL) //this will handle (only) 2 formats - PCM and IEEE float - more MIGHT be needed in future
	{
		if (IsEqualGUID(format.SubFormat, KSDATAFORMAT_SUBTYPE_PCM) && bytesPerSample == 2)
		{
			//16 bit PCM has short data type - so reinterpret BYTE* 
			short* pSource = reinterpret_cast<short*>(pData); 

			for (UINT32 i = 0; i < totalSamples; ++i)
			{
				float normalizedSample = static_cast<float>(pSource[i]) / 32768.0f;
				timeAmplitudes.push_back(normalizedSample); 
			}
		}

		else if (IsEqualGUID(format.SubFormat, KSDATAFORMAT_SUBTYPE_IEEE_FLOAT) && bytesPerSample == 4)
		{
			float* pSource = reinterpret_cast<float*>(pData); 
			timeAmplitudes.insert(timeAmplitudes.end(), pSource, pSource + totalSamples); //should be nice for speed here 
		}
	}

	/*IMPORTANT - update pcmPos now that all data has been copied:*/
	pcmPos += numFramesAvailable;

	if (pcmPos >= sampleCount)
	{
		*bDone = TRUE; 
	}

	return S_OK;
}

void MyAudioSink::normalize()
{
	float max = 0.0f; 

	for (const auto& amp : timeAmplitudes)
	{
		max = std::max(max, std::abs(amp)); 
	}

	if (max > 0.0f) //if silence, do not divide by 0
	{
		for (auto& amp : timeAmplitudes)
		{
			amp = amp / max;
		}
	}
}
