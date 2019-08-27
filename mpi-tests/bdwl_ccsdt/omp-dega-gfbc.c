#include "../../tensor_kernels.h"
#include "../../scatter_kernels.h"
#include<omp.h>
//#include"mpi.h"


int A_range_a= 24;
int B_range_b= 16;
int B_range_c= 16;
int A_range_d = 24;
int A_range_e= 16;
int B_range_f= 16;

int A_range_g= 24;
int B_range_g= 24;
int C_range_g= 24;

int C_range_a= 24;
int C_range_b= 16;
int C_range_c= 16;
int C_range_d = 24;
int C_range_e= 16;
int C_range_f= 16;



void compare_fun(double* A, double*B,double*C ){
for(int a=0; a<C_range_a; a++)
for(int b=0; b<C_range_b; b++)
for(int c=0; c<C_range_c; c++)
for(int d=0; d<C_range_d; d++)
for(int e=0; e<C_range_e; e++)
for(int f=0; f<C_range_f; f++)
for(int g=0; g<C_range_g; g++)
C[+a* C_range_b* C_range_c* C_range_d* C_range_e* C_range_f+b* C_range_c* C_range_d* C_range_e* C_range_f+c* C_range_d* C_range_e* C_range_f+d* C_range_e* C_range_f+e* C_range_f+f] += 
A[+d* C_range_e* C_range_g* C_range_a+e* C_range_g* C_range_a+g* C_range_a+a] * 
B[+g* C_range_f* C_range_b* C_range_c+f* C_range_b* C_range_c+b* C_range_c+c]; 
}



