#include "HashTable.h"
#include "provided.h"
#include "http.h"
#include <cassert>
using namespace std;

/////////////////////////////////////
//	Test Functions
/////////////////////////////////////

void generalTesting()
{
	string s1 = "segiou"; // "This is a string.";
	string s2 = "nwuen"; // "This a string is.";
	string s3 = "d at sh"; // "This string is a.";
	string s4 = "thw2  3 "; // "This a is string.";
	string s5 = "random!"; // "a string is This.";
	HashTable<string, int> a(5, 10);
	assert(a.set(s1, 1));
	a.set(s2, 2);
	assert(a.set(s3, 3, true));
	a.set(s4, 4, true);
	a.set(s5, 5);

	int value;
	assert(a.get(s1, value) && value == 1);
	assert(!a.get("", value) && value == 1);
	assert(!a.isFull());
	assert(a.touch(s1));
	string key;
	assert(a.discard(key, value) && key == s2 && value == 2);
	assert(a.discard(key, value) && key == s5 && value == 5);
	assert(a.discard(key, value) && key == s1 && value == 1);
	assert(!a.discard(key, value) && key == s1 && value == 1);
}

void testEncode()
{
	vector<unsigned short> v;
	v.push_back(1);
	v.push_back(5);
	string hiddenMessage = BinaryConverter::encode(v);
	assert(hiddenMessage == "               \t             \t \t");
}

void testDecode()
{
	vector<unsigned short> v;
	string msg = "               \t             \t \t";
	assert(BinaryConverter::decode(msg, v) && v.size() == 2);
}

void testCompressDecompress()
{
	vector<unsigned short> v;
	v.push_back(65);
	v.push_back(65);
	v.push_back(256);
	v.push_back(65);
	v.push_back(65);
	v.push_back(65);
	v.push_back(256);
	v.push_back(66);
	v.push_back(257);
	string msg;
	assert(Compressor::decompress(v, msg) && msg == "AAAAAAAAAB");
	vector<unsigned short> v2;
	Compressor::compress(msg, v2);
	assert(v2.size() == 8 && v2.front() == 65 && v2.back() == 517);
	assert(Compressor::decompress(v2, msg) && msg == "AAAAAAAAAB");
	vector<unsigned short> v3;
	assert(!Compressor::decompress(v3, msg));
	v3.push_back(257);
	assert(Compressor::decompress(v3, msg) && msg == "");
	Compressor::compress(msg, v3);
	assert(v3.size() == 1 && v3.front() == 512 && v3.back() == 512);
}

void testSteg()
{
	/*
	vector<unsigned short> v;
	string in = "<html>   \r\nQ \tQQ \t \nBBB\t\t\t   \n\nGG \nBBB \n\t\nDDD\nEEE </html>   "
		, out;
	string msg = "    \t \t\t\t \t      \t\t \t \t  \t\t\t \t\t ";
	BinaryConverter::decode(msg, v);
	Compressor::decompress(v, msg);
	Compressor::compress(msg, v);
	string msgEncoded = BinaryConverter::encode(v);
	assert(Steg::hide(in, msg, out)
		&& out == "<html>    \nQ \tQQ\t \t\t\nBBB\t \t \n    \nGG \t\t \nBBB\t \t\n  \t\nDDD\t\t \nEEE </html>\t\t \n");
	*/
	vector<unsigned short> v;
	string in = "<html>   \r\nQ \tQQ \t \nBBB\t\t\t   \n\nGG \nBBB \n\t\nDDD\nEEE </html>   ",
		msg = "Hello.", out, result;
	assert(Steg::hide(in, msg, out));
	assert(Steg::reveal(out, result) && result == msg);
}

void testWebSteg()
{
	string pageText =
		"<html>\n"
		"<head>\n"
		" <title>My Pretend Web Page</title>\n"
		"</head>\n"
		"<body>\n"
		"<h2>My Pretend Web Page<h1>\n"
		"<p>\n"
		"I wish I were creative enough to have something interesting \n"
		"to say here.  I'm not, though, so this is a boring page.\n"
		"</p>\n"
		"<p>\n"
		"Oh, well.\n"
		"</p>\n"
		"</body>\n"
		"</html>\n"
		;
	HTTP().set("http://boring.com", pageText);
	string plan =
		"Lefty and Mugsy enter the bank by the back door.\n"
		"Shorty waits out front in the getaway car.\n"
		"Don't let the teller trip the silent alarm.\n"
		;
	assert(WebSteg::hideMessageInPage("http://boring.com", plan, pageText));
	HTTP().set("http://boring.com", pageText);
	string msg;
	assert(WebSteg::revealMessageInPage("http://boring.com", msg)
		&& msg == plan);
}

int main()
{
	generalTesting();
	testEncode();
	testDecode();
	testCompressDecompress();
	testSteg();	
	testWebSteg();
	cout << "Passed all tests!";	// !!! more comprehensive testing
}