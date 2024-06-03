//
// Created by 王栋梁 on 2024/5/31.
//

#ifndef EXTRACTMULTIPERIODLINEARVELOCITY_FEATURIZE_H
#define EXTRACTMULTIPERIODLINEARVELOCITY_FEATURIZE_H

typedef struct _complexSp{
    double real;
    double imag;
}complexSp;

extern void delay_signal(complexSp * signal, int signal_length, double sample_rate, double delay_time, complexSp* delayed_signal);
extern double informationEntropy(double *signal1,double *signal2,int signal_length);
extern double waveAlign(complexSp (*signal)[1024],complexSp (*delayed_signal)[1024],double *interval,double sample_rate,int PeriodNum,double speedRange,double precision);
extern void accumulate(complexSp (*signal)[1024],double *signalMod,double *interval,double sample_rate,int PeriodNum,double speed,char *mode);
#endif //EXTRACTMULTIPERIODLINEARVELOCITY_FEATURIZE_H
