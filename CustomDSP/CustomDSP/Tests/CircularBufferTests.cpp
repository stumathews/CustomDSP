#include "pch.h"
#include "../OpenGLTemplate/CircularBuffer.h"
#include "../OpenGLTemplate/ConvolutionHelper.h"

using namespace std;


template<typename T>
void printArray(T arr[], int size)
{
	cout << "[";
	for (int i = 0; i < size ; i++)
	{
		cout << arr[i];
		if (i + 1 < size)
			cout << ",";

	}
	cout << "]" << endl;
}

class CircularBufferTests : public ::testing::Test
{
private:
	
protected:
    void SetUp() override
    {
    }

    void TearDown() override {}
    
};

TEST_F(CircularBufferTests, GetWriteIndex)
{
	int writeIndex = 0;
	cbuf<float> buf(4);

	writeIndex = buf.GetWriteIndex();
	EXPECT_EQ(writeIndex, 0);
	buf.Put(1.0);
	
	writeIndex = buf.GetWriteIndex();
	EXPECT_EQ(writeIndex, 1);
	buf.Put(1.0);
	
	writeIndex = buf.GetWriteIndex();
	EXPECT_EQ(writeIndex, 2);
	
	buf.Put(1.0);
	writeIndex = buf.GetWriteIndex();
	EXPECT_EQ(writeIndex, 3);
	
	buf.Put(1.0);
	writeIndex = buf.GetWriteIndex();
	EXPECT_EQ(writeIndex, 0);
	
	buf.Put(1.0);
	writeIndex = buf.GetWriteIndex();
	EXPECT_EQ(writeIndex, 1);

	buf.Put(1.0);
	writeIndex = buf.GetWriteIndex();
	EXPECT_EQ(writeIndex, 2);
	
}

TEST_F(CircularBufferTests, InitialSizeTest) 
{
	cbuf<float> buf(4);
	buf.Put(1.0);
	buf.Put(2.0);
	buf.Put(2.0);
	buf.Put(4.0);
	ASSERT_EQ(buf.GetSize(), 4) << "Buffer expected to be 4 items long but was " << buf.GetSize() << " in length";
}

TEST_F(CircularBufferTests, SmallArrayTest)
{
	const int cols = 4;
	const int rows = 4;
	float x[rows][cols] = 
	{ 
		{1, 2, 3, 4}, 
		{5, 6, 7, 8},
		{9, 10, 11, 12},
		{13, 14, 15, 16},
	};

	float y[rows][cols];

	cbuf<float> cbuf(4);

	for (int r = 0; r < rows; r++)
	{	
		// Simulates an new invocation with access 		
		for (int c = 0; c < cols; c++)
		{
			// Access new samples
			auto val = x[r][c];
			cbuf.Put(val);
			auto headval = cbuf.ReadNewestHead();
			ASSERT_EQ(headval, val);
		}		
	}

	EXPECT_EQ(cbuf.ReadAtIndex(0), 13);
	EXPECT_EQ(cbuf.ReadAtIndex(1), 14);
	EXPECT_EQ(cbuf.ReadAtIndex(2), 15);
	EXPECT_EQ(cbuf.ReadAtIndex(3), 16);
}

TEST_F(CircularBufferTests, ReadFromBack)
{
	cbuf<float> buf(4);
	buf.Put(1.0);
	buf.Put(2.0);
	buf.Put(3.0);
	buf.Put(4.0);

	ASSERT_EQ(buf.ReadFromBack(0), buf.ReadNewestHead()); // n
	ASSERT_EQ(buf.ReadFromBack(), buf.ReadNewestHead()); // n
	ASSERT_EQ(buf.ReadFromBack(1), 3.0); // n-1
	ASSERT_EQ(buf.ReadFromBack(2), 2.0); // n-2
	ASSERT_EQ(buf.ReadFromBack(3), 1.0); // n-3

	buf.Put(5.0); //everwrite oldest entry
	ASSERT_EQ(buf.ReadFromBack(3), 5.0); // n-3

}

