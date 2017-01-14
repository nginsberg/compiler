
#ifndef __TOKNAMES_H__
#define __TOKNAMES_H__

struct tok_name_pair { int num; char* name; };

typedef struct tok_name_pair tok_name;

static tok_name token_name_table[] = {

  { 258, "CLASS" }
 ,{ 259, "DEF" }
 ,{ 260, "EXTENDS" }
 ,{ 261, "IF" }
 ,{ 262, "ELIF" }
 ,{ 263, "ELSE" }
 ,{ 264, "WHILE" }
 ,{ 265, "RETURN" }
 ,{ 266, "ATLEAST" }
 ,{ 267, "ATMOST" }
 ,{ 268, "EQUALS" }
 ,{ 269, "AND" }
 ,{ 270, "OR" }
 ,{ 271, "NOT" }
 ,{ 272, "IDENT" }
 ,{ 273, "INT_LIT" }
 ,{ 274, "STRING_LIT" }
};
int num_named_tokens = 17;

#endif
