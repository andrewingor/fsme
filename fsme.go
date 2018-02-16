//This is an implementation of Finite State Machine. It handling Events byte by byte.
//FSM has current state (byte), State - slice of map of handles, Event (byte) uses as key: map[Event] = Handler()
//TODO ------------------------------------------------
//TODO IN, OUT and Err is a queue of Events, it channels
//TODO Event has priority, queue can be reordered and handling can be interrupt for realtime work.
//TODO ------------------------------------------------

package fsme

import (
	"log"
	"os"
	"sync"
)

var Log = log.New(os.Stderr, "", 0)

type Robot struct {
	FSME Bot
	IN   chan byte
	OUT  chan byte
	Err  chan byte
}

//A Bot represents a FSM Engine
type Bot struct {
	mux   sync.Mutex            //protect concurrent
	curr  byte                  //current state
	state []map[byte]func(byte, *byte) //array (slice) of states
	names map[byte]string       //named states (for debugging)
}

//New Creates a new FSMe Bot
func New(States []map[byte]func(byte, *byte)) *Bot {
	return &Bot{curr: 0, state: States}
}

//Nstate set names of States (for debbuging)
func (the *Bot) Nstate(names map[byte]string) {
	the.names = names
}

//Fill filling current state
func (the *Bot) Fill(Low, High byte, Handle func(byte, *byte)) {
	for ; Low != High; Low++ {
		the.state[the.curr][Low] = Handle
	}
	the.state[the.curr][High] = Handle
}

//State name of current state
func (the *Bot) StateN() string { return the.names[the.curr] }

//State byte of current state
func (the *Bot) State() byte { return the.curr }

//Transit transition to new State
func (the *Bot) Transit(newState byte) {
	the.curr = newState //Transition
}

//Go call Handle of Event
func (the *Bot) Go(Event byte) bool {
	_, ok := the.state[the.curr][Event]
	if ok { the.state[the.curr][Event](Event, &the.curr) //Run
	} else {
		Log.Print("No Handle: State=",
			the.curr, "(", the.names[the.curr], ") ",
			"Event=", Event)
	}
	return ok
}

//Lock mutex
func (the *Bot) Lock() { the.mux.Lock() }

//UnLock mutex
func (the *Bot) UnLock() { the.mux.Unlock() }

//EOF
