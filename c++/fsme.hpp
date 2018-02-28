//FSME: Finite State Machine Engine
//(c)2018 by <a href="https://goo.gl/FFF1Up">Max Lancer</a>
//License: GNU GPLv3

#include <algorithm>
#include <exception>
#include <vector>
#include <sstream>
#include <map>

using namespace std;

namespace fsm {

typedef void(*Handle)(void*);

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
event_type	__Event;
public:
virtual ~bell() { }
	explicit
	bell(	  const event_type& Event
		, const string& __errmsg = "fsm::bell")
	 	: string(__errmsg), __Event(Event) {
	}
virtual	const	char*		what()	const noexcept	{ return this->c_str(); }
	const	event_type&	event()	const noexcept	{ return __Event; }

};//bell ssalc

//---------
/* base --------------------
	Interface of FSMe 
--------------------------------*/
template<  typename event_type = uint8_t
	 , typename state_type = event_type
	 , typename traits_type = event_traits<event_type> >
class base {
private:
//
protected:
virtual	void	run	(const event_type& ) = 0;	//Handle Fast & Dangerous
virtual void	walk	(const event_type& ) throw() = 0;//Handle Slow & Safty
virtual	void	transit	(const state_type& ) = 0;	//Transit to new State
virtual	const state_type& state() const = 0; //Current State (e.g. Event)
//IO base
virtual	void	put( const event_type& Event) = 0;
//TODO virtual	void	write( const event_type& Event) = 0;

public:
	 base() { }
virtual ~base() { }

virtual const	string&	revision() const = 0;	//Revision number, do not forget
virtual		void	clear() = 0;		//Clean up, get start to begin
};//engine

//------------
/* engine -----------------
	Of FSM
--------------------------------*/
template<  typename event_type = uint8_t
	 , typename state_type = event_type
	 , typename traits_type = event_traits<event_type> >
class engine:
	virtual public base<event_type, state_type, traits_type> {
public:
	typedef void(*Handle)(void*);
	typedef	map<event_type, Handle>	Circuit;
	typedef vector<Circuit>		table_of_state;
//
private:
state_type			__State;
basic_ostringstream<event_type>	__Out;

// H A N D L E S TEST FOR
static void
empty(void*) { }

static void
zero(void*__data) {
	cout << "HANDLE: Switch ON" << endl;
//	*((uint8_t*)__data) = 1; //Transition to State 1
}
static void
ONE (void*__data) {
	cout << "HANDLE: Switch OFF" << endl;
//	*((uint8_t*)__data) = 0; //Transition to State 0
}
// S T A T E S TEST FOR
const Circuit switchOff {
	  { 0, zero }
	, { 1, ONE  }
};
const Circuit switchOn = {
	  { 0, ONE  }
	, { 1, zero }
};
//
protected:
table_of_state	States;
//
public:
//Constructor
	engine() : __Out() {
	       	__State = 0; //TODO zero type into event_traits
		States = { switchOff , switchOn }; //TODO TEST FOR, MOVE it
       	}
//Destructor { }
virtual ~engine() { }

virtual	const string&  revision() const {
	static string rev("$Id$");
	return rev;
}

//run Fast & Dangerous
inline	virtual	void	run(const event_type& Event)		{ (*States[__State][Event])(this); }
inline	virtual	void	transit( const state_type& newState)	{ __State = newState; } 
inline	virtual	void	put( const event_type& Event)		{ __Out.put(Event); } 
inline	virtual	void	clear() { } //TODO clean up

inline	virtual	const state_type& state() const	{ return __State; } 
//TODO virtual void write (const event_type& Event, basic_ostringstream<event_type>::size_type len)
	//{ __Out.write (Event, len); } 


static
void never_mind(void* Event) { clog << "Event=" << (int)(*(event_type*)Event) << ": Handle not exist" << endl; }

Handle	__default_handle = never_mind;
inline void handle ( Handle proc) { __default_handle = proc; }

//walk Slow & Safe
virtual void	walk(const event_type& Event) throw()
{
	if ( States.size() <= __State)
	throw bell<event_type>(Event, "Current State out of size")
	;
	if ( States[__State].end() == States[__State].find(Event)) {
		static event_type p;
		p = Event; 
		(*__default_handle)( &p);
	}
	else
	   run(Event);
}

};//engine ssalc


//-----------------
/* basic_eventbuf ------------
	Basic I/O FSMe buffer 
--------------------------------*/
template<  typename event_type = uint8_t
	 , typename state_type = event_type
	 , typename traits_type = event_traits<event_type> >
class basic_eventbuf:
	public basic_stringbuf<event_type> {
//
public:
	typedef basic_stringbuf<event_type>	buf_type;
	typedef typename buf_type::char_type	char_type;
	typedef typename buf_type::int_type	int_type;
	typedef typename buf_type::pos_type	pos_type;
	typedef typename buf_type::off_type	off_type;
//
protected:
engine<event_type, state_type, traits_type> FSM;
//
public:
//Constructor
explicit
basic_eventbuf () : buf_type() { }
//Destructor
virtual
~basic_eventbuf() { }

inline void handle ( Handle proc) { FSM.handle(proc); }

//
int
sync() {
	while ( buf_type::in_avail()) {
		char_type e = buf_type::sbumpc();
		FSM.walk (e);
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


};//basic_eventbuf ssalc

//----------
/* state --------
	FSM State
--------------------------------*/
//example.cc: under constract

}//namespace fsm
