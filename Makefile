SOURCES = ClassHierarchy.cpp Methods.cpp Expressions.cpp Statements.cpp Static.cpp
HEADERS = ClassHierarchy.h Methods.h Expressions.h Statements.h
OBJECTS = $(SOURCES:.cpp=.o)
DOTS = graph/ast.dot
GRAPHS = $(DOTS:.dot=.png)
WARN = -Wno-deprecated-register

OS = $(shell uname)
ifeq ($(OS), Darwin)
LIBS = -ll -lgc
else
LIBS = -lfl -lgc
endif

CC = g++
CFLAGS = -std=c++11 -g

%.o: %.cpp
	$(CC) -c $(CFLAGS) $<

%.png: %.dot
	dot -T png -O $<

default: parser

graphs: $(GRAPHS)

quack.tab.c quack.tab.h: quack.y
	bison -d quack.y

lex.yy.c: quack.l quack.tab.h
	flex quack.l

parser: lex.yy.c quack.tab.c quack.tab.h $(OBJECTS)
	g++ -std=c++11 $(OBJECTS) quack.tab.c lex.yy.c $(LIBS) $(WARN) -o $@

clean:
	$(RM) parser quack.tab.* lex.yy.c $(OBJECTS)
