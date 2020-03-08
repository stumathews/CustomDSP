#pragma once
class ConvolutionHelper
{
public:
	static void ConvolveXn(float* chunk, const int numSamplesPerChunk, int n, float* yn, float* xn, float* bCoefficients, int num_coeff, cbuf<float>* prevBuff, float coefficientScale = 1)
	{
		*yn = *xn;
		float swappedXn = chunk[(numSamplesPerChunk - 1) - n];
		*yn = bCoefficients[0] * swappedXn; // x[n] * b[0]

		for (int b = 1; b <= num_coeff - 1; b++) // x[n-1] * b[1] etc...
			*yn += prevBuff->ReadN(-b) * (bCoefficients[b] * coefficientScale);

		prevBuff->Put(*xn);
	}
};

