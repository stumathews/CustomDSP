#pragma once
class ConvolutionHelper
{
public:
	// Swaps the signal and convolves the b coeficcients using previous values stored in circular buffer, updates the buffer
	static void ConvolveXn(float* chunk, const int numSamplesPerChunk, int n, float* yn, float* xn, float* bCoefficients, int num_coeff, cbuf<float>* prevBuff, float coefficientScale = 1)
	{		
		auto swappedXn = chunk[(numSamplesPerChunk - 1) - n];
		*yn = *xn;
		*yn = bCoefficients[0] * swappedXn; // x[n] * b[0]

		for (int b = 1; b <= num_coeff - 1; b++) // x[n-1] * b[1] etc...
			*yn += prevBuff->ReadN(-b) * (bCoefficients[b] * coefficientScale);

		prevBuff->Put(*xn);
	}
};

