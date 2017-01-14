import sys

prologue = """
#ifndef __TOKNAMES_H__
#define __TOKNAMES_H__

struct tok_name_pair { int num; char* name; };

typedef struct tok_name_pair tok_name;

static tok_name token_name_table[] = {
"""

def scan_for_defs(f):
    print(prologue)
    sep = " "
    string_token_count = 0
    for line in f:
        # In quack.tab.h, the only lines containing '=' are in the enumeration
        # of token name
        if "=" in line:
            parts = line.split()
            assert parts[1] == "="
            name = parts[0]
            num = parts[2].strip(",")
            print(' {}{{ {}, "{}" }}'.format(sep, num, name))
            string_token_count += 1
            sep = ","
    print("};")
    # Plus it helps to know how many
    print ("int num_named_tokens = {};\n".format(string_token_count))
    print("#endif")

def main():
    scan_for_defs(sys.stdin)

if __name__ == "__main__":
    main()
