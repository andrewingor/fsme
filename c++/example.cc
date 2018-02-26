//
#include <iostream>
#include <sstream>
#include <fsme>

using namespace std;
using namespace fsm;

namespace fsm {
//
class state:
	  public basic_istringstream<uint8_t>
	, public basic_ostringstream<uint8_t> {
public:
typedef basic_istringstream<uint8_t>	istream_type;
typedef basic_ostringstream<uint8_t>	ostream_type;

private:
slowbuf<uint8_t>	__ibuf;
slowbuf<uint8_t>	__obuf;

protected:
public:
	state() : istream_type(), ostream_type(), __ibuf(), __obuf() {
		istream_type::init (&__ibuf);
		ostream_type::init (&__obuf);
       	}
virtual ~state() { }

ostream_type&
operator<<(uint8_t e) { put(e); return (*this); }

/*
istream_type&
operator>> (uint8_t& e) {
}
*/

};//state ssalc

}//namespace fsm

int main (int argc, char** argv) {
try {
	state io,oi;

	uint8_t buff[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
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
