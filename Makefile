# Dependency rules
all: ishlex ishsyn ish
ishlex: ishlex.o lexanalyzer.o token.o dynarray.o
	gcc217 ishlex.o lexanalyzer.o token.o dynarray.o -o ishlex
ishlex.o: ishlex.c lexanalyzer.h token.h dynarray.h
	gcc217 -c ishlex.c
lexanalyzer.o: lexanalyzer.c lexanalyzer.h token.h dynarray.h
	gcc217 -c lexanalyzer.c
token.o: token.c token.h
	gcc217 -c token.c
dynarray.o: dynarray.c dynarray.h
	gcc217 -c dynarray.c

ishsyn: ishsyn.o synanalyzer.o lexanalyzer.o command.o token.o dynarray.o
	gcc217 ishsyn.o synanalyzer.o lexanalyzer.o command.o token.o dynarray.o -o ishsyn
ishsyn.o: ishsyn.c synanalyzer.h lexanalyzer.h command.h token.h dynarray.h
	gcc217 -c ishsyn.c
synanalyzer.o: synanalyzer.c synanalyzer.h command.h token.h dynarray.h
	gcc217 -c synanalyzer.c
command.o: command.c command.h token.h dynarray.h
	gcc217 -c command.c

ish: ish.o handler.o synanalyzer.o lexanalyzer.o command.o token.o dynarray.o
	gcc217 ish.o handler.o synanalyzer.o lexanalyzer.o command.o token.o dynarray.o -o ish
ish.o: ish.c handler.h synanalyzer.h lexanalyzer.h command.h token.h dynarray.h
	gcc217 -c ish.c
handler.o: handler.c command.h token.h dynarray.h
	gcc217 -c handler.c