TEST_F(CircularBufferTests, SimulateStream)
{
	const int nSamples = 4;
	const int numInvocations = 4;

	float bCoefficients[numInvocations] = { 10 /*n*/, 20 /*n-1*/, 30/*n-2*/ , 40/*n-3*/ }; // latest to earliest
	
	float y[numInvocations * nSamples] = {0};
	float x[numInvocations][nSamples] = // arranged earliest to latest ie [0] = n-3 (where n = nSamples)
	{
		{1, 2, 3, 4}, /* as in: n-3, n-2, n-1, n */
		{5, 6, 7, 8},
		{9, 10, 11, 12},	
		{13, 14, 15, 16},
	};

	cbuf<float> prevBuff(4);
	
	for (int call = 0; call < numInvocations; call++)
	{		
		float* chunk = &x[call][0];

		for (int n = 0; n < nSamples; n++)
		{
			float* xn = &x[call][n]; 
			float* yn = &y[(call * nSamples) + n];
			
			ConvolutionHelper::ConvolveXn(chunk, nSamples, n, yn, xn, bCoefficients, numInvocations, &prevBuff);
		}
	}
	
	EXPECT_EQ(y[0], 40);
	EXPECT_EQ(y[1], 30);
	EXPECT_EQ(y[2], 40);
	EXPECT_EQ(y[3], 80);
}

TEST_F(CircularBufferTests, ReadOldestEntry)
{
	cbuf<float> buf(4); //[0,0,0,0]
	EXPECT_EQ(buf.ReadOldest(), 0);
	buf.Put(1.0); //[1,0,0,0]
	EXPECT_EQ(buf.ReadOldest(), 0);
	buf.Put(2.0); //[1,2,0,0]
	EXPECT_EQ(buf.ReadOldest(), 0);
	buf.Put(3.0); //[1,2,3,0]
	EXPECT_EQ(buf.ReadOldest(), 0);
	buf.Put(4.0); //[1,2,3,4]
	EXPECT_EQ(buf.ReadOldest(), 1);
	buf.Put(5.0); //[1,2,3,4]
	EXPECT_EQ(buf.ReadOldest(), 2);
}


TEST_F(CircularBufferTests, ReadOldest)
{
	cbuf<float> buf(4); //[0,0,0,0]
	EXPECT_EQ(buf.ReadOldest(), 0);

	buf.Put(1.0); //[1,0,0,0]
	EXPECT_EQ(buf.ReadOldest(), 0);
	buf.Put(2.0); //[1,2,0,0]
	EXPECT_EQ(buf.ReadOldest(), 0);
	buf.Put(3.0); //[1,2,3,0]
	EXPECT_EQ(buf.ReadOldest(), 0);
	buf.Put(4.0); //[1,2,3,4]
	EXPECT_EQ(buf.ReadOldest(), 1);
	buf.Put(5.0); //[1,2,3,4]
	EXPECT_EQ(buf.ReadOldest(), 2);
}

TEST_F(CircularBufferTests, ReadNewestHead)
{
	cbuf<float> buf(4); //[0,0,0,0]
	EXPECT_EQ(buf.ReadNewestHead(), 0);

	buf.Put(1.0); //[1,0,0,0]
	EXPECT_EQ(buf.ReadNewestHead(), 1);
	buf.Put(2.0); //[1,2,0,0]
	EXPECT_EQ(buf.ReadNewestHead(), 2);
	buf.Put(3.0); //[1,2,3,0]
	EXPECT_EQ(buf.ReadNewestHead(), 3);
	buf.Put(4.0); //[1,2,3,4]
	EXPECT_EQ(buf.ReadNewestHead(), 4);
	buf.Put(5.0); //[1,2,3,4]
	EXPECT_EQ(buf.ReadNewestHead(), 5);
}

