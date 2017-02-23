SOURCES = ClassHierarchy.cpp Methods.cpp Expressions.cpp Statements.cpp
HEADERS = ClassHierarchy.h Methods.h Expressions.h Statements.h
OBJECTS = $(SOURCES:.cpp=.o)
LIBS = -lfl -lgc
CC = g++
CFLAGS = -std=c++11 -g

%.o: %.cpp
	$(CC) -c $(CFLAGS) $<

default: parser

quack.tab.c quack.tab.h: quack.y
	bison -d quack.y

lex.yy.c: quack.l quack.tab.h
	flex quack.l

parser: lex.yy.c quack.tab.c quack.tab.h $(OBJECTS)
	g++ -std=c++11 $(OBJECTS) quack.tab.c lex.yy.c $(LIBS) -o $@

clean:
	$(RM) parser quack.tab.* lex.yy.c $(OBJECTS)
