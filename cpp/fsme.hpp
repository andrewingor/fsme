//FSME: Finite State Machine Engine
//(c)2018 by <a href="https://goo.gl/FFF1Up">Max Lancer</a>
//License: Devil's Contract

#include <algorithm>
#include <exception>
#include <vector>
#include <sstream>
#include <map>
#include <mutex>

using namespace std;

namespace fsm {

typedef void(*Handle)(void*);

//---------
/*TODO event_traits --------------------
	Traits Event types
--------------------------------*/
template <typename even_t>
struct event_traits
{
//typedef even_t	char_type;
typedef uint8_t		char_type;
typedef unsigned long 	int_type;
typedef std::streampos	pos_type;
typedef std::streamoff	off_type;

//TODO...
static	void	assign ( even_t& dst, const even_t& src ) { /* dst = src; */ }
//TODO...
};

//---------
/* bell --------------------
	FSM exception
--------------------------------*/
template <typename even_t = uint8_t>
class bell:
	  public std::string
	, public exception {
private:
even_t	__Event;
public:
virtual ~bell() { }
	explicit
	bell(	  const even_t& Event
		, const string& __errmsg = "fsm::bell")
	 	: string(__errmsg), __Event(Event) {
	}
virtual
const char*	what()	const noexcept	{ return this->c_str(); }
const even_t&	event()	const noexcept	{ return __Event; }

};//bell ssalc

//---------
/* base --------------------
	Interface of FSMe 
--------------------------------*/
template<  typename even_t = uint8_t
	 , typename state_type = even_t
	 , typename traits_type = event_traits<even_t> >
class base {
protected:
virtual	void	call	(const even_t& ) = 0;		//Call Handle
virtual	void	transit	(const state_type& ) = 0;		//Transit to new State
virtual	void	hand	(even_t, even_t, const Handle ) = 0;//Set new Handle for Event
//Base data
virtual	const state_type&	state() const = 0; //Current State
virtual	const even_t&		event() const = 0; //Current Event
//Concurrent
virtual	void	  		lock(const bool ) = 0;	//Lock/Unlock engine::mutex
//IO base
virtual	void	put( const even_t& Event) = 0;
//TODO virtual	void	write( const even_t& Event) = 0;
//TODO virtual	void	get( even_t Event) = 0;

public:
virtual const	string&	revision() const = 0;	//Revision number, do not forget
virtual		void	clear() = 0;		//Clean up, get start to begin