TEST_F(CircularBufferTests, GetNewestIndex)
{
	cbuf<float> buf(4); //[0,0,0,0]
	EXPECT_EQ(buf.GetNewestIndex(), 0);
	
	buf.Put(1.0); //[1,0,0,0]
	EXPECT_EQ(buf.GetNewestIndex(), 0);
	buf.Put(2.0); //[1,2,0,0]
	EXPECT_EQ(buf.GetNewestIndex(), 1);
	buf.Put(3.0); //[1,2,3,0]
	EXPECT_EQ(buf.GetNewestIndex(), 2);
	buf.Put(4.0); //[1,2,3,4]
	EXPECT_EQ(buf.GetNewestIndex(), 3);
	buf.Put(5.0); //[5,2,3,4]
	EXPECT_EQ(buf.GetNewestIndex(), 0);
	buf.Put(6.0); //[5,6,3,4]
	EXPECT_EQ(buf.GetNewestIndex(), 1);

	EXPECT_EQ(buf.ReadAtIndex(0), 5);
	EXPECT_EQ(buf.ReadAtIndex(1), 6);
	EXPECT_EQ(buf.ReadAtIndex(2), 3);
	EXPECT_EQ(buf.ReadAtIndex(3), 4);
	EXPECT_EQ(buf.ReadFromBack(0), 4);

}

TEST_F(CircularBufferTests, PutReturnedIndex)
{
	cbuf<float> buf(4); //[0,0,0,0]
	int i1 = buf.Put(1.0); //[1,0,0,0]
	int i2 = buf.Put(2.0); //[1,2,0,0]
	int i3 = buf.Put(3.0); //[1,2,3,0]
	int i4 = buf.Put(4.0); //[1,2,3,4]
	int i5 = buf.Put(5.0); //[5,2,3,4]
	int i6 = buf.Put(6.0); //[5,6,3,4]

	EXPECT_EQ(i1, 0);
	EXPECT_EQ(i2, 1);
	EXPECT_EQ(i3, 2);
	EXPECT_EQ(i4, 3);
	EXPECT_EQ(i5, 0);
	EXPECT_EQ(i6, 1);
}

TEST_F(CircularBufferTests, PreviousN)
{
	cbuf<float> buf(4); //[0,0,0,0]
	int i1 = buf.Put(1.0); //[1,0,0,0]
	int i2 = buf.Put(2.0); //[1,2,0,0]
	int i3 = buf.Put(3.0); //[1,2,3,0]
	int i4 = buf.Put(4.0); //[1,2,3,4]
	int i5 = buf.Put(5.0); //[5,2,3,4]
	int i6 = buf.Put(6.0); //[5,6,3,4]

	/* Internal function not accessible to the public */
	ASSERT_EQ(buf.Wrap(0, 4), 0);
	ASSERT_EQ(buf.Wrap(1, 4), 1);
	ASSERT_EQ(buf.Wrap(2, 4), 2);
	ASSERT_EQ(buf.Wrap(3, 4), 3);
	ASSERT_EQ(buf.Wrap(4, 4), 0);
	ASSERT_EQ(buf.Wrap(5, 4), 1);
	ASSERT_EQ(buf.Wrap(6, 4), 2);

	ASSERT_EQ(buf.GetNewestIndex(), buf.Wrap(buf.GetNewestIndex(), 4));

	/* GetPrevNIndex is not accessible to the public, but ReadN(+/-x) is */
	ASSERT_EQ(buf.GetPrevNIndex(), 1);
	ASSERT_EQ(buf.GetPrevNIndex(1), 0);
	ASSERT_EQ(buf.GetPrevNIndex(2), 3);
	ASSERT_EQ(buf.GetPrevNIndex(3), 2);
	ASSERT_EQ(buf.GetPrevNIndex(4), 1);	

	ASSERT_EQ(buf.ReadN(0), 6);
	ASSERT_EQ(buf.ReadN(), 6);
	ASSERT_EQ(buf.ReadN(-1), 5);
	ASSERT_EQ(buf.ReadN(-2), 4);
	ASSERT_EQ(buf.ReadN(-3), 3);
	ASSERT_EQ(buf.ReadN(-4), 6);
	ASSERT_EQ(buf.ReadN(-5), 5);
	ASSERT_EQ(buf.ReadN(1), 3);
}