//
// Created by 王栋梁 on 2024/6/1.
//
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <portaudio.h>

#define SAMPLE_RATE 44100
#define FRAMES_PER_BUFFER 64

typedef struct {
    double f_start;
    double f_end;
    double pulse_width;
    int sampleIndex;
    int num_samples;
    double k;
    double* signal;
} paTestData;

// Function to generate Linear Frequency Modulated (LFM) signal
void generateLFM(double f_start, double f_end, double pulse_width, double* signal) {
    int num_samples = (int)(SAMPLE_RATE * pulse_width);
    double k = (f_end - f_start) / pulse_width;

    for (int i = 0; i < num_samples; i++) {
        double t = (double)i / SAMPLE_RATE;
        double f_t = f_start + k * t;
        signal[i] = sin(2.0 * M_PI * f_t * t);
    }
}

// Function to generate Hyperbolic Frequency Modulated (HFM) signal
void generateHFM(double f_start, double f_end, double pulse_width, double* signal) {
    int num_samples = (int)(SAMPLE_RATE * pulse_width);
    double k = (f_start - f_end) / (f_end * pulse_width);

    for (int i = 0; i < num_samples; i++) {
        double t = (double)i / SAMPLE_RATE;
        double phi_t = (2.0 * M_PI * f_start / k) * log(1.0 + k * t);
        signal[i] = sin(phi_t);
    }
}

static int paCallback(const void *inputBuffer, void *outputBuffer,
                      unsigned long framesPerBuffer,
                      const PaStreamCallbackTimeInfo* timeInfo,
                      PaStreamCallbackFlags statusFlags,
                      void *userData) {
    paTestData *data = (paTestData*)userData;
    float *out = (float*)outputBuffer;
    unsigned int i;

    for (i = 0; i < framesPerBuffer; i++) {
        if (data->sampleIndex < data->num_samples) {
            *out++ = data->signal[data->sampleIndex++]; // left channel
            *out++ = data->signal[data->sampleIndex++]; // right channel (duplicate for stereo)
        } else {
            *out++ = 0.0; // silence
            *out++ = 0.0; // silence
        }
    }
    return paContinue;
}

void playSignal(double f_start, double f_end, double pulse_width, int signalType) {
    PaStream *stream;
    PaError err;
    paTestData data;

    data.f_start = f_start;
    data.f_end = f_end;
    data.pulse_width = pulse_width;
    data.sampleIndex = 0;
    data.num_samples = (int)(SAMPLE_RATE * pulse_width);
    data.signal = (double*)malloc(data.num_samples * sizeof(double));

    if (signalType == 0) {
        generateLFM(f_start, f_end, pulse_width, data.signal);
    } else {
        generateHFM(f_start, f_end, pulse_width, data.signal);
    }

    err = Pa_Initialize();
    if (err != paNoError) goto error;

    err = Pa_OpenDefaultStream(&stream,
                               0,          /* no input channels */
                               2,          /* stereo output */
                               paFloat32,  /* 32 bit floating point output */
                               SAMPLE_RATE,
                               FRAMES_PER_BUFFER,
                               paCallback,
                               &data);
    if (err != paNoError) goto error;

    err = Pa_StartStream(stream);
    if (err != paNoError) goto error;

    printf("Press any key to stop playback.\n");
    getchar();

    err = Pa_StopStream(stream);
    if (err != paNoError) goto error;

    err = Pa_CloseStream(stream);
    if (err != paNoError) goto error;

    Pa_Terminate();
    free(data.signal);
    return;

    error:
    Pa_Terminate();
    fprintf(stderr, "An error occurred while using the portaudio stream\n");
    fprintf(stderr, "Error number: %d\n", err);
    fprintf(stderr, "Error message: %s\n", Pa_GetErrorText(err));
    free(data.signal);
    return;
}
