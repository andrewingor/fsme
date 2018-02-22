//FSME Example & Test
//

#include <iostream>
#include "fsme"

using namespace std;
using namespace fsm;

int main (int argc, char** argv) {
try {
	fsm::init First;

	First << 0 << flush;
	cout << "Data=" << First.data() << endl;
	First.go(1);
	cout << "Data=" << First.data() << endl;

return 0;
} catch (const exception& e) {
	//logs << debug <<  e.what() << endl;
	clog << e.what() << endl;
return 1;
}}
//EOF
