#include "provided.h"
#include <string>
#include <vector>
using namespace std;

const size_t BITS_PER_UNSIGNED_SHORT = 16;

string convertNumberToBitString(unsigned short number);
bool convertBitStringToNumber(string bitString, unsigned short& number);

string BinaryConverter::encode(const vector<unsigned short>& numbers)
{
	if (numbers.empty())
		return "";

	string code;
	int vector_size = numbers.size();
	for (int i = 0; i < vector_size; i++)
		code += convertNumberToBitString(numbers[i]);

	int string_size = code.size();
	for (int i = 0; i < string_size; i++)
		code[i] = code[i] == '0' ? ' ' : '\t';
	return code;
}

bool BinaryConverter::decode(const string& bitString,
	vector<unsigned short>& numbers)
{
	int size = bitString.size();
	if (size % 16 != 0)
		return false;
	
	vector<unsigned short> v;
	for (int i = 0; i < size;)
	{
		string temp_string;
		unsigned short number;
		for (int k = 0; k < 16; k++, i++)
		{
			if (bitString[i] == '\t')
				temp_string += '1';
			else if (bitString[i] == ' ')
				temp_string += '0';
			else
				return false;
		}
		if (!convertBitStringToNumber(temp_string, number))
			return false;
		v.push_back(number);
	}
	numbers = v;
	return true;
}

string convertNumberToBitString(unsigned short number)
{
	string result(BITS_PER_UNSIGNED_SHORT, '0');
	for (size_t k = BITS_PER_UNSIGNED_SHORT; number != 0; number /= 2)
	{
		k--;
		if (number % 2 == 1)
			result[k] = '1';
	}
	return result;
}

bool convertBitStringToNumber(string bitString, unsigned short& number)
{
	if (bitString.size() != BITS_PER_UNSIGNED_SHORT)
		return false;
	number = 0;
	for (size_t k = 0; k < bitString.size(); k++)
	{
		number *= 2;
		if (bitString[k] == '1')
			number++;
		else if (bitString[k] != '0')
			return false;
	}
	return true;
}

/////////////////////////////////////
//	computeHash Functions
/////////////////////////////////////

unsigned int computeHash(string key)	// why not const string& ?
{
	unsigned int total = 0;
	int size = key.length();
	for (int i = 0; i < size; i++)
		total += (i + 1) * key[i];
	return total;
}

unsigned int computeHash(unsigned short key)
{
	return key;
}