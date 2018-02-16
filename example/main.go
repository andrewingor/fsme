//Test and examples of FSME

package main

import (
	"github.com/andrewingor/fsme"
)

var log = fsme.Log

func init() {}

func two(Event byte, State* byte)  { log.Print("Event=", Event); *State=2; } //Transit to State #2
func zero(byte, *byte) { log.Print("Zero") }
func one(byte, *byte)  { log.Print("One") }

func three(byte, *byte) { log.Print("Three") }
func four(byte, *byte)  { log.Print("Four") }
func five(byte, *byte)  { log.Print("Five") }

func broken(e byte, state* byte) { log.Print("Broken: Event=", e) }

var states = []map[byte]func(byte, *byte){
	{0: zero, 1: one, 2: two},    // State 1
	{0: three, 1: four, 2: five}, // State 2
	{0: broken},                  // State 3
}

var names = map[byte]string{0: "Zero", 1: "One", 2: "Twice"} // Named State

func main() {
	bot := fsme.New(states)

	bot.Nstate(names)
	bot.Transit(2)

	bot.Fill(0, 255, broken)
	bot.Go(143)
	bot.Go(255)

	bot.Transit(0)
	bot.Go(1)
	bot.Go(2)

	log.Print("State=", bot.StateN())
	bot.Go(24)

	bot.Transit(0)
	log.Print("State=", bot.StateN())
	bot.Go(1)
	bot.Go(0)
	bot.Go(2)
}
//EOF
