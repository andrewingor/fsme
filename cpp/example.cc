//
#include <iostream>
#include <fsme>

using namespace std;
using namespace fsm;

//typedef	uint16_t	even_t;
//typedef	uint32_t	even_t;
typedef		uint8_t		even_t;

namespace fsm { static engine<even_t>* master = 0; }

//HOWTO <<< Use FSM Engine >>>
//HOWTO create H A N D L E S 
void outResult_transit_0 ( void* __bot
) {
//HOWTO Get access to fsm::engine data set from Handle
	static char c = 'A';
	master = ( engine<even_t>*)__bot;
//HOWTO Write to output; to itself by default, override it with operator<<( fsm::stream)
	clog	<< "Output=" << c;
	master->ios()->put(c++);
//HOWTO Transition to (another) STATE=0
	master->transit(0);
	clog	<< ", OFF, State=" << (int)master->state()
		<< ", Revision=" << master->revision()
		<< endl;
}
void transit_0 (void*__data
) {
	master = ( engine<even_t>*)__data;
	master->transit(0);
       	clog << "OFF, State=" << (int)master->state() << endl;
}

void transit_1 (void*__data
) {
	master = ( engine<even_t>*)__data;
	master->transit(1);
       	clog << "ON, State=" << (int)master->state() << endl;
}

inline void handle_66 (void* ) { clog << "Event=66" << endl; }

//HOWTO create S T A T E S: Table1
typedef	map<even_t, Handle>	circ;
static
const circ switchOff {
	  { 0, outResult_transit_0 }
	, { 1, outResult_transit_0 }
	, { 2, outResult_transit_0 }
	, { 3, transit_1 }
	, { 4, outResult_transit_0 }
	, { 65, outResult_transit_0 }
};
const circ switchOn = {
	  { 0, transit_1 }
	, { 1, transit_0 }
	, { 255, transit_0 }
	, { 2, handle_66 }
	, { 3, transit_0 }
	, { 4, outResult_transit_0 }
};

//HOWTO create S T A T E S: Table2
static
const circ Enable = {
	  { 68, outResult_transit_0 }
	, { 69, outResult_transit_0 }
	, { 70, transit_1 }
	, { 72, handle_66 }
};
const circ Disable = {
	  { 71, outResult_transit_0 }
	, { 72, handle_66 }
	, { 3, transit_0 }
	, { 4, transit_1 }
};

//HOWTO create T A B L E OF S T A T E 
typedef	vector<circ>	States;
States __table1 = { switchOff, switchOn };
States __table2 = { Enable, Disable };

int main( int argc
	, char* argv[]
) { try {
	fsm::stream<even_t> src(__table1), dst(__table2), odd; 
//HOWTO set new handle of Event
	dst.hand( 66, handle_66);

//HOWTO write Events: output result to itself
	even_t buff[] = { 0, 1, 2, 3, 4 };
	src.write(buff, 2);
	cout << "--- Source FLUSH --" << endl;
	src.flush();

//HOWTO set new table_of_state
	odd.table(__table1);
//HOWTO redirect IO: Conveyor
	//odd << dst << src;//TODO stream& operator<< (stream&, stream&)
	dst << src;
	dst >> odd;

	src.write(buff, 5);
//HOWTO run processing (sync)
	cout << "--- Source FLUSH --" << endl;
	src << 255 << flush;

	src.put(69);
//HOWTO read unprocessed events
	src >> buff[0];
//HOWTO run processing (sync)
	src << 1 << 4 << flush;//TODO Event[2] operator<< (Event, Event)
	cout << "--- Dst FLUSH --" << endl;
	dst.flush();
	cout << "--- Odd FLUSH --" << endl;
	odd.put(0);
	odd.flush();
	
return 0;
} catch (const exception& e) {
	clog << "example::what() " << e.what() << endl;
return 1;
}}
//EOF
