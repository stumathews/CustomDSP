#pragma once
#include <vector>
using namespace std;

class CircularBufferTests_PreviousN_Test;

template <typename T>
class cbuf
{
	friend CircularBufferTests_PreviousN_Test;

private:
	int m_size;
	vector<T> buf;
	int writeIndex;
	int readIndex;

	int GetPrevNIndex(int n = 0) { return Wrap(GetNewestIndex() - n, m_size); }
	int Wrap(int n, int arrayLength) { return ((n % arrayLength) + arrayLength) % arrayLength; }

public:
	cbuf(int size) :m_size(size),
		writeIndex(0), readIndex(0) // start writing from index 0 ie the 'front'
	{
		buf = vector<T>(size);
	}
	~cbuf() {}

	T* ToArray() { return  &buf[0];	}

	// Overwrite oldest entry, returns the index of the item saved
	int Put(T item)
	{
		buf[writeIndex] = item;
		readIndex = writeIndex;
		writeIndex = (writeIndex + 1) % m_size;
		return readIndex;
	}

	T ReadNewestHead() { return buf[readIndex]; }
	T ReadAtIndex(int i) { return buf[i]; }
	T ReadFromBack(int n = 0) { return buf[(m_size - 1) - n]; }
	T ReadN(int n = 0) { return buf[GetPrevNIndex(-n)]; }
	T ReadOldest() { return buf[writeIndex]; }
	int GetOldIndex() { return writeIndex; }
	int GetNewestIndex() { return readIndex; }
	int GetSize() { return buf.size(); }

	void PrintContents()
	{
		cout << "[";
		for (int i = 0; i < buf.size(); i++)
		{
			std::cout << buf[i];
			if (i == buf.size())
				cout << ",";
		}
		cout << "]" << endl;;
	}



	int GetWriteIndex() { return writeIndex; }

};
