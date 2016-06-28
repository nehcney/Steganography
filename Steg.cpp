#include "provided.h"
#include <string>
using namespace std;

/////////////////////////////////////
//	Helper Function
/////////////////////////////////////

int Steg::divideString(const string& src, vector<string>& strings)
{
	strings.clear();	// pre-emptively clear the vector
	int carriage = 0;
	int size = src.size();
	int k = 0, pos = 0;
	for (; k < size; k++)
	{
		if (src[k] == '\r')
		{
			carriage = 1;
			continue;
		}
		if (src[k] == '\n')
		{
			strings.push_back(string(src, pos, k - pos - carriage));
			pos = k + 1;
		}
		carriage = 0;
	}
	if (k > pos)
		strings.push_back(string(src, pos, k - pos - carriage));
	return strings.size();
}

/////////////////////////////////////
//	Public Member Functions
/////////////////////////////////////

bool Steg::hide(const string& hostIn, const string& msg, string& hostOut)
{
	if (hostIn.empty())
		return false;

	  // Divide hostIn into N lines separated by newline characters
	vector<string> sv;
	int N = divideString(hostIn, sv);
	
	  // Strip any trailing tabs/spaces
	for (int i = 0; i < N; i++)
		while (!sv[i].empty() && (sv[i].back() == ' ' || sv[i].back() == '\t'))
			sv[i].pop_back();
	
	  // Compress and encode msg into tabs/spaces
	vector<unsigned short> v;
	Compressor::compress(msg, v);
	string msgEncoded = BinaryConverter::encode(v);

	  // Append the tabs/spaces to the substrings of hostIn
	int L = msgEncoded.size();
	for (int i = 0, pos = 0; i < N; i++) // each substring of hostIn
	{
		if (i < L % N)
		{
			sv[i].append(msgEncoded, pos, L / N + 1);
			pos += L / N + 1;
		}
		else
		{
			sv[i].append(msgEncoded, pos, L / N);
			pos += L / N;
		}
	}

	  // Concatenate the substrings to produce hostOut
	hostOut.clear();
	for (int i = 0; i < N; i++) // each substring of hostIn
		hostOut += sv[i] + '\n';
	return true;
}

bool Steg::reveal(const string& hostIn, string& msg)
{
	  // Divide hostIn into N lines separated by newline characters
	vector<string> sv;
	int N = divideString(hostIn, sv);

	  // Gather the trailing tabs/spaces into one string
	  // !!! start from the end instead!
	string trailingTabsSpaces;
	for (int i = 0; i < N; i++)
	{
		int substring_size = sv[i].size(), pos = 0;
		bool newSeries = true;
		for (int k = 0; k < substring_size; k++)
		{
			if (sv[i][k] == ' ' || sv[i][k] == '\t')
			{
				if (newSeries)
				{
					pos = k;
					newSeries = false;
				}
			}
			else
				newSeries = true;
		}
		trailingTabsSpaces += string(sv[i], pos);
	}

	  // Decode and decompress the tabs/spaces into final msg
	vector<unsigned short> v;
	string result;
	if (!BinaryConverter::decode(trailingTabsSpaces, v)
		|| !Compressor::decompress(v, result))
		return false;
	msg = result;
	return true;
}
