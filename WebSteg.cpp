// The WebSteg class (defined in provided.h) allows its client to download a webpage from
// the internet and, using the Steg class, compress and encode a secret message onto that
// page. Conversely it can also download a webpage from the internet that contains a 
// steganographically-encoded secret message and, using the Steg class, decode the secret
// message. 

#include "provided.h"
#include "http.h"
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
