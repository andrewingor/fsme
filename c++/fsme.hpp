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
protected:
virtual	void	call	( const event_type& ) = 0;		//Call Handle
virtual	void	transit	( const state_type& ) = 0;		//Transit to new State
virtual	Handle	handle	( const event_type&, const Handle ) = 0;//Set new Handle for Event

virtual	const state_type& state() const = 0; //Current State
//IO base
virtual	void	put( const event_type& Event) = 0;
//TODO virtual	void	write( const event_type& Event) = 0;
//TODO virtual	void	get( event_type Event) = 0;

public:
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

	typedef	basic_stringstream<event_type>	stream_type;

virtual	stream_type*	ios() {  return __ios_ptr; }
//
private:
//
protected:
table_of_state	States;
state_type	__state;

stream_type*	__ios_ptr;
//
public:

//Constructor
explicit
engine(	stream_type* __master
) {
	__ios_ptr = __master;
}
explicit
engine(	  stream_type* __master
	, const table_of_state& __tbl
) {
	__ios_ptr = __master;
	this->table( __tbl);
}

virtual
void table( const table_of_state& __tbl)
{ 
       	__state ^=__state; //TODO zero type into event_traits<>
	States = __tbl;
}

//Destructor { }
virtual ~engine() { }

virtual	const string&  revision() const {
	static string rev("$Id$");
	return rev;
}

//call Fast & Dangerous
inline	virtual	void	transit( const state_type& newState)	{ __state = newState; } 
inline	virtual	void	put( const event_type& Event)		{ __ios_ptr->put(Event); } 
inline	virtual	void	clear() { } //TODO CLEAN UP
//inlinevirtual	void	call( const event_type& Event)		{ (*States[__state][Event])(this); }
//
//call Slow & Safe for debug
virtual	void	call (const event_type& Event) throw()
{
	if ( States.size() <= __state)
	{
		std::stringstream msg;
		msg	<< "Event#" << (int)Event
			<< "; State#" << (int)__state
			<< "; States.size=" << States.size()
			<< ": current State invalid";
		throw std::out_of_range( msg.str());
	}
	if ( States[__state].end() == States[__state].find(Event)) 
	{
		clog << "Event!" << (int)Event << "!" << endl;
		//throw bell<event_type>(Event, "Handle not exist");
	}
	else
	( *States[__state][Event])(this);
}

inline	virtual	const state_type& state() const	{ return __state; } 

//handle Set new handle for Event in current State
virtual
Handle handle(  const event_type& Event
	      , const Handle newHandle )
{
	Handle old = States[__state][Event]; //Old Handle
	States[__state][Event] = newHandle; //New Handle
	return old;
}

inline	void	redirect( stream_type* __dest) {  __ios_ptr =  __dest; }

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
typedef basic_stringbuf<event_type>	buffer_type;

	typedef typename buffer_type::char_type	char_type;
	typedef typename buffer_type::int_type	int_type;
	typedef typename buffer_type::pos_type	pos_type;
	typedef typename buffer_type::off_type	off_type;
//
protected:
//
public:
	typedef	map<event_type, Handle>	Circuit;
	typedef vector<Circuit>		table_of_state;

	typedef basic_stringstream<event_type>	stream_type;
//Constructor
explicit
basic_eventbuf(	stream_type* __master
		) :
		    buffer_type()
		  , engine_type(__master
) {
}
basic_eventbuf (  stream_type* __master
		, const table_of_state& __tbl 
) : buffer_type()
  , engine_type( __master, __tbl)
{ }
//Destructor
virtual
~basic_eventbuf()
{ }

inline	stream_type*	ios() {  return engine_type::ios(); }

//
virtual
int sync(
) {
	while ( buffer_type::in_avail())
       	{
		char_type e = buffer_type::sbumpc();
		//clog << "debug::sync::sbumpc=" << (uint32_t)e << endl;
		engine_type::call( e);
	}
	return buffer_type::in_avail();
}

int_type
overflow( event_type c
) {
	clog << "debug over:" << (int_type)c << endl;
	return 0;
}
int_type
underflow(
) {
	clog << "debug under" << endl;
	return buffer_type::underflow();
}
int_type
uflow(
) {
	clog << "debug uflow" << endl;
	return buffer_type::uflow();
}

};//basic_eventbuf class

//----------
/* stream --------
	FSM I/O stream
--------------------------------*/
template<  typename event_type = uint8_t
	 , typename state_type = event_type
	 , typename traits_type = event_traits<event_type> >
class stream:
	public basic_stringstream<event_type> {
public:
typedef basic_stringstream<event_type>		stream_type;
typedef typename stream_type::char_type		char_type;

	typedef	map<event_type, Handle>		Circuit;
	typedef vector<Circuit>			table_of_state;
	typedef basic_eventbuf<event_type>	buffer_type;

protected:
basic_eventbuf<event_type>* __ptr_buf;

public:
explicit
stream() : stream_type()
{ 
	__ptr_buf = new buffer_type(this);
}
stream( const table_of_state& __tbl )
	: stream_type()
{
	__ptr_buf = new buffer_type(this, __tbl);
	stream_type::init ( __ptr_buf);
}
stream(  basic_eventbuf<event_type>* __externbuf
	, const table_of_state& __tbl )
	: stream_type()
{
	__ptr_buf = new buffer_type(this, __externbuf);
	stream_type::init ( __ptr_buf);
}
virtual
~stream() { delete __ptr_buf; }
//
Handle handle(	  const event_type& Event
		, Handle newHandle)
{
	__ptr_buf->handle( Event, newHandle);
}

inline	void	table( const table_of_state& __tbl)	{ __ptr_buf->table(__tbl); }

inline	stream_type*	ios() {  return __ptr_buf->ios(); }
inline	void		redirect( stream_type* __dest) {  __ptr_buf->redirect( __dest); }

inline
stream_type&
operator<<( event_type E
) {
	this->put(E);
	return(*this);
}

inline
stream_type&
operator<< ( fsm::stream<event_type>& __src
) {
	__src.redirect( this);
	return(*this);
}

stream_type&
operator>> ( char_type& __e) { __e = this->get(); return(*this); }

stream_type&
operator>> ( stream_type& __s) { ; return(*this); }


};//fsm::stream class

//
template<  typename event_type = uint8_t
	 , typename traits_type = event_traits<event_type> >
inline
event_type*
operator<<(  event_type e1
	   , event_type e2
) {
	clog << "operator<<(" << (int)e1<< "," << (int)e2 << ")!" << endl;
	return &e2;
}
//
template<  typename event_type = uint8_t
	 , typename traits_type = event_traits<event_type> >
inline
stream<event_type>&
operator<<(  stream<event_type>& __dst
	   , event_type Event
) {
	clog << "operator<<(__dst, Event)!" << endl;
	return __dst.write ( &Event, 1);
}
//
template<  typename event_type = uint8_t
	 , typename traits_type = event_traits<event_type> >
inline
stream<event_type>&
operator<<( stream<event_type>& __dst
	  , stream<event_type>& __src
) {
	__src.redirect( &__dst);
	return __src;
}

}//namespace fsm
