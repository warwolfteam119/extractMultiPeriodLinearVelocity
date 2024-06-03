//
// Created by 王栋梁 on 2024/5/31.
//
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <fftw3.h>
#include <cblas.h>
#include <strings.h>
#include "featurize.h"

void getEnvelope(complexSp *signal,int signal_length,double *signalMod){
    int i = 0;
    double l1 = 0;
    for(i=0;i<signal_length;i++){
        signalMod[i] = sqrt(signal[i].real*signal[i].real+signal[i].imag*signal[i].imag);
        l1 += signalMod[i]/(double)signal_length;
    }
    for(i=0;i<signal_length;i++){
        signalMod[i] = signalMod[i]/l1+0.2;
    }
}
// Function to delay a signal by a specified time
void delay_signal(complexSp* signal, int signal_length, double sample_rate, double delay_time, complexSp* delayed_signal) {
    // Step 1: Initialize FFTW
    fftw_complex *in, *out;
    fftw_plan forward_plan, backward_plan;

    // Allocate memory for FFTW input and output arrays
    in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * signal_length);
    out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * signal_length);

    // Initialize the input array to zero
    memset((void*) in,0,sizeof(fftw_complex) * signal_length);

    // Fill the input array with the signal using OpenBLAS cblas_dcopy
    cblas_zcopy(signal_length, signal, 1, in, 1);

    // Create a plan for forward FFT
    forward_plan = fftw_plan_dft_1d(signal_length, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
    // Execute the forward FFT
    fftw_execute(forward_plan);
    // Step 2: Adjust the phase of each frequency component
    double angular_frequency, phase_shift;
    double real_part,imag_part;
    for (int i = 0; i < signal_length; i++) {
        // Calculate the angular frequency
        if(i<signal_length/2){
            angular_frequency = 2.0 * M_PI * (double)i / (double)signal_length * sample_rate;
        }else{
            angular_frequency = 2.0 * M_PI * ((double)i / (double)signal_length * sample_rate-sample_rate);
        }

        // Calculate the phase shift for the given delay time
        phase_shift = angular_frequency * delay_time;

        // Adjust the real and imaginary parts of the frequency component
        real_part = out[i][0];
        imag_part = out[i][1];
        out[i][0] = real_part * cos(phase_shift) - imag_part * sin(phase_shift);
        out[i][1] = real_part * sin(phase_shift) + imag_part * cos(phase_shift);
    }

    // Create a plan for inverse FFT
    backward_plan = fftw_plan_dft_1d(signal_length, out, in, FFTW_BACKWARD, FFTW_ESTIMATE);

    // Execute the inverse FFT
    fftw_execute(backward_plan);

    // Normalize the result and copy it to the output array
    for (int i = 0; i < signal_length; i++) {
        delayed_signal[i].real = in[i][0] / (float)signal_length;
        delayed_signal[i].imag = in[i][1] / (float)signal_length;
    }

    // Clean up
    fftw_destroy_plan(forward_plan);
    fftw_destroy_plan(backward_plan);
    fftw_free(in);
    fftw_free(out);
}

double waveAlign(complexSp (*signal)[1024],complexSp (*delayed_signal)[1024],double *interval,double sample_rate,int PeriodNum,double speedRange,double precision){
    if(PeriodNum<=1){
        return NAN;
    }
    int iPeriod = 0,iSpeed = 0,index = 0;
    double speed = 0,entropy=0,entropyMax = 0,estimate = 0;
    double *delayed_signal_mod;
    double *temp = (double *) malloc(1024*sizeof(double));
    for(iSpeed = 0;iSpeed<(speedRange*2/precision);iSpeed++){
        speed = (double)iSpeed*precision-speedRange;
        for(iPeriod = 0;iPeriod<PeriodNum;iPeriod++){
            delay_signal(signal[iPeriod],1024,sample_rate,speed*interval[iPeriod]/750.0,delayed_signal[iPeriod]);
            getEnvelope(delayed_signal[iPeriod],1024,(double*)delayed_signal[iPeriod]);
        }
        memcpy(temp,delayed_signal[0],sizeof(double)*1024);
        for(iPeriod = 1;iPeriod<PeriodNum;iPeriod++){
            delayed_signal_mod = (double *)delayed_signal[iPeriod];
            for(index = 0;index<1024;index++){
                temp[index] *= delayed_signal_mod[index];
            }
        }
        entropy = cblas_dsum(1024,temp,1);
        if(entropy>entropyMax){
            entropyMax = entropy;
            estimate = speed;
        }
    }
    return estimate;
}

void accumulate(complexSp (*signal)[1024],double *signalMod,double *interval,double sample_rate,int PeriodNum,double speed,char *mode){
    memset(signalMod,0,1024*sizeof(double));

    int iPeriod = 0,index = 0;
    complexSp *signalTemp = (complexSp *) malloc(1024*sizeof(complexSp));
    double *signal_mod = (double *)signalTemp;
    for(iPeriod = 0;iPeriod<PeriodNum;iPeriod++){
        delay_signal(signal[iPeriod],1024,sample_rate,speed*interval[iPeriod]/750.0,signalTemp);
        getEnvelope(signalTemp,1024,(double*)signalTemp);
        for(index = 0;index<1024;index++){
            signalMod[index] += signal_mod[index];
        }
    }
}