	typedef	map<even_t, Handle>	State;
	typedef vector<State>		States;

virtual		void	table( const States& ) = 0;//Set New Table of States

};//base class

//------------
/* engine -----------------
	Of FSM
--------------------------------*/
template<  typename even_t = uint8_t
	 , typename state_type = even_t
	 , typename traits_type = event_traits<even_t> >
class engine:
	virtual public base<even_t, state_type, traits_type> {
public:
	typedef	basic_stringstream<even_t>	stream_type;

	typedef void(*Handle)(void*);
	typedef	map<even_t, Handle>	State;
	typedef vector<State>		Table_of;

//Base data
protected:
even_t		__event;
Table_of	__States;
state_type	__current_state;
stream_type*	__ios_ptr;
mutex		__mux;
//
public:
//Constructor
explicit
engine(	stream_type* __master
) {
	__ios_ptr = __master;
}
explicit
engine(	  stream_type*	  __master
	, const Table_of& __tbl
) {
	__ios_ptr = __master;
	this->table( __tbl);
}

virtual
stream_type* ios() {  return __ios_ptr; }

virtual
void table( const Table_of& __tbl
) { 
       	__current_state ^=__current_state; //TODO zero type into event_traits<>
	__States = __tbl;
}

//Destructor { }
virtual ~engine() { }

virtual
const string&  revision(
) const {
	static string rev("$Id$");
	return rev;
}

//call Fast & Dangerous
virtual	void	transit( const state_type& __newState)	{ __current_state = __newState; } 
virtual	void	put( const even_t& __Event)		{ __ios_ptr->put(__Event); } 
virtual	void	clear() { } //TODO CLEAN UP
//virtual	void	call( const even_t& __Event) throw()	{ (*__States[__current_state][__Event])(this); }
//
//call Slow & Safe for debugging
virtual	void	call ( const even_t& __Event
) throw() {
	__event = __Event;
	if ( __States.size() <= __current_state)
	{
		std::stringstream msg;
		msg	<< "Event#" << (int)__Event
			<< "; State#" << (int)__current_state
			<< "; States.size=" << __States.size()
			<< ": current State invalid";
		throw std::out_of_range( msg.str());
	}
	if ( __States[__current_state].end() == __States[__current_state].find(__Event)) 
	{
		clog << "Event!" << (int)__Event << "! handle not exist" << endl;
		//throw bell<even_t>(Event, "Handle not exist");
	}
	else
	  ( *__States[__current_state][__Event])(this);
}

virtual const state_type&	state() const	{ return __current_state; } 
virtual	const even_t&		event() const 	{ return __event; } 

virtual
void lock( const bool LockIt
) {
	if (LockIt)
		__mux.lock();
	else	__mux.unlock();
}

void redirect( stream_type* __dest) {  __ios_ptr =  __dest; }

//handle Set new handle for Event in current State
virtual
void hand( even_t _from
    	 , even_t _to
    	 , const Handle _newHandle
) {
	do { __States[__current_state][_from++] = _newHandle; //New Handle
	}
	 while( _from != _to);
}

};//engine class


//-----------------
/* basic_eventbuf ------------
	Basic I/O FSMe buffer 
--------------------------------*/
template<  typename even_t = uint8_t
	 , typename state_type = even_t
	 , typename traits_type = event_traits<even_t> >
class basic_eventbuf:
	  public engine<even_t, state_type, traits_type>
	, public basic_stringbuf<even_t> {
//
public:
typedef engine<even_t, state_type, traits_type> engine_type;
typedef basic_stringbuf<even_t>	buffer_type;

	typedef typename buffer_type::char_type	char_type;
	typedef typename buffer_type::int_type	int_type;
	typedef typename buffer_type::pos_type	pos_type;
	typedef typename buffer_type::off_type	off_type;
//
protected:
//
public:
	typedef	map<even_t, Handle>	State;
	typedef vector<State>		Table_of;

	typedef basic_stringstream<even_t>	stream_type;
//Constructor
explicit
basic_eventbuf(	stream_type* __master
		) :
		    buffer_type()
		  , engine_type(__master
) {
}
basic_eventbuf (  stream_type* __master
		, const Table_of& __tbl 
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
		char_type Event = buffer_type::sbumpc();
		//clog << "debug::sync::sbumpc=" << (uint8_t)e << endl;
		engine_type::call(Event);
	}
	return buffer_type::in_avail();
}

int_type
overflow( even_t c
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
template<  typename even_t = uint8_t
	 , typename state_type = even_t
	 , typename traits_type = event_traits<even_t> >
class stream:
	public basic_stringstream<even_t> {
public:
typedef basic_stringstream<even_t>		stream_type;
typedef typename stream_type::char_type		char_type;

	typedef	map<even_t, Handle>		State;
	typedef vector<State>			Table_of;
	typedef basic_eventbuf<even_t>		buffer_type;

protected:
basic_eventbuf<even_t>* __ptr_buf;

public:
//Constructor1
explicit
stream() : stream_type(
) { 
	__ptr_buf = new buffer_type(this);
}
//Constructor2
stream( const Table_of& __tbl
      )
	: stream_type()
{
	__ptr_buf = new buffer_type( this, __tbl);
	stream_type::init ( __ptr_buf);
}
//Constructor3
stream(  basic_eventbuf<even_t>* __externbuf
	, const Table_of& __tbl
      )
	: stream_type()
{
	__ptr_buf = new buffer_type(this, __externbuf);
	stream_type::init ( __ptr_buf);
}
//Destructor
virtual
~stream() { delete __ptr_buf; }

//hand New handle
virtual
void hand( even_t _from
	 , even_t _to
	 , const Handle _newHandle
) {
	__ptr_buf->hand( _from, _to, _newHandle);
}

//table New table of state
void table( const Table_of& __tbl
) {
       	__ptr_buf->table(__tbl);
	stream_type::init ( __ptr_buf);
}

void redirect( stream_type* __dest
) {
       	__ptr_buf->redirect( __dest);
}

//
inline void	transit( const state_type& __newState)	{ __ptr_buf->transit(__newState); } 

stream_type*	ios() {  return __ptr_buf->ios(); }

inline
stream_type&
operator<<( even_t Event
) {
	this->put( Event);
	return(*this);
}

inline
stream_type&
operator<< ( fsm::stream<even_t>& __src
) {
	__src.redirect( this);
	return(*this);
}

//IO redirect
stream_type&
operator>> ( char_type& __e) { __e = this->get(); return(*this); }

//IO redirect
stream_type&
operator>> ( stream_type& __dst
) {
	this->redirect( &__dst);
	return(*this);
}

};//fsm::stream class


//TODO------------------------
template<  typename even_t = uint8_t
	 , typename traits_type = event_traits<even_t> >
inline
even_t*
operator<<(  even_t e1
	   , even_t e2
) {
	clog << "operator<<(" << (int)e1<< "," << (int)e2 << ")!" << endl;
	return &e2;
}
//TODO--------------------------
template<  typename even_t = uint8_t
	 , typename traits_type = event_traits<even_t> >
inline
stream<even_t>&
operator<<(  stream<even_t>& __dst
	   , even_t Event
) {
	clog << "operator<<(__dst, Event)!" << endl;
	return __dst.write ( &Event, 1);
}
//TODO---------------------------
template<  typename even_t = uint8_t
	 , typename traits_type = event_traits<even_t> >
inline
stream<even_t>&
operator<<( stream<even_t>& __dst
	  , stream<even_t>& __src
) {
	__src.redirect( &__dst);
	return __src;
}

}//namespace fsm
