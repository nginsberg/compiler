SOURCES = ClassHierarchy.cpp Methods.cpp
HEADERS = ClassHierarchy.h Methods.h
OBJECTS = $(SOURCES:.cpp=.o)
LIBS = -lfl
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
	g++ -std=c++11 $(OBJECTS) quack.tab.c lex.yy.c -lfl -lgc -o parser

clean:
	$(RM) parser quack.tab.* lex.yy.c $(OBJECTS)
