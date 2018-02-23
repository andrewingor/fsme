//FSME Example & Test
//(c)2018 by <a href="https://goo.gl/FFF1Up">Max Lancer</a>
//License: Devil's Contract

#include <iostream>
#include <fsme>

using namespace std;
using namespace fsm;

int main (int argc, char** argv) {
try {
	fsm::simple Bot;

	Bot.go(1); cout << "	State=" << Bot.stat() << endl;
	Bot.go(0); cout << "	State=" << Bot.stat() << endl;
	Bot.go(0); cout << "	State=" << Bot.stat() << endl;
	Bot.go(66);

return 0;
} catch (const exception& e) {
return 1;
} catch (...) {
	//logs << debug <<  e.what() << endl;
return 2;
}}
//EOF
