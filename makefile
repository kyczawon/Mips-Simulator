# This needs a comment

bin/simulator : src/simulator.cpp
	g++ -W -Wall src/simulator.cpp -o bin/simulator
