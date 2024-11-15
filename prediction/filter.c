/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: filter.c
 *
 * MATLAB Coder version            : 24.2
 * C/C++ source code generated on  : 15-Nov-2024 02:25:16
 */

/* Include Files */
#include "filter.h"
#include "../fft/rt_nonfinite.h"
#include <string.h>

/* Function Definitions */
/*
 * Arguments    : const double x[43200]
 *                double y[43200]
 * Return Type  : void
 */
void filter(const double x[43200], double y[43200])
{
  static const double b_dv[175] = {
      5.2747645709160296E-5,   -4.4943519512620165E-5,  -0.00010306214662330459,
      9.807789526674575E-19,   0.00014028169888312338,  8.3566945155661478E-5,
      -0.00013500176488278815, -0.00018765921723615649, 6.3291827730905438E-5,
      0.00027457639968037236,  7.9432307142823728E-5,   -0.00029585955110438726,
      -0.00026789963382540345, 0.00020936613167617103,  0.0004456224996533351,
      -5.4631904698794113E-18, -0.0005363668764988128,  -0.00030343852688047908,
      0.00046790623012550668,  0.00062349607579904453,  -0.00020232237078375998,
      -0.00084715172917950452, -0.00023718584397729148, 0.00085707146289649234,
      0.00075451685068958518,  -0.00057437185139346035, -0.0011928423150722603,
      -6.5107665490306077E-19, 0.0013730148643804146,   0.00076109195287697013,
      -0.0011512889643056105,  -0.0015065533502414228,  0.00048056430498046927,
      0.0019798256327660385,   0.0005458679129957838,   -0.0019440199823992149,
      -0.0016879871889020344,  0.001268305373068339,    0.0026016134972123273,
      -6.9997088306550092E-18, -0.0029270974154462994,  -0.0016056497223868075,
      0.0024049455271416883,   0.0031178931993082067,   -0.00098588691441274783,
      -0.0040284708003459086,  -0.0011022353640586548,  0.0038975799478433238,
      0.0033620522713603451,   -0.0025109281740185157,  -0.0051222930456524237,
      3.0176891177665871E-17,  0.0057095711500602317,   0.0031200471932407142,
      -0.0046581878111145829,  -0.0060233898502161064,  0.0019008704062672081,
      0.0077571488941875133,   0.0021211873952170042,   -0.0075018240880467138,
      -0.0064772230869624839,  0.0048461974705619722,   0.0099131567131333382,
      -1.3319309973420422E-17, -0.011144105082768115,   -0.0061263223110741239,
      0.0092134087592087681,   0.01201818283872062,     -0.0038322001659359217,
      -0.015830205595835325,   -0.004390845194717107,   0.015788594355034737,
      0.013897947521642424,    -0.010634494104032858,   -0.022329997401020148,
      1.5466717606261648E-17,  0.02682614186846612,     0.01538456518796036,
      -0.024331364167777498,   -0.03371902542545184,    0.011575653612928644,
      0.052409787135925673,    0.01633842619706798,     -0.068545601677178,
      -0.07480019629169872,    0.07947206176204262,     0.30736855444839523,
      0.41667873010766449,     0.30736855444839523,     0.07947206176204262,
      -0.07480019629169872,    -0.068545601677178,      0.01633842619706798,
      0.052409787135925673,    0.011575653612928644,    -0.03371902542545184,
      -0.024331364167777498,   0.01538456518796036,     0.02682614186846612,
      1.5466717606261648E-17,  -0.022329997401020148,   -0.010634494104032858,
      0.013897947521642424,    0.015788594355034737,    -0.004390845194717107,
      -0.015830205595835325,   -0.0038322001659359217,  0.01201818283872062,
      0.0092134087592087681,   -0.0061263223110741239,  -0.011144105082768115,
      -1.3319309973420422E-17, 0.0099131567131333382,   0.0048461974705619722,
      -0.0064772230869624839,  -0.0075018240880467138,  0.0021211873952170042,
      0.0077571488941875133,   0.0019008704062672081,   -0.0060233898502161064,
      -0.0046581878111145829,  0.0031200471932407142,   0.0057095711500602317,
      3.0176891177665871E-17,  -0.0051222930456524237,  -0.0025109281740185157,
      0.0033620522713603451,   0.0038975799478433238,   -0.0011022353640586548,
      -0.0040284708003459086,  -0.00098588691441274783, 0.0031178931993082067,
      0.0024049455271416883,   -0.0016056497223868075,  -0.0029270974154462994,
      -6.9997088306550092E-18, 0.0026016134972123273,   0.001268305373068339,
      -0.0016879871889020344,  -0.0019440199823992149,  0.0005458679129957838,
      0.0019798256327660385,   0.00048056430498046927,  -0.0015065533502414228,
      -0.0011512889643056105,  0.00076109195287697013,  0.0013730148643804146,
      -6.5107665490306077E-19, -0.0011928423150722603,  -0.00057437185139346035,
      0.00075451685068958518,  0.00085707146289649234,  -0.00023718584397729148,
      -0.00084715172917950452, -0.00020232237078375998, 0.00062349607579904453,
      0.00046790623012550668,  -0.00030343852688047908, -0.0005363668764988128,
      -5.4631904698794113E-18, 0.0004456224996533351,   0.00020936613167617103,
      -0.00026789963382540345, -0.00029585955110438726, 7.9432307142823728E-5,
      0.00027457639968037236,  6.3291827730905438E-5,   -0.00018765921723615649,
      -0.00013500176488278815, 8.3566945155661478E-5,   0.00014028169888312338,
      9.807789526674575E-19,   -0.00010306214662330459, -4.4943519512620165E-5,
      5.2747645709160296E-5};
  int j;
  int k;
  memset(&y[0], 0, 43200U * sizeof(double));
  for (k = 0; k < 175; k++) {
    int b_k;
    b_k = k + 1;
    for (j = b_k; j < 43201; j++) {
      y[j - 1] += b_dv[k] * x[(j - k) - 1];
    }
  }
}

/*
 * File trailer for filter.c
 *
 * [EOF]
 */
