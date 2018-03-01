//
#include <iostream>
#include <fsme>

using namespace std;
using namespace fsm;

// H A N D L E S Exapmle 
inline void test1 (void* __bot)
{
	static char c = 'A';
	fsm::engine<>* bot = (fsm::engine<>*)__bot;

	bot->out()->put(c++);	//Output to ostream (set it operator<<( fsm::stream) )
	clog	<< "State=" << (int)bot->state() //Access to fsm::engine data set
		<< " Revision=" << bot->revision() << endl;
	bot->transit(0);	//Transition to State=0

}
inline void test2 (void* __bot) { clog << "test2" << endl; }
inline void zero(void*__data) { clog << "OFF" << endl; }
inline void ONE (void*__data) { clog << "ON" << endl; }

// S T A T E S TEST4
typedef	map<uint8_t, Handle>	circ;
const circ switchOff {
	  { 0, test1 }
	, { 1, test1  }
	, { 4, test1  }
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
	, { 66, test2 }
	, { 67, zero }
	, { 4, ONE  }
};
const circ Disable = {
	  { 0, test1 }
	, { 1, test2 }
	, { 3, zero }
	, { 4, ONE  }
};


int main (int argc, char** argv) {
try {
	typedef	vector<circ>	States;
	States tb1 = { switchOff, switchOn };
	States tb2 = { Enable, Disable };

	fsm::stream<uint8_t> src( tb1), dst( tb2), err; 

	err.table( tb1);

	dst.handle( 66, test1);

	dst << src; // >> err;

	src.handle( 2, test1);
	src.handle( 3, test1);

	uint8_t buff[] = { 0, 2, 3, 1, 5, 6, 7, 8 };
	src.write(buff, 8);
	src.put(13);

	src << 255 << flush;

	basic_istringstream<uint8_t> res( src.out()->str());

	uint8_t c;
	while ( res.get(c) )
		cout << "out:" << c <<  endl;

	cout << "-----------------------" << endl;
	dst.flush();
	
return 0;
} catch (const exception& e) {
	clog << "example::what()	" << e.what() << endl;
return 1;
}}
//EOF
