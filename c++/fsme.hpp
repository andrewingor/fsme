//FSME: Finite State Machine Engine
//(c)2018 by <a href="https://goo.gl/FFF1Up">Max Lancer</a>
//License: GNU GPLv3

#include <algorithm>
#include <exception>
#include <vector>
#include <string>
#include <map>

using namespace std;

namespace fsm {

//---------
/*TODO event_traits --------------------
	Traits Event types
--------------------------------*/
template <typename event_type>
struct event_traits
{
typedef	uint32_t	int_type;
//TODO...
static	void	assign ( event_type& dst, const event_type& src ) { /* dst = src; */ }
//TODO...
};

//---------
/* bell --------------------
	FSM exception
--------------------------------*/
template <typename event_type = uint8_t>
class bell:
	  public std::string
	, public exception {
private:
event_type	Event;
public:
virtual ~bell() { }
	explicit
	bell(	  const event_type& event
		, const string& __errmsg = "fsm::bell")
	 	: string(__errmsg), Event(event) {
	}
virtual	const	char*		what()	const throw()	{ return this->c_str(); }
	const	event_type&	event()	const		{ return Event; }
};

//---------
/* engine --------------------
	Base of FSMe (interface)
--------------------------------*/
template<  typename event_type = uint8_t
	 , typename state_type = event_type
	 , typename traits_type = event_traits<event_type> >
class engine {
private:
//
protected:
virtual 	void	call	(const event_type&) = 0; //Call handler by Event
virtual		void	transit	(const state_type&) = 0; //Transit to new State

public:
	 engine() { }
virtual ~engine() { }

virtual const	string&		revision() const = 0;	//Revision number, do not forget
virtual	const	state_type&	current()  const = 0;	//TODO Delete it: Look inside for debugging
virtual		void		clear() = 0;		//Clean up, get start to begin
};//engine


//----------
/* eventbuf --------
	I/O buffer of FSM 
--------------------------------*/
template<  typename event_type = uint8_t
	 , typename state_type = event_type
	 , typename traits_type = event_traits<event_type> >
class
eventbuf: virtual public fsm::engine<event_type, state_type, traits_type>
		, public basic_stringbuf<event_type>{
//
public:
	typedef basic_stringbuf<event_type>	buf_type;
	typedef typename buf_type::char_type	char_type;
	typedef typename buf_type::int_type	int_type;
	typedef typename buf_type::pos_type	pos_type;
	typedef typename buf_type::off_type	off_type;

	typedef void(*Handle)(void*);
//
private:
//
protected:
	typedef	map<event_type, Handle>		Circuit;
	typedef vector<Circuit>			table_of_state;

table_of_state	States;
state_type	Current;

// H A N D L E S
static void
empty(void*) { }

static void
debug(void*__data) {
	clog << "event=" << *((int_type*)__data) << endl;
}
static void
zero(void*__data) {
	cout << "Switch On" << endl;
	*((uint8_t*)__data) = 1; //Transition to State 1
}
static void
one (void*__data) {
	cout << "Switch Off" << endl;
	*((uint8_t*)__data) = 0; //Transition to State 0
}
// S T A T E S
const Circuit switchOff {
	  { 0, zero }
	, { 1, one  }
};
const Circuit switchOn = {
	  { 0, one }
	, { 1, zero }
};

virtual	void	call	(const event_type& Event)	{ (*States[Current][Event])(&Current);  }
virtual	void	transit	(const state_type& Next)	{ Current = Next; }
//
public:
basic_ostringstream<uint8_t>	__out; //TODO 
//Constructor
explicit
eventbuf () : buf_type() {
	Current = event_type(0);
	States = { switchOff , switchOn };
	//TODO std::sort(States.begin(), States.end());
}
//Destructor
virtual
~eventbuf() { }
//
int
sync() {
	while ( buf_type::in_avail()) {
		char_type e = buf_type::sbumpc();
		call (e);
	}
	return buf_type::in_avail();
}

int_type
overflow (event_type c) {
	cout << "OVER:" << (int_type)c << endl;
	return 0;
}
int_type
underflow() {
	cout << "UNDER" << endl;
	return buf_type::underflow();
}
int_type
uflow() {
	cout << "UFLOW" << endl;
	return buf_type::uflow();
}

const
string&
revision() const {
	static string rev = ("$Id$");
	return rev;
}

virtual	const	state_type&	current() const	{ return Current; }
virtual		void		clear() 	{ } //TODO clean up

};//eventbuf ssalc

//----------
/* slowbuf ----------
	I/O buffer slow but safe
-------------------------------*/
template<  typename event_type = uint8_t
	 , typename state_type = event_type
	 , typename traits_type = event_traits<event_type> >
class slowbuf:
	public eventbuf<event_type, state_type, traits_type> {
//
protected:
//call Handler by Event
virtual
void call ( const event_type& Event) throw()
{
	if ( this->States.size() <= this->Current)
		throw bell<event_type>(Event, "Current State out of size");

	if ( this->States[this->Current].end() == this->States[this->Current].find(Event))
		throw bell<event_type>(Event, "Handle of Event not exist");
	//elsewhere
	(*(this->States[this->Current][Event]))(&this->Current);
}
//
public:
// Named States
const string&
stat() const {
	static map<event_type, string> NameOf = {
		  {0, "Switch Off"}
		, {1, "Switch On"}
	};
	return NameOf[this->Current];
}

};//slowbuf ssalc


//----------
/* state --------
	FSM State
--------------------------------*/
//example.cc: under constract

}//namespace fsm
