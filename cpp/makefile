# Build example

#a.out: example.cc fsme.hpp fsme
#	g++ -I./  example.cc
#
dtf8: dtf8.cc fsme.hpp fsme
	g++ -I./  dtf8.cc

data: data.cc
	g++ -I./  data.cc -o data

clean:
	rm -rfv qq q a.out autoscan.log autom4te.cache config.log\
	       	config.status configure.ac
