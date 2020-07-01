#include "../FourQ_api.h"
#include "../FourQ_internal.h"
#include "../FourQ_params.h"
#include "../random.h"
#include "test_extras.h"
#include<stdio.h>
#include<string.h>
#include <string.h>
#include <time.h>

#include <stdlib.h>
#include <time.h>
#include "blake2.h"
#include <math.h>


// Benchmark and test parameters 
#define BENCH_LOOPS       10000      // Number of iterations per bench

unsigned int countDistinct(unsigned char arr[], int n);
char * toArray(int number);
void MVectorFill(unsigned char MV[],unsigned char arr[], int n);
int main()
{
  unsigned int t = 1024; 
  unsigned char msg[1024];


   for(int gg = 0; gg < t; gg++) {
   		msg[gg] = gg%255;
   }
   
   printf("Starting CORE Key Generation\n");

   clock_t myS = clock();

   //Please change Random Bytes 16
   unsigned char x1[16];
   RandomBytesFunction(x1,16);
   unsigned char R_Prime[32],Y_Prime[32];
    //Choose a random y_1 in 0 < y_1 < q
   unsigned char y_1[32],yTemp[32],yTemp2[32],yTemporary[32];
    unsigned char r_1[32], rTemp[32],rTemp2[32],rTemporary[32];

   point_t R_1t;

   point_t h_1t;

   for (unsigned int n = 0; n<BENCH_LOOPS; n++)
   {
      
      RandomBytesFunction(y_1,32);
      modulo_order((digit_t*)y_1,(digit_t*)y_1);//y_1 mod q
      memcpy(yTemp,y_1,32);
      memcpy(yTemp2,y_1,32);

      //Choose a random r_1 in 0 < r_1 < q
      
        RandomBytesFunction(r_1,32);
        modulo_order((digit_t*)r_1,(digit_t*)r_1);//r_1 mod q
        memcpy(rTemp,r_1,32);
        memcpy(rTemp2,r_1,32);

      // ecc_mul_fixed((digit_t*)yTemp,h_1t);
      
        memcpy(yTemporary,yTemp,32);
        blake2s(yTemp, yTemp, NULL, 32, 32, 0);
        modulo_order((digit_t*)yTemp,(digit_t*)yTemp);
        add_mod_order((digit_t*)yTemporary,(digit_t*)yTemp,(digit_t*)yTemporary);
        for (unsigned int g = 1; g < t-1; g++) {
          blake2s(yTemp, yTemp, NULL, 32, 32, 0);
          modulo_order((digit_t*)yTemp,(digit_t*)yTemp);
          add_mod_order((digit_t*)yTemporary,(digit_t*)yTemp,(digit_t*)yTemporary);
        }
        ecc_mul_fixed((digit_t*)yTemporary,h_1t);

        memcpy(rTemporary,rTemp,32);
        blake2s(rTemp, rTemp, NULL, 32, 32, 0);
        modulo_order((digit_t*)rTemp,(digit_t*)rTemp);
        add_mod_order((digit_t*)rTemporary,(digit_t*)rTemp,(digit_t*)rTemporary);
        for (unsigned int g = 1; g < t-1; g++) {
          blake2s(rTemp, rTemp, NULL, 32, 32, 0);
          modulo_order((digit_t*)rTemp,(digit_t*)rTemp);
          add_mod_order((digit_t*)rTemporary,(digit_t*)rTemp,(digit_t*)rTemporary);
        }
        ecc_mul_fixed((digit_t*)rTemporary,R_1t);
        blake2s(Y_Prime, h_1t, NULL, 32, 32, 0);
        blake2s(R_Prime, R_1t, NULL, 32, 32, 0);
   }
  clock_t endS = clock();
  float secondsS = ((float)(endS - myS) / CLOCKS_PER_SEC)/BENCH_LOOPS;
  printf("Key generation runs in %f miliseconds\n", (secondsS*1000));

   printf("CORE Key Generation Completed!\n");
 clock_t start = clock();
  //Initial Values
   unsigned int  i = 1, j = 1, j_Prime = 1, c = 0,pVecCount = 0;
   //To have a similar functionality as if we are using ArrayLists in Java
   unsigned char MVector[countDistinct(msg, t)];
   unsigned char PVector[countDistinct(msg, t)];
   PVector[0] = j_Prime;
   MVectorFill(MVector,msg,t);
   point_t h_iterationArr[countDistinct(msg, t)]; //Sum of All Pks
   unsigned int jPrimeArr[countDistinct(msg, t)];
   unsigned char hashConcatDeg[32] = {0},hashConcat[32] = {0},s_jTotal[32] = {0};
  for(unsigned int fff = 0; fff < BENCH_LOOPS; fff++) {
      
   i = 1; j = 1; j_Prime = 1; c = 0; pVecCount = 0;
   //To have a similar functionality as if we are using ArrayLists in Java
   memset(MVector, 0, sizeof(MVector));
   memset(PVector, 0, sizeof(PVector));
   PVector[0] = j_Prime;
   MVectorFill(MVector,msg,t);
   memset(h_iterationArr, 0, sizeof(h_iterationArr));
   memset(jPrimeArr, 0, sizeof(jPrimeArr));
   memset(hashConcatDeg, 0, sizeof(hashConcatDeg));
   memset(hashConcat, 0, sizeof(hashConcat));
   memset(s_jTotal, 0, sizeof(s_jTotal));

   memcpy(yTemp2,y_1,32);
   memcpy(rTemp2,r_1,32);

    
    for(unsigned int iteration = 0; iteration < t; iteration++) {
        point_t prevH;
        if(j > t) {
          break;
        } 
        else if(c == 1) {
          j_Prime = j;
          pVecCount++;
          //Calculating y_j to t
          unsigned char y_jtot[32];
          memcpy(y_jtot,yTemp2,32);
          unsigned char r_jtot[32];
          memcpy(r_jtot,rTemp2,32);
          for(unsigned int h = 0; h < t; h++) {
            blake2s(yTemp2, yTemp2, NULL, 32, 32, 0);
            modulo_order((digit_t*)yTemp2,(digit_t*)yTemp2);
            add_mod_order((digit_t*)y_jtot,(digit_t*)yTemp2,(digit_t*)y_jtot);
          } 

          for(unsigned int h = 0; h < t; h++) {
            blake2s(rTemp2, rTemp2, NULL, 32, 32, 0);
            modulo_order((digit_t*)rTemp2,(digit_t*)rTemp2);
            add_mod_order((digit_t*)r_jtot,(digit_t*)rTemp2,(digit_t*)r_jtot);
          } 
          //Hash Preparation
          x1[0] = MVector[j-1];
          blake2s(hashConcat, x1, NULL, 32, 100, 0);
          //Calculate s_jtot
          unsigned char sj_tot[32];
          memcpy(sj_tot,r_jtot,sizeof(r_jtot));
          ecc_mul_fixed((digit_t*)y_jtot,h_iterationArr[j-1]);

          //Calculate h*y
          to_Montgomery((digit_t*)hashConcat,(digit_t*) hashConcat);
          to_Montgomery((digit_t*)y_jtot,(digit_t*) y_jtot);
            
          Montgomery_multiply_mod_order((digit_t*)y_jtot,(digit_t*) hashConcat,(digit_t*) hashConcat);
          from_Montgomery((digit_t*)hashConcat, (digit_t*)hashConcat);
          from_Montgomery((digit_t*)y_jtot, (digit_t*)y_jtot);

          subtract_mod_order((digit_t*)sj_tot,(digit_t*)hashConcat,(digit_t*)sj_tot);
          add_mod_order((digit_t*)s_jTotal,(digit_t*)sj_tot,(digit_t*)s_jTotal);
          modulo_order((digit_t*)s_jTotal,(digit_t*)s_jTotal);    
          i = i + 1;

          //Move directly into verificatiion for convenience (only for c = 1) situations

          if(c == 1 && 0 == iteration) {
            x1[0] =  MVector[0];
          }
          else {
            x1[0] = MVector[0] + PVector[0];
          }
            
          blake2s(hashConcat, x1, NULL, 32, 100, 0);
          point_t ProductResult;
          ecc_mul(h_iterationArr[0],(digit_t*)hashConcat,ProductResult,false);
          unsigned int temp2 = i;
          if(c == 1)
            temp2--;
          
          for(unsigned int gg = 1; gg < j; gg++) {
            point_extproj_t p1sigPoint,p2Extproj;
            point_extproj_precomp_t p1sigPointpoint_extproj_precomp_t;
            point_setup(ProductResult, p1sigPoint);
            R1_to_R2(p1sigPoint, p1sigPointpoint_extproj_precomp_t);   
            if(c == 1 && gg == j - 1) 
              x1[0] = MVector[gg];
          else
              x1[0] = MVector[gg] + PVector[gg];
            blake2s(hashConcat, x1, NULL, 32, 100, 0);

            point_t tempPT;
            memcpy(tempPT,h_iterationArr[gg],sizeof(tempPT));
            ecc_mul(tempPT,(digit_t*)hashConcat,tempPT,false);

            point_setup(tempPT, p2Extproj);
            eccadd(p1sigPointpoint_extproj_precomp_t, p2Extproj);
            eccnorm(p2Extproj, ProductResult);
          }


          point_t PKVectorSum;
          memcpy(PKVectorSum,h_iterationArr[0],sizeof(h_iterationArr[0]));
          unsigned int temp = i;
          if(c == 1)
            temp--;
          for(unsigned int gg = 1; gg < j; gg++) {
            point_extproj_t p1sigPoint,p2Extproj;
            point_extproj_precomp_t p1sigPointpoint_extproj_precomp_t;
            point_setup(PKVectorSum, p1sigPoint);
            R1_to_R2(p1sigPoint, p1sigPointpoint_extproj_precomp_t);   
            point_setup(h_iterationArr[gg], p2Extproj);
            eccadd(p1sigPointpoint_extproj_precomp_t, p2Extproj);
            eccnorm(p2Extproj, PKVectorSum);
          }

          if(memcmp(h_1t,PKVectorSum,sizeof(h_1t)) == 0) {
      		//printf("Passed");
          }
          else {
      		//printf("Second Check Failed!\n");
              return 0;
          }
          
          unsigned int sigSum[32];
          //Preping alpha^s1,t
          point_t sigPoint;
          ecc_mul_fixed((digit_t*)s_jTotal,sigPoint);


          point_t check3;
          point_extproj_t p1sigPoint,p2Extproj;
          point_extproj_precomp_t p1sigPointpoint_extproj_precomp_t;
          point_setup(sigPoint, p1sigPoint);
          R1_to_R2(p1sigPoint, p1sigPointpoint_extproj_precomp_t);   

          point_setup(ProductResult, p2Extproj);
          eccadd(p1sigPointpoint_extproj_precomp_t, p2Extproj);
          eccnorm(p2Extproj, check3);

          if(memcmp(check3,R_1t,sizeof(point_t)) == 0) {
      		//printf("Passed!\n");
          }
          else {
      		//printf("Third Check Failed!\n");
          }
          return 0;
        }
        else if(iteration != 0 && msg[iteration] != msg[iteration-1]) { //Since indexes start at 0
            //No need to change x1
            //Will change this up

            i = i + 1;
            j_Prime = j;
            pVecCount++;
            PVector[pVecCount] = j_Prime;
            
            x1[0] = MVector[(i-1)] + PVector[pVecCount];
            blake2s(hashConcat, x1, NULL, 32, 100, 0);
            memcpy(hashConcatDeg,hashConcat,32);

            unsigned char sig_j[32] = {0},yDig[32] = {0},ydeneme[32];
            memcpy(ydeneme,yTemp2,32);
          
            to_Montgomery((digit_t*)hashConcatDeg,(digit_t*) hashConcatDeg);
            to_Montgomery((digit_t*)yTemp2,(digit_t*) yDig);
            to_Montgomery((digit_t*)sig_j,(digit_t*) sig_j);
            Montgomery_multiply_mod_order((digit_t*)yDig,(digit_t*) hashConcatDeg,(digit_t*) sig_j);
            from_Montgomery((digit_t*)yDig, (digit_t*)yDig);
            from_Montgomery((digit_t*)sig_j, (digit_t*)sig_j);
            from_Montgomery((digit_t*)hashConcatDeg,(digit_t*) hashConcatDeg);
            unsigned char res[32] = {0};
            subtract_mod_order((digit_t*)rTemp2, (digit_t*)sig_j, (digit_t*)res);

            if(iteration == 0) {
              memcpy(s_jTotal,res,32);
              modulo_order((digit_t*)s_jTotal,(digit_t*)s_jTotal); 
            } else {
              add_mod_order((digit_t*)s_jTotal,(digit_t*)res,(digit_t*)s_jTotal);
              modulo_order((digit_t*)s_jTotal,(digit_t*)s_jTotal);
            }
         //   ecc_mul_fixed((digit_t*)yTemp2,h_iterationArr[i-1]);
           memcpy(h_iterationArr[i-1],prevH,sizeof(point_t));
      }
      else {
          x1[0] = MVector[(i-1)] + PVector[pVecCount];
          blake2s(hashConcat, x1, NULL, 32, 100, 0);
          memcpy(hashConcatDeg,hashConcat,32);
          unsigned char sig_j[32] = {0},yDig[32] = {0},ydeneme[32];
          memcpy(ydeneme,yTemp2,32);
          to_Montgomery((digit_t*)hashConcatDeg,(digit_t*) hashConcatDeg);
          to_Montgomery((digit_t*)yTemp2,(digit_t*) yDig);
          to_Montgomery((digit_t*)sig_j,(digit_t*) sig_j);
          Montgomery_multiply_mod_order((digit_t*)yDig,(digit_t*) hashConcatDeg,(digit_t*) sig_j);
          from_Montgomery((digit_t*)yDig, (digit_t*)yDig);
          from_Montgomery((digit_t*)sig_j, (digit_t*)sig_j);
          from_Montgomery((digit_t*)hashConcatDeg,(digit_t*) hashConcatDeg);
          unsigned char res[32] = {0};
          subtract_mod_order((digit_t*)rTemp2, (digit_t*)sig_j, (digit_t*)res);

          if(iteration == 0 && c != 1) {
            ecc_mul_fixed((digit_t*)yTemp2,h_iterationArr[0]);
            memcpy(s_jTotal,res,32);
            modulo_order((digit_t*)s_jTotal,(digit_t*)s_jTotal);  
          }
          else if (c != 1) {
            point_extproj_t Q;
            point_extproj_precomp_t K;
            point_setup(h_iterationArr[i-1],Q);
            R1_to_R2(Q,K);
            point_extproj_t h_1tQ;
            point_setup(prevH,h_1tQ);
            eccadd(K,h_1tQ);
            eccnorm(h_1tQ,h_iterationArr[i-1]);
            add_mod_order((digit_t*)s_jTotal,(digit_t*)res,(digit_t*)s_jTotal);
            modulo_order((digit_t*)s_jTotal,(digit_t*)s_jTotal);
          }
        }

        if(c != 1) {
          blake2s(yTemp2, yTemp2, NULL, 32, 32, 0);
          modulo_order((digit_t*)yTemp2,(digit_t*)yTemp2);
          ecc_mul_fixed((digit_t*)yTemp2,prevH);
          blake2s(rTemp2, rTemp2, NULL, 32, 32, 0);
          modulo_order((digit_t*)rTemp2,(digit_t*)rTemp2);  
          j = j + 1;
        }
   }
  }
  


clock_t end = clock();
float seconds = ((float)(end - start) / CLOCKS_PER_SEC)/BENCH_LOOPS;
printf("Signature generation runs in %f miliseconds\n", (seconds*1000));

   

printf("Signature generation is now over. Moving into Verification!\n");
clock_t startT = clock();

for(unsigned int g = 0; g < BENCH_LOOPS; g++) {
  if(j_Prime >= 1 && j_Prime <= i && i <= j && j-1 != t) {
   return 0;
  }
    
  unsigned char RSumHash[32];
  unsigned char hashConcat2[32];
  if(i == 1) { //Handling the case where all the messages are equal
    if(memcmp(h_1t,h_iterationArr[0],sizeof(h_1t)) == 0) {
     //    printf("Y' Check (Second Check) Passed!\n");
    }
    else {
    //  printf("Second Check Failed!\n");
      return 0;
    }       
    x1[0] = MVector[(i-1)] + PVector[0];
    blake2s(hashConcat2, x1, NULL, 32, 100, 0);
    point_t toBeComp;

    ecc_mul(h_iterationArr[0], (digit_t*) hashConcat2, toBeComp, false);
    unsigned int sigSum[32];

        //Preping alpha^s1,t

         
        point_t sigPoint;
        //toBeComp is hash and pk point
        ecc_mul_fixed((digit_t*)s_jTotal,sigPoint);
        point_t check3;
        point_extproj_t p1sigPoint,p2Extproj;
        point_extproj_precomp_t p1sigPointpoint_extproj_precomp_t;
        point_setup(sigPoint, p1sigPoint);
        R1_to_R2(p1sigPoint, p1sigPointpoint_extproj_precomp_t);   

        point_setup(toBeComp, p2Extproj);
        eccadd(p1sigPointpoint_extproj_precomp_t, p2Extproj);
        eccnorm(p2Extproj, check3);

        if(memcmp(check3,R_1t,sizeof(point_t)) == 0) {
      		//printf("Passed!\n");
        }
        else {
       		//printf("Failed!\n");
          return 0;
        }

    }
    else {
    // Different Message Situation Handling
      x1[0] = MVector[0] + PVector[0];
      blake2s(hashConcat, x1, NULL, 32, 100, 0);
      point_t ProductResult;
      memcpy(ProductResult,h_iterationArr[0],sizeof(ProductResult));
      ecc_mul(ProductResult,(digit_t*)hashConcat,ProductResult,false);
      
      for(unsigned int gg = 1; gg < i; gg++) {
        point_extproj_t p1sigPoint,p2Extproj;
        point_extproj_precomp_t p1sigPointpoint_extproj_precomp_t;
        point_setup(ProductResult, p1sigPoint);
        R1_to_R2(p1sigPoint, p1sigPointpoint_extproj_precomp_t);   
        x1[0] = MVector[gg] + PVector[gg];
        blake2s(hashConcat, x1, NULL, 32, 100, 0);

        point_t tempPT;
        memcpy(tempPT,h_iterationArr[gg],sizeof(tempPT));
        ecc_mul(tempPT,(digit_t*)hashConcat,tempPT,false);

        point_setup(tempPT, p2Extproj);
        eccadd(p1sigPointpoint_extproj_precomp_t, p2Extproj);
        eccnorm(p2Extproj, ProductResult);
      }


      point_t PKVectorSum;
      memcpy(PKVectorSum,h_iterationArr[0],sizeof(h_iterationArr[0]));
      for(unsigned int gg = 1; gg < i; gg++) {
        point_extproj_t p1sigPoint,p2Extproj;
        point_extproj_precomp_t p1sigPointpoint_extproj_precomp_t;
        point_setup(PKVectorSum, p1sigPoint);
        R1_to_R2(p1sigPoint, p1sigPointpoint_extproj_precomp_t);   

        point_setup(h_iterationArr[gg], p2Extproj);
        eccadd(p1sigPointpoint_extproj_precomp_t, p2Extproj);
        eccnorm(p2Extproj, PKVectorSum);
      }

      if(memcmp(h_1t,PKVectorSum,sizeof(h_1t)) == 0) {
     	//printf("Passed!\n");
      }
      else {
    	//printf("Second Check Failed!\n");
          return 0;
      }
      
      unsigned int sigSum[32];
      //Preping alpha^s1,t
      point_t sigPoint;
      ecc_mul_fixed((digit_t*)s_jTotal,sigPoint);


      point_t check3;
      point_extproj_t p1sigPoint,p2Extproj;
      point_extproj_precomp_t p1sigPointpoint_extproj_precomp_t;
      point_setup(sigPoint, p1sigPoint);
      R1_to_R2(p1sigPoint, p1sigPointpoint_extproj_precomp_t);   

      point_setup(ProductResult, p2Extproj);
      eccadd(p1sigPointpoint_extproj_precomp_t, p2Extproj);
      eccnorm(p2Extproj, check3);

      if(memcmp(check3,R_1t,sizeof(point_t)) == 0) {
   	//printf("Passed!\n");
      }
      else {
   	//printf("Failed!\n");
        return 0;
      }


  }

   }

  clock_t endT = clock();
  float secondsT = ((float)(endT - startT) / CLOCKS_PER_SEC)/BENCH_LOOPS;
  
  printf("Verification runs in %f miliseconds\n", (secondsT*1000));
   
  return 0;
}

void MVectorFill(unsigned char MV[],unsigned char arr[], int n) 
{ 
    unsigned int total = 1; 
    unsigned char curElement = arr[0];
    MV[0] = curElement;
  
    // Pick all elements one by one 
    for (int i = 1; i < n; i++) { 
        if(curElement != arr[i]) {
              MV[total] = arr[i];
              total++; 
              curElement = arr[i];
        }
    } 
} 

unsigned int countDistinct(unsigned char arr[], int n) 
{ 
    unsigned int total = 1; 
    unsigned char curElement = arr[0];
  
    // Pick all elements one by one 
    for (int i = 1; i < n; i++) { 
        if(curElement != arr[i]) {
              total++; 
              curElement = arr[i];
        }
    } 
    return total; 
} 
 
char * toArray(int number)
{
        int n = log10(number) + 1;
        int i;
      char *numberArray = calloc(n, sizeof(char));
        for ( i = 0; i < n; ++i, number /= 10 )
        {
            numberArray[i] = number % 10;
        }
        return numberArray;
} //Reference: https://stackoverflow.com/questions/14564813/how-to-convert-integer-to-character-array-using-c




