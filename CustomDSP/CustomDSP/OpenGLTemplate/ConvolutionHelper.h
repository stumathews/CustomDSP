#pragma once
class ConvolutionHelper
{
public:
	// Swaps the signal and convolves the b coeficcients using previous values stored in circular buffer, updates the buffer
	
	static void ConvolveXn(float* chunk, const unsigned int numSamplesPerChunk, unsigned int n, float* yn, const float* xn,
	                       const float* b_coefficients, int numCoefficients, cbuf<float>* prevBuff, float coefficientScale = 1)
	{		
		auto swappedChunkSample = chunk[(numSamplesPerChunk - 1) - n];
		
		*yn = *xn;
		*yn = b_coefficients[0] * swappedChunkSample; // x[n] * b[0]

		for (int b = 0; b < numCoefficients - 1; b++) // x[n-1] * b[1] etc...
			*yn += (prevBuff->ReadN(-b) * (b_coefficients[b] * coefficientScale)) * swappedChunkSample ;

		prevBuff->Put(*xn);
	}

	/*
	 * Perform s1 * s2 ie a convolution of s1 and s2
	 * s1: input vector or signal or x[]
	 * N1: length of s1
	 * s2: filter b coefficients
	 * N2: length of s2
	 * Returns a pointer to cbuf<float>,which represents y[], which the caller is responsible for releasing after use
	 * Remarks: No need to flip any of the input vectors
	 */
	static void ConvolveTo(const float* s1, const unsigned int N1, const float* s2, const unsigned int N2, float* result)
	{
		for (auto i = 0; i < N1+N2-1; i++)
			result[i] = convolve_at_t1(i, s1, N1, s2, N2);
	}

	static cbuf<float>* ConvolveBuffers(cbuf<float>* s1, cbuf<float>* s2)
	{
		auto result = new cbuf<float>(s1->GetSize() + s2->GetSize() - 1);
		for (auto i = 0; i < result->GetSize(); i++)
			result->Put(convolve_at_t1(i, s1->ToArray(), s1->GetSize(), s2->ToArray(), s2->GetSize()));
		return result;
	}

private:

	// Performs convolution at position y[t1]	
	static float convolve_at_t1(const unsigned int t1, const float* s1, const unsigned int N1, const float* s2, const unsigned int N2)
	{
		float sum = 0;
		for (unsigned t = 0; t <= N2 - 1; t++)
			sum += t1 - t >= N1 ? 0 : s1[t1 - t] * s2[t];
		return sum;
	}

};

