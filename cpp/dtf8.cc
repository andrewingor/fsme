//xUTF8coder 6-bytes sequence encoding with 4-bytes (32 bits) for code point.
//There is use 2nd bit of 1st byte FSS-UTF(1992) to store 32nd. 
//One sequence called BRUNE (Big Rune or Bit Rune). 
//At least BRUNE only can be from 1 to 6 bytes, all another has size 6 byte.
//
//It is use for big binary data transmission (like media stream)
//Base on <a href="https://github.com/fsme/">FSM Engine</a>
//
//(c)2018 by <a href="https://goo.gl/FFF1Up">Max Lancer</a>
//License: Devil's Contract

#include <iostream>
#include <vector>
#include <regex>
#include <fsme>

using namespace std;
using namespace fsm;

typedef	char	EvenT;

static fsm::engine<EvenT>* fsme;

//handle
void onebyte (void*__fsme
) {
	fsme = ( engine<EvenT>*)__fsme;
       	clog << "onebyte" << endl;
}

//handle
void append_xutf8 (void*__fsme
) {
	fsme = ( engine<EvenT>*)__fsme;
       	clog << "append_xutf8::event="  << (int)fsme->event() << endl;
	//xutf8
	fsme->transit(1);// Append xUTF8
       	clog << "append_xutf8_first" << endl;
}

//append_ascii Just output
void append_ascii (void*__fsme
) {
	fsme = ( engine<EvenT>*)__fsme;
	fsme->ios()->put( fsme->event());
       	clog << "append_ascii" << endl;
}

//handle
void append_byte1 (void*__fsme
) {
	fsme = ( engine<EvenT>*)__fsme;
	fsme->transit(2);
       	clog << "append_byte-1" << endl;
}

typedef	map<EvenT, fsm::Handle>	State_t;
typedef	vector<State_t>		States_t;

//Decoder
State_t first_byte = {
	  { 0,	 onebyte }
	, { 128, onebyte }
};
State_t non_first = {
	  { 0, onebyte  }
	, { 1, onebyte  }
};

States_t _xUTF8_decode = { first_byte, non_first };

//Encoder 
State_t header_brune = {
	  {   0, append_ascii }
	, { 255, append_xutf8 }
};

State_t append_brune = {
	  {   0, append_byte1 }
	, { 255, append_byte2 }
};

States_t _xUTF8_encode = { header_brune, append_brune };


//
int main ( int argc
	 , char* argv[]
) { try {
	vector<string> args(argc);
	reverse_iterator< vector<string>::iterator > i = args.rbegin();
	for (	i  = args.rbegin();
		i != args.rend(); 
		i++
	) {
		--argc;
		*i = argv[argc];
	}

	//Erase specific parts ./|.exe from command
	{
	  regex ext("^\\.\\/|\\.exe$");
	  args[0] = regex_replace( args[0], ext, "");
	}

	fsm::stream<EvenT> coder(_xUTF8_encode) //Encoder
			,  result; //Result
	result << coder;

	coder.transit(0); //Find 1st header_brune (first byte)
	coder.hand(    0, 0x7f, append_ascii);
	coder.hand( 0x80, 0xff, append_byte1);

	coder.transit(1); //Append 1st byte
	coder.hand(0, 0xff, append_byte2);

	coder.transit(2); //Append 2nd byte
	coder.hand(0, 0xff, append_byte3);

	coder.transit(3); //Append 3rd byte
	coder.hand(0, 0xff, append_byte4);

//TODO Non-formated input
	coder.transit(0);
	std::copy( istream_iterator<EvenT>(cin)
		 , istream_iterator<EvenT>()
		 , ostream_iterator<EvenT>(coder)
	);
	coder.flush();

	cout << "-------------" << endl;

return 0;
} catch (const exception& e) {
	//logs << debug <<  e.what() << endl;
	clog << e.what() << endl;
return 1;
}}
//EOF

