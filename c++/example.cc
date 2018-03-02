//
#include <iostream>
#include <fsme>

using namespace std;
using namespace fsm;

namespace fsm { static fsm::engine<uint8_t>* bot = 0; }
//HOWTO USE FSM Engine
//HOWTO create H A N D L E S 
void test1 (void* __bot
) {
	static char c = 'A';
	fsm::bot = (fsm::engine<uint8_t>*)__bot;

//HOWTO Write to output (to itself by default, override it operator<<( fsm::stream) )
	bot->ios()->put(c++);
//HOWTO Get access to fsm::engine data set from Handle
	clog	<< "State=" << (int)bot->state()
		<< " Revision=" << bot->revision() << endl;
//HOWTO Transition to (another) STATE=0
	bot->transit(0);

}
void zero(void*__data
) {
	fsm::bot = (fsm::engine<uint8_t>*)__data;
	bot->transit(0);
       	clog << "OFF, state=0" << endl;
}

void ONE (void*__data
) {
	fsm::bot = (fsm::engine<uint8_t>*)__data;
	bot->transit(1);
       	clog << "ON, state=1" << endl;
}

inline void test66 (void* __bot) { clog << "test66" << endl; }

//HOWTO create S T A T E S
typedef	map<uint8_t, Handle>	circ;
const circ switchOff {
	  { 0, test1 }
	, { 1, test1 }
	, { 4, test1 }
	, { 3, zero  }
};
const circ switchOn = {
	  { 0, ONE  }
	, { 1, zero }
	, { 2, zero }
	, { 5, zero }
};
const circ Enable = {
	  { 65, ONE }
	, { 66, test66 }
	, { 67, zero }
	, { 4, ONE  }
};
const circ Disable = {
	  { 0, test1 }
	, { 66, test66 }
	, { 3, zero }
	, { 4, ONE  }
};

//HOWTO create T A B L E OF S T A T E 
typedef	vector<circ>	States;
States _table1 = { switchOff, switchOn };
States _table2 = { Enable, Disable };

int main (int argc, char** argv
) { try {
	fsm::stream<uint8_t> src( _table1 ), dst( _table2 ), odd; 
//HOWTO init stream with table of state
	odd.table( _table1);
//HOWTO set up new handle of Event
	dst.handle( 66, test66);
//HOWTO redirect IO: make conveyor
	dst << src; //>> odd;
//HOWTO write Events
	uint8_t buff[] = { 0, 1, 2, 3, 4 };
	src.write(buff, 5);
	src.put(13);
//HOWTO run processing
	src << 255 << 55 << 10 << flush;//TODO where is 55, 10?!
	cout << "_____Destination______" << endl;
	dst.flush();
	
return 0;
} catch (const exception& e) {
	clog << "example::what() " << e.what() << endl;
return 1;
}}
//EOF
