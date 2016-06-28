#include "provided.h"
#include "http.h"	// user added
#include <string>
using namespace std;

bool WebSteg::hideMessageInPage(const string& url, const string& msg, string& host)
{
	string page;
	return HTTP().get(url, page) && Steg::hide(page, msg, host);	
}

bool WebSteg::revealMessageInPage(const string& url, string& msg)
{
	string page;
	return HTTP().get(url, page) && Steg::reveal(page, msg);
}