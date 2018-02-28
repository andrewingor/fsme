//
#include <iostream>
#include <fsme>

using namespace std;
using namespace fsm;

namespace fsm {
//
class stream:
	  public basic_istringstream<uint8_t>
	, public basic_ostringstream<uint8_t> {
public:
typedef basic_istringstream<uint8_t>	istream_type;
typedef basic_ostringstream<uint8_t>	ostream_type;

private:
basic_eventbuf<uint8_t>	__ibuf;
basic_eventbuf<uint8_t>	__obuf;

protected:
public:
	stream() : istream_type(), ostream_type(), __ibuf(), __obuf() {
		istream_type::init (&__ibuf);
		ostream_type::init (&__obuf);
       	}
virtual ~stream() { }

inline void handle ( Handle proc) { __obuf.handle(proc); }

ostream_type&
operator<<(uint8_t e) { put(e); return (*this); }

/*
istream_type&
operator>> (uint8_t& e) {
}
*/

};//state ssalc

}//namespace fsm

//TODO 1 Output result to __Out
inline void handler(void* __bot)
{
	fsm::engine<>* bot = (fsm::engine<>*)__bot;
	clog << "event=" << (int)bot->state() << endl;
}

int main (int argc, char** argv) {
try {
	fsm::stream io,oi;

	//io.handle (handler);

	uint8_t buff[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };
	io.write(buff, 8);
	io.put(13);

	io << 255 << 165 << flush;
	
	uint8_t in;
	if (io >> in) do
	{ cout << (int)in << endl;
	} while (io >> in);
	else
	    cout << "got nothing" << endl;

return 0;
} catch (const exception& e) {
	clog << "example::what()	" << e.what() << endl;
return 1;
}}
//EOF
