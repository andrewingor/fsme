//xUTF8enc 6-bytes bit sequence encoding with 32 bits for code point
//It use 2nd bit of 1st byte FSS-UTF(1992) for 32th bit
//For big binary data transmission (like media stream)
//(c)2018 by <a href="https://goo.gl/FFF1Up">Max Lancer</a>
//License: Devil's Contract

#include <iostream>
#include <fsme>

using namespace std;
using namespace fsm;

//----------
/*TODO xrune --------------------------
	xUTF8 symbol
-----------------------------------*/
class xrune {
};

//----------
/*TODO xUTF8 --------------------------
	Input byte by byte and output std::string<rune>
-----------------------------------*/
class xUTF8 {
};

int main (int argc, char** argv) {
try {

return 0;
} catch (const exception& e) {
return 1;
} catch (...) {
	//logs << debug <<  e.what() << endl;
return 2;
}}
//EOF

