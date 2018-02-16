//
//

#include <iostream>
#include <fsme>

using namespace std;

typedef	uint8_t	byte;

class bender:
	public fsm::bot<byte> {
private:

void	one() { clog << "One" << endl; }
void	two() { clog << "Two" << endl; }
void	thr() { clog << "Three" << endl; }

public://____________

};

void func () {
	clog << "hello, func Bender" << endl;
}

int main ( int argc, char** argv) {
try {
	map<byte, void(*)()> handl;
	handl[0] = &func;

	handl[0]();

	bender Bot;
	Bot.put('x');


return 0;
} catch (const exception& e) {
	//logs << DEBUG <<  e.what() << endl;
	return 1;
}}
//EOF
