//
//

#include <iostream>
#include "fsme"

using namespace std;
using namespace fsm;

// -------- H A N D L E S
void	zero(uint8_t* State) { clog << "0: transit to stateOn" << endl; *State = 1; }
void	one(uint8_t* State) { clog << "1: transit to stateOff" << endl; *State = 0; }
void	two(uint8_t*) { clog << "Two" << endl; }
void	thr(uint8_t*) { clog << "Three" << endl; }
// -------- H A N D L E S

class robot: public bot<>{
public:
	robot () {
		bot::Current = 0;
		static const ___Handles stateOff = {
		  { 0, zero }
		, { 1, two }
		};
		static const ___Handles stateOn = {
		  { 0, one }
		, { 1, thr }
		};
		bot::States =  {  stateOff, stateOn };
	}
virtual	~robot() { }
};

int main (int argc, char** argv) {
try {
	robot Bender; //stateOff
	Bender.go(1);
	Bender.go(0); //Transit to stateOn
	Bender.go(1);
	Bender.go(0); //Transit to stateOff
	Bender.go(1);

	Bender.States.push_back( { {0, two}, {1, thr} } );
	Bender.transit(2); 
	Bender.go(0); 
	Bender.go(1);

	cout << Bender.rev() << endl;

return 0;
} catch (const exception& e) {
	//logs << debug <<  e.what() << endl;
	clog << e.what() << endl;
return 1;
}}
//EOF
