#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

#define beta 1.0
#define N 64
#define delt 0.005
#define pi 3.1415

void init(double * masas);
double * Newton(double * masas);
double freq_2(int k);
double Q(int k, double * masas);
double * derv(double * masas, double * masas_ant);
double E(int k, double * masas, double * masas_ant);

int main(int argc, char *argv[]){

  FILE * posiciones;
  posiciones=fopen("posiciones.txt", "w");
  FILE * energias;
  energias=fopen("energias.txt", "w");
  int procs = argv[1][0]-'0';

  int i, j;
  int iter = (int) 5.0*pow(N,2.2)/delt;
  double *osc;
  osc=malloc(N*sizeof(double));
  double *osc_p;
  osc_p=malloc(N*sizeof(double));
  double *vels;
  vels=malloc(N*sizeof(double));
  double *vels_p;
  vels_p=malloc(N*sizeof(double));
  double *acc;
  acc=malloc(N*sizeof(double));
  init(osc_p);
  acc=Newton(osc_p);
  for(j=0;j<N;j++){
    vels[j]=0.0;
    vels_p[j]=0.0;
  }
  for(j=1;j<N-1;j++){
    vels[j]=acc[j]*delt/2.0;
  }

  omp_set_num_threads(procs);
  for (i=0;i<iter;i++){

    #pragma omp parallel for private(j), shared(osc, osc_p, vels)
    for(j=1;j<N-1;j++){
      osc[j]=osc_p[j]+vels[j]*delt;
    }
    acc=Newton(osc);
    for(j=1;j<N-1;j++){
      vels_p[j]=vels[j];
    }

    #pragma omp parallel for private(j), shared(vels, vels_p, acc)
    for(j=1;j<N-1;j++){
      vels[j]=vels_p[j]+acc[j]*delt;
    }

    if(i%(iter/1000)==0){
      for(j=0;j<N;j++){
        fprintf(posiciones, "%lf ", osc[j]);
      }
      fprintf(posiciones, "\n");
      fprintf(energias, "%lf %lf %lf %lf \n", E(1, osc, osc_p), E(2, osc, osc_p), E(3,osc, osc_p), i*delt);
    }

    for(j=1;j<N-1;j++){
      osc_p[j]=osc[j];
    }
  }
}

void init(double * masas){
  int i;
  for (i=0;i<N;i++){
    masas[i]=sin((pi*i)/(N-1));
  }
}
double * Newton(double * masas){
  int i;
  double *temp;
  temp=malloc(N*sizeof(double));
  temp[0]=0.0; temp[N-1]=0.0;

  for(i=1;i<N-1;i++){
    temp[i]=masas[i+1]-2*masas[i]+masas[i-1]+beta*(pow(masas[i+1]-masas[i],2)-pow(masas[i]-masas[i-1],2));
  }
  return temp;
}
double freq_2(int k){
  double var;
  var=4*pow(sin(k*pi/(2*N+2)),2);
  return var;
}
double Q(int k, double * masas){
  double var;
  int i;
  for(i=0;i<N;i++){
    var+=masas[i]*sin(k*i*pi/(N+1));
  }
  var=var*pow(2.0/(N+1),0.5);
  return var;

}
double * derv(double * masas, double * masas_ant){
  int i;
  double * var;
  var=malloc(sizeof(double)*N);
  var[0]=0.0;
  var[N-1]=0.0;
  for(i=1;i<N-1;i++){
    var[i]=(masas[i]-masas_ant[i])/delt;
  }
  return var;
}
double E(int k, double * masas, double * masas_ant){
  double var;
  var=0.5*(pow(Q(k, derv(masas, masas_ant)),2)+freq_2(k)*pow(Q(k, masas),2));
  return var;
}
