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
/* panic --------------------
	FSM exception
--------------------------------*/
class panic: public exception {
string	What;
//TODO Event;
public:
	panic(const string& msg) {
		What = msg;
		//TODO logs << warn << What << endl; 
		clog << What << endl;
	}
virtual ~panic() { }
virtual	const char* what() const noexcept { return What.c_str(); }
};

//---------
/* engine --------------------
	Base of FSMe (interface)
--------------------------------*/
template<  typename event_in_type = uint8_t
	 , typename event_out_type = event_in_type 
	 , typename state_type = event_in_type >
class engine {
private:
	//
protected:
virtual 	void	proc (const event_in_type&) = 0; //Call handler by Event
virtual		void	transit (const state_type&) = 0; //Transit to new State

public:
	 engine() { }
virtual ~engine() { }

virtual const	string&	revision() const = 0;		//Revision number, don't forget
//TODO virtual	void clear() = 0;			//Clean up, go to begin
//
virtual	const	state_type& current() const = 0;	//TODO Delete it: Look inside for debugging
};//engine

//----------
/* state --------
	FSM State
--------------------------------*/
template< typename event_in_type = uint8_t
	, typename event_out_type = event_in_type
	, typename state_type = event_in_type >
 class state:
	  public engine<event_in_type, event_out_type, state_type>
	, public basic_istream<event_in_type>
	, public basic_ostream<event_out_type> {
private:
protected:
//proc Call process by Event
//Fast But Dangerous: check it out for existence
virtual		void	proc (const event_in_type& Event) {
						(*States[Current][Event])(&Current);  }
//transit To next (another) State
//Fast But Dangerous: check it out for existence
virtual		void	transit (const state_type& Next) {
						Current = Next; }
public:
//TODO basic_ostream<error_type>	err;

	typedef void(*Handle)(void*);
	typedef	map<event_in_type, Handle>	Circuit;
	typedef vector<Circuit>			table_of_state;

table_of_state	States;
state_type	Current;

public:
	 state() { }
virtual	~state() { }
//
virtual const	string&	revision() const = 0;
//
virtual	const	state_type& current() const {
					return Current; } //TODO void*__Data include Current
};//state

//----------
/* simple ----------
	FSMe example
---------------------------*/
class simple:
	public state<>{ //Event type uint8_t by default
private:
// -------- H A N D L E S
static void	zero(void*_Data) {
				cout << "Switch On" << endl; *((uint8_t*)_Data) = 1; }
//
static void	one (void*_Data) {
				cout << "Switch Off" << endl; *((uint8_t*)_Data) = 0; }
// -------- S T A T E S
const Circuit switchOff {
	  { 0, zero }
	, { 1, one  }
};
const Circuit switchOn = {
	  { 0, one }
	, { 1, zero }
};
//
protected:
//proc Call handle of Event
//Slow: check it out for existence State and Event
virtual void	proc (const uint8_t& Event) 
	{	if ( States.size() <= Current)
			throw panic("Current State out of size");
		if ( States[Current].end() == States[Current].find(Event))
			throw panic("Event not found");

		(*States[Current][Event])(&Current);
	}
//
public:
	simple() {
		state::Current = 0;
		state::States = { switchOff , switchOn };
		//TODO std::sort(States.begin(), States.end());
	}
virtual	~simple() { }
//
virtual	const	string&	revision() const {
					static string rev = ("$Id$"); return rev; }
// Named States
	const	string& stat() const {
			static map<uint8_t, string> NameOf = {
				  {0, "Switch Off"}
				, {1, "Switch On"}
			};
			return NameOf[state::Current];
		}
//Call process by Event
		void	go (uint8_t Event) { proc(Event); }
};//simple


}//namespace fsm
