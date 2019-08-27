#include "../../tensor_kernels.h"
#include "../../scatter_kernels.h"
#include<omp.h>
#include"mpi.h"

int B_range_a=312;
int A_range_b=312;
int B_range_c=312;

int A_range_d=312;
int B_range_d=312;

int C_range_a=312;
int C_range_b=312;
int C_range_c=312;
int C_range_d=312;


void compare_fun(double* A, double*B,double*C ){
for(int a=0; a<C_range_a; a++)
for(int b=0; b<C_range_b; b++)
for(int c=0; c<C_range_c; c++)
for(int d=0; d<C_range_d; d++)
C[+a* C_range_b* C_range_c+b* C_range_c+c] += 
A[+d* C_range_b+b] * 
B[+a* C_range_c* C_range_d+c* C_range_d+d]; 
}

int main(int argc, char** argv){

  int Tk4, Tk3, Tk2, Tk1, Tk0;
    int Tm4, Tm3, Tm2, Tm1, Tm0;
    int Tn4, Tn3, Tn2, Tn1, Tn0;

    printf("abc-acd-db\n");
    Tk4 = C_range_d;
    Tk3 = 312;
    Tk2 = Tk1 = 256;
    Tk0 = 1;

    Tm4 = A_range_b;
    Tn4 = B_range_a*B_range_c;

    Tm0 = 6; Tn0=8;

    Tm1 = 6; Tn1 = 8;
    /* Tm2 = 96; Tn2 = 16; */
    /* Tm3 = 312; Tn3 = 2271/16*16; */
    Tm2 = 96; Tn2 = 8;
    Tm3 = 312; Tn3 = 112;
    
    double *A, *B, *C, *Abuf, *Bbuf;    
    C = (double*)memalign(4096, sizeof(double) * C_range_a * C_range_b *C_range_c );
    double* C1 = (double*)memalign(4096, sizeof(double) * C_range_a * C_range_b *C_range_c );

A = (double*)memalign(4096, sizeof(double) * A_range_d * A_range_b);
B = (double*)memalign(4096, sizeof(double) * B_range_a * B_range_c * B_range_d);
for(int i = 0; i < 1*A_range_d*A_range_b; i++){
A[i] = rand()%1000;
}
for(int i = 0; i < 1*B_range_a*B_range_c*B_range_d; i++){
B[i] = rand()%1000;
}
for(int i = 0; i < 1*C_range_a*C_range_b*C_range_c; i++){
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
;MPI_Init(NULL,NULL);
MPI_Barrier(MPI_COMM_WORLD);
        
    RDTSC(a);
            double start = omp_get_wtime();
    for(int ccnt = 0; ccnt < tot_runtime; ccnt++){


//Abuf = (double*)memalign(4096, sizeof(double) * A_range_d * A_range_b);
        Abuf = (double*)memalign(4096, sizeof(double)*Tm3*Tk3);
int ext_range_b = A_range_b % Tm0;
int base_range_b = A_range_b - ext_range_b;
int base_m_range = base_range_b;
unsigned long long *Ascatter = (unsigned long long*)memalign(4096, sizeof(unsigned long long)*A_range_b);

int A_stride_d = 1 * A_range_b;
int A_stride_b = 1;

int Fused_A_stride_b = 1;
int Ext_A_stride_b = 1;

for(int Im=0; Im < base_m_range;Im++){
int Ib = Im / Fused_A_stride_b;
Ascatter[Im] = +Ib*C_range_c;
}
for(int Im=base_m_range; Im < Tm4;Im++){
int Ib = (Im - base_m_range) / Ext_A_stride_b;
Ascatter[Im] = +Ib*C_range_c;
}

//Bbuf = (double*)memalign(4096, sizeof(double) * B_range_a * B_range_c * B_range_d);
 Bbuf = (double*)memalign(4096, sizeof(double) * Tn3*Tk3);
int ext_range_c = B_range_c % Tn0;
int base_range_c = B_range_c - ext_range_c;
int base_n_range = base_range_c * B_range_a;
unsigned long long *Bscatter = (unsigned long long*)memalign(4096, sizeof(unsigned long long)*B_range_a*B_range_c);

int B_stride_a = 1 * B_range_c * B_range_d;
int B_stride_c = 1 * B_range_d;
int B_stride_d = 1;

int Fused_B_stride_a = 1 * base_range_c;
int Fused_B_stride_c = 1;
int Ext_B_stride_a = 1 * ext_range_c;
int Ext_B_stride_c = 1;

for(int In=0; In < base_n_range;In++){
int Ia = In / Fused_B_stride_a;
int Ic = In % Fused_B_stride_a;
Bscatter[In] = +Ia*C_range_b*C_range_c+Ic;
}
for(int In=base_n_range; In < Tn4;In++){
int Ia = (In - base_n_range) / Ext_B_stride_a;
int Ic = (In - base_n_range) % Ext_B_stride_a+ base_range_c;
Bscatter[In] = +Ia*C_range_b*C_range_c+Ic;
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
/* //for(int m0 = 0; m0 < Tm0; m0 += 1) */
/* { */
/* //if(+m0+m1+m2+m3+m4>=Tm4)break; */
/* int Im  = +m1+m2+m3+m4; */
/* int Id = k1+k2+k3+k4; */
/* // begin pack body */
/* int A_pack_offset =   + k1* Tm0 + m1* MIN(Tk1, Tk4-k2-k3-k4) + m2* MIN(Tk1, Tk4-k2-k3-k4) + k2* MIN(Tm2, Tm4-m3-m4) + m3* MIN(Tk2, Tk4-k3-k4) + k3* MIN(Tm3, Tm4-m4);// + m4* MIN(Tk3, Tk4-k4) + k4* MIN(Tm4, Tm4); */
/* if(Im+6<= base_m_range){ */
/* int Ib = Im / Fused_A_stride_b; */
/* for(int ii=0;ii<6;ii++) */
/* Abuf[A_pack_offset+ii] = A[ +Id*A_stride_d +(Ib+ii)*A_stride_b]; */
/* } */
/* else{ */
/* int Ib = (Im - base_m_range) / Ext_A_stride_b; */
/* Abuf[A_pack_offset] = A[ +Id*A_stride_d +Ib*A_stride_b]; */
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
//for(int m0 = 0; m0 < Tm0; m0 += 1)
{
//if(+m0+m1+m2+m3+m4>=Tm4)break;
int Im  = +m1+m2+m3+m4;
int Id = k1+k2+k3+k4;
// begin pack body
int A_pack_offset =   + k1* Tm0 + m1* MIN(Tk1, Tk4-k2-k3-k4) + m2* MIN(Tk1, Tk4-k2-k3-k4) + k2* MIN(Tm2, Tm4-m3-m4) + m3* MIN(Tk2, Tk4-k3-k4) + k3* MIN(Tm3, Tm4-m4);// + m4* MIN(Tk3, Tk4-k4) + k4* MIN(Tm4, Tm4);
if(Im+6<= base_m_range){
int Ib = Im / Fused_A_stride_b;
for(int ii=0;ii<6;ii++)
Abuf[A_pack_offset+ii] = A[ +Id*A_stride_d +(Ib+ii)*A_stride_b];
}
else{
int Ib = (Im - base_m_range) / Ext_A_stride_b;
Abuf[A_pack_offset] = A[ +Id*A_stride_d +Ib*A_stride_b];
}
// end pack body
}
}
}
}
}
}
}

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
//            printf("packb in k2\n");
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
//for(int n0 = 0; n0 < Tn0; n0 += 1)
{
//if(+n0+n1+n2+n3+n4>=Tn4)break;
// begin pack body

    int In = +n1+n2+n3+n4;
    int Id = k1+k2+k3+k4;

    int B_pack_offset =  + k1* Tn0 + n1* MIN(Tk1, Tk4-k2-k3-k4) + n2* MIN(Tk1, Tk4-k2-k3-k4) + k2* MIN(Tn2, Tn4-n3-n4) + n3* MIN(Tk2, Tk4-k3-k4) + k3* MIN(Tn3, Tn4-n4);// + n4* MIN(Tk3, Tk4-k4) + k4* MIN(Tn4, Tn4);
if(In+8<= base_n_range){
int Ia = In / Fused_B_stride_a;
int Ic = In % Fused_B_stride_a;
for(int jj=0;jj<8;jj++)
Bbuf[B_pack_offset+jj] = B[ +Ia*B_stride_a +(Ic+jj)*B_stride_c +Id*B_stride_d];
}
else{
int Ia = (In - base_n_range) / Ext_B_stride_a;
int Ic = (In - base_n_range) % Ext_B_stride_a+ base_range_c;
Bbuf[B_pack_offset] = B[ +Ia*B_stride_a +Ic*B_stride_c +Id*B_stride_d];
}
// end pack body
}
}
}
}
}
}
}


