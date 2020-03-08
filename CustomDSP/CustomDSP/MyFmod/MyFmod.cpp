// MyFmod.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "../OpenGLTemplate/include/fmod_studio/fmod.hpp"
#include "../OpenGLTemplate/include/fmod_studio/fmod_errors.h"

#include <iostream>
#include "../../../../../../../../Program Files (x86)/FMOD SoundSystem/FMOD Studio API Windows/api/lowlevel/examples/common.h"

typedef struct
{
    float* buffer;
    float volume_linear;
    int   length_samples;
    int   channels;
} mydsp_data_t;


int main()
{
    FMOD::System* system;
    FMOD::Sound* sound;
    FMOD::Channel* channel;
    FMOD::DSP* mydsp;
    FMOD::ChannelGroup* mastergroup;
    FMOD_RESULT         result;
    unsigned int        version;
    void* extradriverdata = 0;

    Common_Init(&extradriverdata);

    /*
        Create a System object and initialize.
    */
    result = FMOD::System_Create(&system);
    ERRCHECK(result);

    result = system->getVersion(&version);
    ERRCHECK(result);

    if (version < FMOD_VERSION)
    {
        Common_Fatal("FMOD lib version %08x doesn't match header version %08x", version, FMOD_VERSION);
    }

    result = system->init(32, FMOD_INIT_NORMAL, extradriverdata);
    ERRCHECK(result);

    result = system->createSound(Common_MediaPath("stereo.ogg"), FMOD_LOOP_NORMAL, 0, &sound);
    ERRCHECK(result);

    result = system->playSound(sound, 0, false, &channel);
    ERRCHECK(result);

    result = system->getMasterChannelGroup(&mastergroup);
    ERRCHECK(result);

    do
    {
        bool bypass;

        Common_Update();

        result = mydsp->getBypass(&bypass);
        ERRCHECK(result);

        if (Common_BtnPress(BTN_ACTION1))
        {
            bypass = !bypass;

            result = mydsp->setBypass(bypass);
            ERRCHECK(result);
        }
        if (Common_BtnPress(BTN_ACTION2))
        {
            float vol;

            result = mydsp->getParameterFloat(1, &vol, 0, 0);
            ERRCHECK(result);

            if (vol > 0.0f)
            {
                vol -= 0.1f;
            }

            result = mydsp->setParameterFloat(1, vol);
            ERRCHECK(result);
        }
        if (Common_BtnPress(BTN_ACTION3))
        {
            float vol;

            result = mydsp->getParameterFloat(1, &vol, 0, 0);
            ERRCHECK(result);

            if (vol < 1.0f)
            {
                vol += 0.1f;
            }

            result = mydsp->setParameterFloat(1, vol);
            ERRCHECK(result);
        }

        result = system->update();
        ERRCHECK(result);

        {
            char                     volstr[32] = { 0 };
            FMOD_DSP_PARAMETER_DESC* desc;
            mydsp_data_t* data;

            result = mydsp->getParameterInfo(1, &desc);
            ERRCHECK(result);
            result = mydsp->getParameterFloat(1, 0, volstr, 32);
            ERRCHECK(result);
            result = mydsp->getParameterData(0, (void**)&data, 0, 0, 0);
            ERRCHECK(result);

            Common_Draw("==================================================");
            Common_Draw("Custom DSP Example.");
            Common_Draw("Copyright (c) Firelight Technologies 2004-2020.");
            Common_Draw("==================================================");
            Common_Draw("");
            Common_Draw("Press %s to toggle filter bypass", Common_BtnStr(BTN_ACTION1));
            Common_Draw("Press %s to decrease volume 10%%", Common_BtnStr(BTN_ACTION2));
            Common_Draw("Press %s to increase volume 10%%", Common_BtnStr(BTN_ACTION3));
            Common_Draw("Press %s to quit", Common_BtnStr(BTN_QUIT));
            Common_Draw("");
            Common_Draw("Filter is %s", bypass ? "inactive" : "active");
            Common_Draw("Volume is %s%s", volstr, desc->label);

            if (data->channels)
            {
                char display[80] = { 0 };
                int channel;

                for (channel = 0; channel < data->channels; channel++)
                {
                    int count, level;
                    float max = 0;

                    for (count = 0; count < data->length_samples; count++)
                    {
                        if (fabsf(data->buffer[(count * data->channels) + channel]) > max)
                        {
                            max = fabsf(data->buffer[(count * data->channels) + channel]);
                        }
                    }
                    level = (int)(max * 40.0f);

                    sprintf(display, "%2d ", channel);
                    for (count = 0; count < level; count++) display[count + 3] = '=';

                    Common_Draw(display);
                }
            }
        }

        Common_Sleep(50);
    } while (!Common_BtnPress(BTN_QUIT));

    /*
        Shut down
    */
    result = sound->release();
    ERRCHECK(result);

    result = mastergroup->removeDSP(mydsp);
    ERRCHECK(result);
    result = mydsp->release();
    ERRCHECK(result);

    result = system->close();
    ERRCHECK(result);
    result = system->release();
    ERRCHECK(result);

    Common_Close();

    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