int main(int argc, char** argv){

        int Tk4, Tk3, Tk2, Tk1, Tk0;
    int Tm4, Tm3, Tm2, Tm1, Tm0;
    int Tn4, Tn3, Tn2, Tn1, Tn0;

    A_range_a =  C_range_a = atoi(argv[1]);
    B_range_b =  C_range_b = atoi(argv[2]);
    B_range_c =  C_range_c = atoi(argv[3]);
    A_range_d =  C_range_d = atoi(argv[4]);
    A_range_e =  C_range_e = atoi(argv[5]);
    B_range_f =  C_range_f = atoi(argv[6]);
C_range_g =  A_range_g =  B_range_g = atoi(argv[7]);
    

    printf("dega\n");
    Tk4 =  Tk3 =  Tk2 =  Tk1  = A_range_g;
//    Tk1 = 12;
    Tk0 = 1;
    Tm4 =  A_range_a * A_range_d * A_range_e;
    Tm3 =  24;//;A_range_a * A_range_d * A_range_e;
    Tm2 = 24;
    Tm1 = 24;
    Tm0 = 6;

    Tn4 = B_range_b * B_range_c * B_range_f;
    Tn3 = Tn4;
    Tn2 = 16;
    Tn1 = 8;
    Tn0 = 8;

    
    double *A, *B, *C, *Abuf, *Bbuf;

C = (double*)memalign(4096, C_range_a*C_range_b*C_range_c*C_range_d*C_range_e*C_range_f*sizeof(double));//Co = abcdef;
double * C1 = (double*)memalign(4096, C_range_a*C_range_b*C_range_c*C_range_d*C_range_e*C_range_f*sizeof(double));//Co = abcdef;


    
A = (double*)memalign(4096, sizeof(double) * A_range_d * A_range_e * A_range_g * A_range_a);
B = (double*)memalign(4096, sizeof(double) * B_range_g * B_range_f * B_range_b * B_range_c);
for(int i = 0; i < 1*A_range_d*A_range_e*A_range_g*A_range_a; i++){
A[i] = rand()%1000;
}
for(int i = 0; i < 1*B_range_g*B_range_f*B_range_b*B_range_c; i++){
B[i] = rand()%1000;
}
for(int i = 0; i < 1*C_range_a*C_range_b*C_range_c*C_range_d*C_range_e*C_range_f; i++){
C1[i] = C[i] = rand()%1000;
}
//====




        double alpha = 1.0;
    double beta = 1.0;
    static tsc_counter a,b;
    CPUID(); RDTSC(a); CPUID(); RDTSC(b);
    /* warm up */
    CPUID(); RDTSC(a); CPUID(); RDTSC(b);
    /* warm up */
    CPUID(); RDTSC(a); CPUID(); RDTSC(b);

        int tot_runtime = 5;
//;MPI_Init(NULL,NULL);
//MPI_Barrier(MPI_COMM_WORLD);
        double start = omp_get_wtime();
    RDTSC(a);
    for(int ccnt = 0; ccnt < tot_runtime; ccnt++){



// Abuf = (double*)memalign(4096, sizeof(double) * A_range_d * A_range_e * A_range_g * A_range_a);
        Abuf = (double*)memalign(4096, sizeof(double) * Tk3*Tm3);
int ext_range_e = A_range_e % Tm0;
int base_range_e = A_range_e - ext_range_e;
int base_m_range = base_range_e * A_range_d * A_range_a;
unsigned long long *Ascatter = (unsigned long long*)memalign(4096, sizeof(unsigned long long)*A_range_a*A_range_d*A_range_e);

int A_stride_d = 1 * A_range_e * A_range_g * A_range_a;
int A_stride_e = 1 * A_range_g * A_range_a;
int A_stride_g = 1 * A_range_a;
int A_stride_a = 1;

int Fused_A_stride_a = 1 * A_range_d * base_range_e;
int Fused_A_stride_d = 1 * base_range_e;
int Fused_A_stride_e = 1;
int Ext_A_stride_a = 1 * A_range_d * ext_range_e;
int Ext_A_stride_d = 1 * ext_range_e;
int Ext_A_stride_e = 1;

for(int Im=0; Im < base_m_range;Im++){
int Ia = Im / Fused_A_stride_a;
int Id = Im % Fused_A_stride_a / Fused_A_stride_d;
int Ie = Im % Fused_A_stride_d;
Ascatter[Im] = +Ia*C_range_b*C_range_c*C_range_d*C_range_e*C_range_f+Id*C_range_e*C_range_f+Ie*C_range_f;
}
for(int Im=base_m_range; Im < Tm4;Im++){
int Ia = (Im - base_m_range) / Ext_A_stride_a;
int Id = (Im - base_m_range)  % Ext_A_stride_a / Ext_A_stride_d;
int Ie = (Im - base_m_range) % Ext_A_stride_d+ base_range_e;
Ascatter[Im] = +Ia*C_range_b*C_range_c*C_range_d*C_range_e*C_range_f+Id*C_range_e*C_range_f+Ie*C_range_f;
}

//Bbuf = (double*)memalign(4096, sizeof(double) * B_range_g * B_range_f * B_range_b * B_range_c);
Bbuf = (double*)memalign(4096, sizeof(double) * Tn3*Tk3);
int ext_range_f = B_range_f % Tn0;
int base_range_f = B_range_f - ext_range_f;
int base_n_range = base_range_f * B_range_c * B_range_b;
unsigned long long *Bscatter = (unsigned long long*)memalign(4096, sizeof(unsigned long long)*B_range_b*B_range_c*B_range_f);

int B_stride_g = 1 * B_range_f * B_range_b * B_range_c;
int B_stride_f = 1 * B_range_b * B_range_c;
int B_stride_b = 1 * B_range_c;
int B_stride_c = 1;

int Fused_B_stride_b = 1 * B_range_c * base_range_f;
int Fused_B_stride_c = 1 * base_range_f;
int Fused_B_stride_f = 1;
int Ext_B_stride_b = 1 * B_range_c * ext_range_f;
int Ext_B_stride_c = 1 * ext_range_f;
int Ext_B_stride_f = 1;

for(int In=0; In < base_n_range;In++){
int Ib = In / Fused_B_stride_b;
int Ic = In % Fused_B_stride_b / Fused_B_stride_c;
int If = In % Fused_B_stride_c;
Bscatter[In] = +Ib*C_range_c*C_range_d*C_range_e*C_range_f+Ic*C_range_d*C_range_e*C_range_f+If;
}
for(int In=base_n_range; In < Tn4;In++){
int Ib = (In - base_n_range) / Ext_B_stride_b;
int Ic = (In - base_n_range)  % Ext_B_stride_b / Ext_B_stride_c;
int If = (In - base_n_range) % Ext_B_stride_c+ base_range_f;
Bscatter[In] = +Ib*C_range_c*C_range_d*C_range_e*C_range_f+Ic*C_range_d*C_range_e*C_range_f+If;
}

/* for(int k4 = 0; k4 < Tk4; k4 += Tk3) */
/* { */
/* for(int m4 = 0; m4 < Tm4; m4 += Tm3) */
/* { */
/* for(int k3 = 0; k3 < Tk3; k3 += Tk2) */
/* { */
/* if(+k3+k4>=Tk4)break; */
/* for(int m3 = 0; m3 < Tm3; m3 += Tm2) */
/* { */
/* if(+m3+m4>=Tm4)break; */
/* for(int k2 = 0; k2 < Tk2; k2 += Tk1) */
/* { */
/* if(+k2+k3+k4>=Tk4)break; */
/* for(int m2 = 0; m2 < Tm2; m2 += Tm1) */
/* { */
/* if(+m2+m3+m4>=Tm4)break; */
/* for(int m1 = 0; m1 < Tm1; m1 += Tm0) */
/* { */
/* if(+m1+m2+m3+m4>=Tm4)break; */
/* for(int k1 = 0; k1 < Tk1; k1 += Tk0) */
/* { */
/* if(+k1+k2+k3+k4>=Tk4)break; */
/* for(int m0 = 0; m0 < Tm0; m0 += 1) */
/* { */
/* if(+m0+m1+m2+m3+m4>=Tm4)break; */
/* // begin pack body */

/* int Im = m4+m3+m2+m1+m0;        int Ig = k1+k2+k3+k4; */


/* int A_pack_offset =  + m0 + k1* Tm0 + m1* MIN(Tk1, Tk4-k2-k3-k4) + m2* MIN(Tk1, Tk4-k2-k3-k4) + k2* MIN(Tm2, Tm4-m3-m4) + m3* MIN(Tk2, Tk4-k3-k4) + k3* MIN(Tm3, Tm4-m4) + m4* MIN(Tk3, Tk4-k4) + k4* MIN(Tm4, Tm4); */
/* if(Im< base_m_range){ */
/* int Ia = Im / Fused_A_stride_a; */
/* int Id = Im % Fused_A_stride_a / Fused_A_stride_d; */
/* int Ie = Im % Fused_A_stride_d; */
/* Abuf[A_pack_offset] = A[ +Id*A_stride_d +Ie*A_stride_e +Ig*A_stride_g +Ia*A_stride_a]; */
/* } */
/* else{ */
/* int Ia = (Im - base_m_range) / Ext_A_stride_a; */
/* int Id = (Im - base_m_range)  % Ext_A_stride_a / Ext_A_stride_d; */
/* int Ie = (Im - base_m_range) % Ext_A_stride_d+ base_range_e; */
/* Abuf[A_pack_offset] = A[ +Id*A_stride_d +Ie*A_stride_e +Ig*A_stride_g +Ia*A_stride_a]; */
/* } */
/* // end pack body */
/* } */
/* } */
/* } */
/* } */
/* } */
/* } */
/* } */
/* } */
/* } */

/* for(int k4 = 0; k4 < Tk4; k4 += Tk3) */
/* { */
/* for(int n4 = 0; n4 < Tn4; n4 += Tn3) */
/* { */
/* for(int k3 = 0; k3 < Tk3; k3 += Tk2) */
/* { */
/* if(+k3+k4>=Tk4)break; */
/* for(int n3 = 0; n3 < Tn3; n3 += Tn2) */
/* { */
/* if(+n3+n4>=Tn4)break; */
/* for(int k2 = 0; k2 < Tk2; k2 += Tk1) */
/* { */
/* if(+k2+k3+k4>=Tk4)break; */
/* for(int n2 = 0; n2 < Tn2; n2 += Tn1) */
/* { */
/* if(+n2+n3+n4>=Tn4)break; */
/* for(int n1 = 0; n1 < Tn1; n1 += Tn0) */
/* { */
/* if(+n1+n2+n3+n4>=Tn4)break; */
/* for(int k1 = 0; k1 < Tk1; k1 += Tk0) */
/* { */
/* if(+k1+k2+k3+k4>=Tk4)break; */
/* for(int n0 = 0; n0 < Tn0; n0 += 1) */
/* { */
/* if(+n0+n1+n2+n3+n4>=Tn4)break; */
/*     int In = n4+n3+n2+n1+n0;int Ig = k1+k2+k3+k4; */
/* // begin pack body */
/* int B_pack_offset =  + n0 + k1* Tn0 + n1* MIN(Tk1, Tk4-k2-k3-k4) + n2* MIN(Tk1, Tk4-k2-k3-k4) + k2* MIN(Tn2, Tn4-n3-n4) + n3* MIN(Tk2, Tk4-k3-k4) + k3* MIN(Tn3, Tn4-n4) + n4* MIN(Tk3, Tk4-k4) + k4* MIN(Tn4, Tn4); */
/* if(In< base_n_range){ */
/* int Ib = In / Fused_B_stride_b; */
/* int Ic = In % Fused_B_stride_b / Fused_B_stride_c; */
/* int If = In % Fused_B_stride_c; */
/* Bbuf[B_pack_offset] = B[ +Ig*B_stride_g +If*B_stride_f +Ib*B_stride_b +Ic*B_stride_c]; */
/* } */
/* else{ */
/* int Ib = (In - base_n_range) / Ext_B_stride_b; */
/* int Ic = (In - base_n_range)  % Ext_B_stride_b / Ext_B_stride_c; */
/* int If = (In - base_n_range) % Ext_B_stride_c+ base_range_f; */
/* Bbuf[B_pack_offset] = B[ +Ig*B_stride_g +If*B_stride_f +Ib*B_stride_b +Ic*B_stride_c]; */
/* } */
/* // end pack body */
/* } */
/* } */
/* } */
/* } */
/* } */
/* } */
/* } */
/* } */
/* } */






for(int k4 = 0; k4 < Tk4; k4 += Tk3)
{
    

for(int n4 = 0; n4 < Tn4; n4 += Tn3)
{

    for(int k3 = 0; k3 < Tk3; k3 += Tk2)
{
if(+k3+k4>=Tk4)break;
for(int n3 = 0; n3 < Tn3; n3 += Tn2)
{
if(+n3+n4>=Tn4)break;
for(int k2 = 0; k2 < Tk2; k2 += Tk1)
{
if(+k2+k3+k4>=Tk4)break;
for(int n2 = 0; n2 < Tn2; n2 += Tn1)
{
if(+n2+n3+n4>=Tn4)break;
for(int n1 = 0; n1 < Tn1; n1 += Tn0)
{
if(+n1+n2+n3+n4>=Tn4)break;
for(int k1 = 0; k1 < Tk1; k1 += Tk0)
{
if(+k1+k2+k3+k4>=Tk4)break;
for(int n0 = 0; n0 < Tn0; n0 += 1)
{
if(+n0+n1+n2+n3+n4>=Tn4)break;
    int In = n4+n3+n2+n1+n0;int Ig = k1+k2+k3+k4;
// begin pack body
    int B_pack_offset =  + n0 + k1* Tn0 + n1* MIN(Tk1, Tk4-k2-k3-k4) + n2* MIN(Tk1, Tk4-k2-k3-k4) + k2* MIN(Tn2, Tn4-n3-n4) + n3* MIN(Tk2, Tk4-k3-k4) + k3* MIN(Tn3, Tn4-n4) ;//+ n4* MIN(Tk3, Tk4-k4) + k4* MIN(Tn4, Tn4);
if(In< base_n_range){
int Ib = In / Fused_B_stride_b;
int Ic = In % Fused_B_stride_b / Fused_B_stride_c;
int If = In % Fused_B_stride_c;
Bbuf[B_pack_offset] = B[ +Ig*B_stride_g +If*B_stride_f +Ib*B_stride_b +Ic*B_stride_c];
}
else{
int Ib = (In - base_n_range) / Ext_B_stride_b;
int Ic = (In - base_n_range)  % Ext_B_stride_b / Ext_B_stride_c;
int If = (In - base_n_range) % Ext_B_stride_c+ base_range_f;
Bbuf[B_pack_offset] = B[ +Ig*B_stride_g +If*B_stride_f +Ib*B_stride_b +Ic*B_stride_c];
}
// end pack body
}
}
}
}
}
}
}
    
    
for(int m4 = 0; m4 < Tm4; m4 += Tm3)
{

    for(int k3 = 0; k3 < Tk3; k3 += Tk2)
{
if(+k3+k4>=Tk4)break;
for(int m3 = 0; m3 < Tm3; m3 += Tm2)
{
if(+m3+m4>=Tm4)break;
for(int k2 = 0; k2 < Tk2; k2 += Tk1)
{
if(+k2+k3+k4>=Tk4)break;
for(int m2 = 0; m2 < Tm2; m2 += Tm1)
{
if(+m2+m3+m4>=Tm4)break;
for(int m1 = 0; m1 < Tm1; m1 += Tm0)
{
if(+m1+m2+m3+m4>=Tm4)break;
for(int k1 = 0; k1 < Tk1; k1 += Tk0)
{
if(+k1+k2+k3+k4>=Tk4)break;
for(int m0 = 0; m0 < Tm0; m0 += 1)
{
if(+m0+m1+m2+m3+m4>=Tm4)break;
// begin pack body

int Im = m4+m3+m2+m1+m0;        int Ig = k1+k2+k3+k4;


int A_pack_offset =  + m0 + k1* Tm0 + m1* MIN(Tk1, Tk4-k2-k3-k4) + m2* MIN(Tk1, Tk4-k2-k3-k4) + k2* MIN(Tm2, Tm4-m3-m4) + m3* MIN(Tk2, Tk4-k3-k4) + k3* MIN(Tm3, Tm4-m4);// + m4* MIN(Tk3, Tk4-k4) + k4* MIN(Tm4, Tm4);
if(Im< base_m_range){
int Ia = Im / Fused_A_stride_a;
int Id = Im % Fused_A_stride_a / Fused_A_stride_d;
int Ie = Im % Fused_A_stride_d;
Abuf[A_pack_offset] = A[ +Id*A_stride_d +Ie*A_stride_e +Ig*A_stride_g +Ia*A_stride_a];
}
else{
int Ia = (Im - base_m_range) / Ext_A_stride_a;
int Id = (Im - base_m_range)  % Ext_A_stride_a / Ext_A_stride_d;
int Ie = (Im - base_m_range) % Ext_A_stride_d+ base_range_e;
Abuf[A_pack_offset] = A[ +Id*A_stride_d +Ie*A_stride_e +Ig*A_stride_g +Ia*A_stride_a];
}
// end pack body
}
}
}
}
}
}
}

for(int k3 = 0; k3 < Tk3; k3 += Tk2)
{
if(+k3+k4>=Tk4)break;

for(int m3 = 0; m3 < Tm3; m3 += Tm2)
{
if(+m3+m4>=Tm4)break;
for(int n3 = 0; n3 < Tn3; n3 += Tn2)
{
if(+n3+n4>=Tn4)break;
for(int k2 = 0; k2 < Tk2; k2 += Tk1)
{
if(+k2+k3+k4>=Tk4)break;

for(int m2 = 0; m2 < Tm2; m2 += Tm1)
{
if(+m2+m3+m4>=Tm4)break;
for(int n2 = 0; n2 < Tn2; n2 += Tn1)
{
if(+n2+n3+n4>=Tn4)break;
for(int m1 = 0; m1 < Tm1; m1 += Tm0)
{
if(+m1+m2+m3+m4>=Tm4)break;
for(int n1 = 0; n1 < Tn1; n1 += Tn0)
{
if(+n1+n2+n3+n4>=Tn4)break;
    int Im = m1+m2+m3+m4;
    int In = n1+n2+n3+n4;
    int A_pack_offset = // + m0 + k1* Tm0
    
        + m1* MIN(Tk1, Tk4-k2-k3-k4) + m2* MIN(Tk1, Tk4-k2-k3-k4) + k2* MIN(Tm2, Tm4-m3-m4) + m3* MIN(Tk2, Tk4-k3-k4) + k3* MIN(Tm3, Tm4-m4) ;//+ m4* MIN(Tk3, Tk4-k4) + k4* MIN(Tm4, Tm4);
    int B_pack_offset =  //+ n0 + k1* Tn0
        + n1* MIN(Tk1, Tk4-k2-k3-k4) + n2* MIN(Tk1, Tk4-k2-k3-k4) + k2* MIN(Tn2, Tn4-n3-n4) + n3* MIN(Tk2, Tk4-k3-k4) + k3* MIN(Tn3, Tn4-n4);// + n4* MIN(Tk3, Tk4-k4) + k4* MIN(Tn4, Tn4);
    
    int use_k = MIN(Tk1, Tk4-k2-k3-k4);
// begin compute body
if( Im < base_m_range && In < base_n_range){
bli_dgemm_haswell_asm_6x8(use_k,  &alpha, Abuf+ A_pack_offset, Bbuf+B_pack_offset, &beta, C+ Ascatter[Im] + Bscatter[In], 1*C_range_f, 1, NULL, NULL);
}
else if(Im >= base_m_range && In < base_n_range){
sct_dgemm_haswell_asm_6x8_8vec(use_k, &alpha, Abuf+ A_pack_offset, Bbuf+B_pack_offset, &beta, C + Bscatter[In], Ascatter+Im);
}
else if(Im < base_m_range && In >= base_n_range){
sct_dgemm_haswell_asm_6x8(use_k, &alpha, Abuf+ A_pack_offset, Bbuf+B_pack_offset, &beta, C, Ascatter+Im, Bscatter+In);
}
else{
}
// end compute body
}
}
}
}
}
}
}
}
}
}
}




    }//end ccnt
//MPI_Barrier(MPI_COMM_WORLD);
        RDTSC(b);
        double  runtime = omp_get_wtime() - start;
        printf("runtime packAB =, %lf,",runtime);
 
    long long cycles_tuned = (long)(((double) (COUNTER_DIFF_SIMPLE(b,a))) / ((long long) 1));
double flop_count = 2.0 * A_range_a * B_range_b *B_range_c
            *A_range_d * A_range_e * B_range_f *A_range_g*tot_runtime;
    double flops_cycle_tuned = ((double)flop_count)/((double)cycles_tuned);
printf("fpc, %2.4f,", flops_cycle_tuned);
printf("gflops,  %lf\n", flop_count/1000/1000/1000/runtime);

    
     
    
    

    for(int i=0;i<tot_runtime;i++)
    compare_fun(A, B, C1);

    for(int i=0; i< A_range_a * B_range_b *B_range_c
            *A_range_d * A_range_e * B_range_f ;i++){
//        if(C[i] != C1[i]){
        if( C[i] != C1[i]){
            //printf("error at C[%d], C %lf, C1 %lf\n", i, C[i], C1[i]);
                return 0;
        }
    }
    printf("correct\n");

return 0;
}