//    printf("comp\n");
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


for(int n2 = 0; n2 < Tn2; n2 += Tn1)
{
if(+n2+n3+n4>=Tn4)break;
for(int m2 = 0; m2 < Tm2; m2 += Tm1)
{
if(+m2+m3+m4>=Tm4)break;
for(int m1 = 0; m1 < Tm1; m1 += Tm0)
{
if(+m1+m2+m3+m4>=Tm4)break;
for(int n1 = 0; n1 < Tn1; n1 += Tn0)
{
if(+n1+n2+n3+n4>=Tn4)break;
    int Im = m1+m2+m3+m4;
    int In = n1+n2+n3+n4;
    int use_k = MIN(Tk1, Tk4-k2-k3-k4);
int A_pack_offset =   + m1* MIN(Tk1, Tk4-k2-k3-k4) + m2* MIN(Tk1, Tk4-k2-k3-k4) + k2* MIN(Tm2, Tm4-m3-m4) + m3* MIN(Tk2, Tk4-k3-k4) + k3* MIN(Tm3, Tm4-m4);// + m4* MIN(Tk3, Tk4-k4) + k4* MIN(Tm4, Tm4);
    int B_pack_offset = 
        + n1* MIN(Tk1, Tk4-k2-k3-k4) + n2* MIN(Tk1, Tk4-k2-k3-k4) + k2* MIN(Tn2, Tn4-n3-n4) + n3* MIN(Tk2, Tk4-k3-k4) + k3* MIN(Tn3, Tn4-n4);// + n4* MIN(Tk3, Tk4-k4) + k4* MIN(Tn4, Tn4);
// begin compute body
if( Im < base_m_range && In < base_n_range){
bli_dgemm_haswell_asm_6x8(use_k,  &alpha, Abuf+ A_pack_offset, Bbuf+B_pack_offset, &beta, C+ Ascatter[Im] + Bscatter[In], 1*C_range_c, 1, NULL, NULL);
}
else if(Im >= base_m_range && In < base_n_range){
sct_dgemm_haswell_asm_6x8_8vec(use_k, &alpha, Abuf+ A_pack_offset, Bbuf+B_pack_offset, &beta, C + Bscatter[In], Ascatter+Im);
}
else if(Im < base_m_range && In >= base_n_range){
sct_dgemm_haswell_asm_6x8(use_k, &alpha, Abuf+ A_pack_offset, Bbuf+B_pack_offset, &beta, C, Ascatter+Im, Bscatter+In);
}
else{
    printf("exit cop\n");
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
MPI_Barrier(MPI_COMM_WORLD);
        RDTSC(b);
        double  runtime = omp_get_wtime() - start;
    double flop_count = 2.0 * C_range_a * C_range_b *C_range_c
            *C_range_d  *tot_runtime;
    printf("gflops,  %lf\n", flop_count/1000/1000/1000/runtime);
    long long cycles_tuned = (long)(((double) (COUNTER_DIFF_SIMPLE(b,a))) / ((long long) 1));
     
    double flops_cycle_tuned = ((double)flop_count)/((double)cycles_tuned);
    
printf("fpc, %2.4f\n,", flops_cycle_tuned);

    for(int i=0;i<tot_runtime;i++)
    compare_fun(A, B, C1);

    for(int i=0; i< C_range_a * C_range_b *C_range_c  ;i++){
//        if(C[i] != C1[i]){
        if( C[i] != C1[i]){
            //printf("error at C[%d], C %lf, C1 %lf\n", i, C[i], C1[i]);
                return 0;
        }
    }
    printf("correct\n");

return 0;
}