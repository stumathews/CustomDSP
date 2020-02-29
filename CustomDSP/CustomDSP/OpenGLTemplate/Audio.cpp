#include "Audio.h"
#include <iostream>
#include "CircularBuffer.h"
#include "Game.h"


#pragma comment(lib, "lib/fmod_vc.lib")

//circular_buffer<float> cbuf1(1024);
//circular_buffer<float> cbuf2(1024);

float buffer1[1024];
float buffer2[1024];

CCamera* camera;

/*
I've made these two functions non-member functions
*/

// Check for error
void FmodErrorCheck(FMOD_RESULT result)
{
	if (result != FMOD_OK) {
		const char *errorString = FMOD_ErrorString(result);
		// MessageBox(NULL, errorString, "FMOD Error", MB_OK);
		// Warning: error message commented out -- if headphones not plugged into computer in lab, error occurs
	}
}

// DSP callback
FMOD_RESULT F_CALLBACK DSPCallback(FMOD_DSP_STATE *dsp_state, float *inbuffer, float *outbuffer, unsigned int length, int inchannels, int *outchannels)
{
	FMOD::DSP *thisdsp = (FMOD::DSP *)dsp_state->instance;

	for (unsigned int n = 0; n < length; n++) 
	{
		int ci1 = n;
		int ci2 = n;
		for (int chan = 0; chan < *outchannels; chan++) 
		{
			// input signal
			float* x = &inbuffer[(n * inchannels) + chan];

			// Position of output signal
			float* y = &outbuffer[(n * *outchannels) + chan];
			
			// Manipulate the input signal by a factor of 0.2
			float camX = camera->GetPosition().x;
			float camZ = camera->GetPosition().z;
			float orient = camera->GetView().x;
			*y = *x * orient;// camera->GetPosition().x;
			
			// Store input signals for both channels into separate circular buffers
			if (chan == 0)
			{
				buffer1[ci1] = *y;
				ci1 = (ci1 + 1) % 1024;
			}
			if (chan == 1)
			{
				buffer2[ci2] = *y;
				ci2 = (ci2 + 1) % 1024;
			}
		}
		
	}

	return FMOD_OK;
}

CAudio::CAudio()
{

}

CAudio::~CAudio()
{
	m_FmodSystem->release();
}

bool CAudio::Initialise(CCamera* cam)
{
	camera = cam;

	// Create an FMOD system
	result = FMOD::System_Create(&m_FmodSystem);
	FmodErrorCheck(result);
	if (result != FMOD_OK) 
		return false;

	// Initialise the system
	result = m_FmodSystem->init(32, FMOD_INIT_NORMAL, 0);
	FmodErrorCheck(result);
	if (result != FMOD_OK) 
		return false;

	// prepare our buffer
	memset(&buffer1, 0, 1024);
	memset(&buffer2, 0, 1024);
	// Create the DSP effect
	{
		FMOD_DSP_DESCRIPTION dspdesc;
		memset(&dspdesc, 0, sizeof(dspdesc));

		strncpy_s(dspdesc.name, "My first DSP unit", sizeof(dspdesc.name));
		dspdesc.numinputbuffers = 1;
		dspdesc.numoutputbuffers = 1;
		dspdesc.read = DSPCallback;

		result = m_FmodSystem->createDSP(&dspdesc, &m_dsp);
		FmodErrorCheck(result);

		if (result != FMOD_OK)
			return false;
	}

	return true;
	
}

// Load an event sound
bool CAudio::LoadEventSound(char *filename)
{
	result = m_FmodSystem->createSound(filename, FMOD_LOOP_OFF, 0, &m_eventSound);
	FmodErrorCheck(result);
	if (result != FMOD_OK) 
		return false;

	return true;
	

}

// Play an event sound
bool CAudio::PlayEventSound()
{
	result = m_FmodSystem->playSound(m_eventSound, NULL, false, NULL);
	FmodErrorCheck(result);
	if (result != FMOD_OK)
		return false;
	return true;
}


// Load a music stream
bool CAudio::LoadMusicStream(char *filename)
{
	result = m_FmodSystem->createStream(filename, NULL | FMOD_LOOP_NORMAL, 0, &m_music);
	FmodErrorCheck(result);

	if (result != FMOD_OK) 
		return false;

	return true;
	

}

// Play a music stream
bool CAudio::PlayMusicStream()
{
	result = m_FmodSystem->playSound(m_music, NULL, false, &m_musicChannel);
	FmodErrorCheck(result);

	if (result != FMOD_OK)
		return false;

	// Inject a custom DSP unit into the channel
	m_musicChannel->addDSP(0, m_dsp);

	return true;
}

void CAudio::Update()
{
	m_FmodSystem->update();
}