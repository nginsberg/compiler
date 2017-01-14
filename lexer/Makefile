toknames.h: quack.tab.h
	python3 extract_token_names.py < $< > $@

lex.yy.h: quack.l toknames.h
	flex quack.l && cp lex.yy.c lex.yy.h

scanner: scanner.c lex.yy.h toknames.h
	g++ scanner.c -lfl -o scanner
