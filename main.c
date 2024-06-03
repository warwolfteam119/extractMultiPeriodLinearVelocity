#include <stdio.h>
#include "math.h"
#include "featurize.h"
#include "strings.h"
#include <sys/time.h>
#include "test.h"
#define Nfft 1024

int main() {
    struct timeval start, end;
    complexSp signal[5][Nfft];
    complexSp delayed_signal[5][Nfft];
    double signalMod[Nfft];
    double  interval[5] = {0,20,40,60,80};
    memset(signal,0,sizeof(signal));
    //读取本地数据
    FILE *fid = fopen("../snr10speed10.bin","rb");
    fread(signal,sizeof(complexSp),Nfft*5,fid);
    fclose(fid);
    gettimeofday(&start, NULL);
    double delay_estimate = waveAlign(signal,delayed_signal,interval,625,5,10,1e-2);
    accumulate(signal,signalMod,interval,625,5,delay_estimate,"1");
    gettimeofday(&end, NULL);
    double time = end.tv_sec - start.tv_sec +
                  (end.tv_usec - start.tv_usec)/1.e6;

    FILE *fid2 = fopen("../signalMod.bin","wb");
    fwrite(signalMod,sizeof(double),Nfft,fid2);
    fclose(fid2);

//    double entropy = informationEntropy(signal,signal,Nfft);
//    printf("informationEntropy:%f\n",entropy);
    printf("Time taken: %f seconds,delay_estimate:%f\n", time,delay_estimate);


    return 0;
}
