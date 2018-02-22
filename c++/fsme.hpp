//FSME: Finite State Machine Engine
//

#include <exception>
#include <vector>
#include <string>
#include <map>

using namespace std;

namespace fsm {

//---------
/* engine ----------------
	Base of FSM
--------------------------------*/
template<  typename __iEvent
	 , typename __oEvent
	 , typename ___State >
 class engine:
	  public basic_istream<__iEvent>
	, public basic_ostream<__oEvent> {
private:
	//
public:
	 engine() { }
virtual ~engine() { }

const string&	rev() const {
		static string Rev = ("$Id$"); return Rev; }

	typedef __iEvent	event_input_type;
virtual void	go(__iEvent) = 0;
};

//----------
/* bot ----------------
	FSM Engine robot
--------------------------------*/
template< typename __iEvent = uint8_t
	, typename __oEvent = __iEvent
	, typename ___State = __iEvent >
 class bot:
	  public engine<__iEvent, __oEvent, ___State>{
private:
	//
public:
	typedef void(*Handle)(___State*);
	typedef	map<__iEvent, Handle>	___Handles;
	typedef	vector<___Handles>	___Table;
	typedef ___State		state_type;
	typedef __iEvent		event_input_type;
	typedef __oEvent		event_output_type;

___State	Current;
___Table	States;

public:
	bot () { }
virtual	~bot () { }
virtual	void	go(__iEvent Event) { States[Current][Event](&Current); }

	void transit(___State trans) { Current = trans; }

typename ___Table::iterator& state_begin() const { return States.begin(); }
typename ___Table::iterator& state_end()  const { return States.end(); }
___Table& State() { return States; }

};

}//namespace fsm

