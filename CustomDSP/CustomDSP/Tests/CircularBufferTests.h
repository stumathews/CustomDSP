#pragma once

void ConvolveXn(float* chunk, const int& nSamples, int n, float* yn, float  bCoefficients[4], cbuf<float>& prevBuff, float* xn);
