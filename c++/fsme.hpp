//FSME: Finite State Machine Engine
//(c)2018 by <a href="https://goo.gl/FFF1Up">Max Lancer</a>
//License: Devil's Contract

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
//typedef event_type	char_type;
typedef uint8_t		char_type;
typedef unsigned long 	int_type;
typedef std::streampos	pos_type;
typedef std::streamoff	off_type;

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
virtual	Handle	handle	(const event_type&, const Handle ) = 0;	//Set new Handle for Event

virtual	const state_type& state() const = 0; //Current State
//IO base
virtual	void	put( const event_type& Event) = 0;
//TODO virtual	void	write( const event_type& Event) = 0;

public:
typedef	basic_ostringstream<event_type>	ostream_type;
virtual ostream_type*	out() = 0; //Get output stream

virtual const	string&	revision() const = 0;	//Revision number, do not forget
virtual		void	clear() = 0;		//Clean up, get start to begin

typedef	map<event_type, Handle>	circuit;
typedef vector<circuit>		table_of_state;
virtual		void	table( const table_of_state& ) = 0;//Init with states vector

};//base class

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

	typedef	basic_ostringstream<event_type>	ostream_type;

virtual	inline	ostream_type*	out() {  return __optr; }
//
private:
state_type	__state;
ostream_type	__error;
ostream_type*	__optr;
//
protected:
table_of_state	States;

//
public:
//Constructor
	explicit engine() : __error() { }
	explicit engine(const table_of_state& __tbl) : __error() { this->table( __tbl); }

virtual	void table( const table_of_state& __tbl)
{ 
	__optr = &__error;
       	__state^=__state; //TODO zero type into event_traits<>
	States = __tbl;
}

//Destructor { }
virtual ~engine() { }

virtual	const string&  revision() const {
	static string rev("$Id$");
	return rev;
}

//run Fast & Dangerous
inline	virtual	void	run(const event_type& Event)		{ (*States[__state][Event])(this); }
inline	virtual	void	transit( const state_type& newState)	{ __state = newState; } 
inline	virtual	void	put( const event_type& Event)		{ __optr->put(Event); } 
inline	virtual	void	clear() { } //TODO clean up

inline	virtual	const state_type& state() const	{ return __state; } 
//TODO virtual void write (const event_type& Event, basic_ostringstream<event_type>::size_type len)
	//{ __error->write (Event, len); } 

//walk Slow & Safe
virtual	void	walk(const event_type& Event) throw()
{
	if ( States.size() <= __state)
	{
		std::ostringstream e;
		e << "Event=" << Event << "; State=" << __state
		  << "States=" << States.size() << ": Current State Invalid";
		throw std::out_of_range(e.str());
	}
	if ( States[__state].end() == States[__state].find(Event)) 
	{
		clog << "Event!" << (int)Event << "!" << endl;
		//throw bell<event_type>(Event, "Handle not exist");
	}
	else
	   run(Event);
}
//handle Set new handle for Event in current State
Handle
handle( const event_type& Event, const Handle newHandle )
{
	Handle old = States[__state][Event]; //Old Handle
	States[__state][Event] = newHandle; //New Handle
	return old;
}

inline	void	turn( ostream_type* __dest) {  __optr =  __dest; }

};//engine class


//-----------------
/* basic_eventbuf ------------
	Basic I/O FSMe buffer 
--------------------------------*/
template<  typename event_type = uint8_t
	 , typename state_type = event_type
	 , typename traits_type = event_traits<event_type> >
class basic_eventbuf:
	  public engine<event_type, state_type, traits_type>
	, public basic_stringbuf<event_type> {
//
public:
typedef engine<event_type, state_type, traits_type> engine_type;
typedef basic_stringbuf<event_type>	buf_type;

	typedef typename buf_type::char_type	char_type;
	typedef typename buf_type::int_type	int_type;
	typedef typename buf_type::pos_type	pos_type;
	typedef typename buf_type::off_type	off_type;
//
protected:
//
public:
	typedef	map<event_type, Handle>	Circuit;
	typedef vector<Circuit>		table_of_state;
//Constructor
explicit
basic_eventbuf () : buf_type(), engine_type() { }

basic_eventbuf (const table_of_state& __tbl) : buf_type(), engine_type( __tbl) { }
//Destructor
virtual
~basic_eventbuf() { }

typedef	basic_ostringstream<event_type>	ostream_type;
inline	ostream_type*	out() {  return engine_type::out(); }

//
int sync() {
	while ( buf_type::in_avail())
       	{
		char_type e = buf_type::sbumpc();
		engine_type::walk (e);
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
/* stream --------
	FSM State
--------------------------------*/
template<  typename event_type = uint8_t
	 , typename state_type = event_type
	 , typename traits_type = event_traits<event_type> >
class stream:
	public basic_ostringstream<event_type> {
public:
typedef basic_ostringstream<event_type>		ostream_type;
typedef typename ostream_type::char_type	char_type;

private:
basic_eventbuf<event_type> __obuf;

protected:
public:
	explicit
	stream() : __obuf(), ostream_type() { }
	
	typedef	map<event_type, Handle>	circ;
	typedef vector<circ>		table_of_state;

	stream(const table_of_state& __tbl ) : __obuf( __tbl), ostream_type() {
		ostream_type::init (&__obuf);
       	}
virtual ~stream() { }

Handle handle( const event_type& Event, Handle newHandle) { __obuf.handle( Event, newHandle); }

inline	void	table( const table_of_state& __tbl)	{ __obuf.table(__tbl); }

inline	ostream_type*	out() {  return __obuf.out(); }
inline	void		turn( ostream_type* __dest) {  __obuf.turn( __dest); }

ostream_type&
operator<< ( char_type e) { this->put(e); return (*this); }

ostream_type&
operator<< ( fsm::stream<event_type>& __source)
{
	__source.turn( this);
	return (*this);
}

/*
ostream_type&
operator<<( istream_type& __source)
{
	__source.turn( this);
	return (*this);
}

istream_type&
operator>> (event_type & e) {
}
*/

};//stream class

}//namespace fsm
