// The Compressor class (defined in provided.h) uses a Lempel-Ziv-variant compression 
// algorithm to compress encoded messages into a series of numbers. Conversely it can
// take a compressed series of numbers and decompress it back into the original secret
// message.

#include "provided.h"
#include "HashTable.h"
#include <string>
#include <vector>
using namespace std;

void Compressor::compress(const string& s, vector<unsigned short>& numbers)
{
	int size = s.size();
	unsigned short capacity = (size/2 + 512) < 16384 ? (size/2 + 512) : 16384;
	HashTable<string, unsigned short> h(capacity * 2, capacity);
	for (int i = 0; i < 256; i++)
		h.set(string(1, static_cast<char>(i)), i, true);

	unsigned short nextFreeID = 256, x;
	string runSoFar;
	vector<unsigned short> v;
	for (int i = 0; i < size; i++)
	{
		string expandedRun = runSoFar + s[i];
		if (h.get(expandedRun, x))
		{
			runSoFar = expandedRun;
			continue;
		}
		h.get(runSoFar, x);
		v.push_back(x);
		h.touch(runSoFar);
		runSoFar.clear();
		unsigned short cv;
		h.get(string(1, s[i]), cv);
		v.push_back(cv);
		if (!h.isFull())
			h.set(expandedRun, nextFreeID++);
		else
		{
			string key;
			unsigned short value;
			h.discard(key, value);
			h.set(expandedRun, value);
		}
	}
	if (!runSoFar.empty())
	{
		h.get(runSoFar, x);
		v.push_back(x);
	}
	v.push_back(capacity);
	numbers = v;
}

bool Compressor::decompress(const vector<unsigned short>& numbers, string& s)
{
	if (numbers.empty())
		return false;
	
	int size = numbers.size();
	unsigned short capacity = numbers.back();
	HashTable<unsigned short, string> h(capacity * 2, capacity);
	for (int i = 0; i < 256; i++)
		h.set(i, string(1, static_cast<char>(i)), true);

	unsigned short nextFreeID = 256;
	string runSoFar, output;
	for (int i = 0; i < size - 1; i++)
	{
		if (numbers[i] < 256)
		{
			string temp, expandedRun;
			h.get(numbers[i], temp);
			output += temp;
			if (runSoFar.empty())
			{
				runSoFar = temp;
				continue;
			}
			expandedRun = runSoFar + temp;
			if (!h.isFull())
				h.set(nextFreeID++, expandedRun);
			else
			{
				unsigned short key;
				string value;
				h.discard(key, value);
				h.set(key, expandedRun);
			}
			runSoFar.clear();
		}
		else  // >= 256, map represents a multi-char string
		{
			string value;
			if (!h.get(numbers[i], value))
				return false;
			h.touch(numbers[i]);
			output += value;
			runSoFar = value;
		}
	}
	s = output;
	return true;
}
