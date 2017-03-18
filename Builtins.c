/*
 * The built-in classes of Quack
 * (incomplete implementation)
 *
 */
#include <stdIO.h>
#include <stdlib.h>  /* Malloc lives here; might replace with gc.h    */
#include <String.h>  /* For strcpy; might replace with cords.h from gc */

#include "Builtins.h"


/* ==============
 * Obj
 * Fields: None
 * Methods:
 *    Constructor  (called after allocation)
 *    STRING
 *    PRINT
 *    EQUALS
 *
 * ==============
 */

class_Obj the_class_Obj;

/* Constructor */
obj_Obj new_Obj() {
  obj_Obj new_thing = (obj_Obj) malloc(sizeof(struct obj_Obj_struct));
  new_thing->clazz = the_class_Obj;
  return new_thing;
}

/* Obj:STRING */
obj_String Obj_method_STRING(obj_Obj this) {
  long addr = (long) this;
  char *rep;
  asprintf(&rep, "<Object at %ld>", addr);
  obj_String str = str_literal(rep);
  return str;
}



/* Obj:PRINT */
obj_Obj Obj_method_PRINT(obj_Obj this) {
  obj_String str = this->clazz->STRING(this);
  fprintf(stdout, "%s", str->text);
  return this;
}

/* Obj:EQUALS (Note we may want to replace this */
obj_Boolean Obj_method_EQUAL(obj_Obj this, obj_Obj other) {
  if (this == other) {
    return lit_true;
  } else {
    return lit_false;
  }
}


/* The Obj Class (a singleton) */
struct  class_Obj_struct  the_class_Obj_struct = {
  new_Obj,     /* Constructor */
  Obj_method_STRING,
  Obj_method_EQUAL,
  Obj_method_PRINT
};

class_Obj the_class_Obj = &the_class_Obj_struct;


/* ================
 * String
 * Fields:
 *    One hidden field, currently holding char*
 * Methods:
 *    Those of Obj, plus ordering, concatenation
 * ==================
 */

/* Constructor */
obj_String new_String() {
  obj_String new_thing = (obj_String) malloc(sizeof(struct obj_String_struct));
  new_thing->clazz = the_class_String;
  return new_thing;
}

/* String:STRING */
obj_String String_method_STRING(obj_String this) {
  return this;
}

/* String:PRINT */
obj_String String_method_PRINT(obj_String this) {
  fprintf(stdout, "%s", this->text);
  return this;
}

/* String:PLUS */
obj_String String_method_PLUS(obj_String this, obj_String other) {
  char *newData = malloc(1 + strlen(this->text) + strlen(other->text));
  strcpy(newData, this->text);
  strcat(newData, other->text);
  obj_String res = new_String();
  res->text = newData;
  return res;
}

obj_Boolean String_method_ATMOST(obj_String this, obj_String other) {
  if (strcmp(this->text, other->text) <= 0) {
    return lit_true;
  } else {
    return lit_false;
  }
}

obj_Boolean String_method_LESS(obj_String this, obj_String other) {
  if (strcmp(this->text, other->text) < 0) {
    return lit_true;
  } else {
    return lit_false;
  }
}

obj_Boolean String_method_ATLEAST(obj_String this, obj_String other) {
  if (strcmp(this->text, other->text) >= 0) {
    return lit_true;
  } else {
    return lit_false;
  }
}

obj_Boolean String_method_MORE(obj_String this, obj_String other) {
  if (strcmp(this->text, other->text) > 0) {
    return lit_true;
  } else {
    return lit_false;
  }
}

/* String:EQUALS (Note we may want to replace this */
obj_Boolean String_method_EQUALS(obj_String this, obj_String other) {
  if (strcmp(this->text,other->text) == 0) {
    return lit_true;
  } else {
    return lit_false;
  }
}

/* The String Class (a singleton) */
struct  class_String_struct  the_class_String_struct = {
  new_String,     /* Constructor */
  String_method_STRING,
  Obj_method_EQUAL,
  String_method_PRINT,
  String_method_PLUS,
  String_method_ATMOST,
  String_method_LESS,
  String_method_ATLEAST,
  String_method_MORE,
  String_method_EQUALS
};

class_String the_class_String = &the_class_String_struct;

/*
 * Internal use function for creating String objects
 * from char*.  Use this to create string literals.
 */
obj_String str_literal(char *s) {
  obj_String str = the_class_String->constructor();
  str->text = s;
  return str;
}

/* ================
 * Boolean
 * Fields:
 *    One hidden field, an int (0 for False, -1 for True)
 * Methods:
 *    Those of Obj
 * =================
 */
/* Constructor */
obj_Boolean new_Boolean() {
  obj_Boolean new_thing = (obj_Boolean)
    malloc(sizeof(struct obj_Boolean_struct));
  new_thing->clazz = the_class_Boolean;
  return new_thing;
}

/* Boolean:STRING */
obj_String Boolean_method_STRING(obj_Boolean this) {
  if (this == lit_true) {
    return str_literal("true");
  } else if (this == lit_false) {
    return str_literal("false");
  } else {
    return str_literal("!!!BOGUS BOOLEAN");
  }
}

obj_Boolean Boolean_method_EQUALS(obj_Boolean this, obj_Boolean other) {
  if (this == other) {
    return lit_true;
  } else {
    return lit_false;
  }
}

obj_Boolean Boolean_method_OR(obj_Boolean this, obj_Boolean other) {
  if (this == lit_true) {
    return lit_true;
  }
  if (other == lit_true) {
    return lit_true;
  }
  return lit_false;
}

obj_Boolean Boolean_method_AND(obj_Boolean this, obj_Boolean other) {
  if (this == lit_false) {
    return lit_false;
  }
  if (other == lit_false) {
    return lit_false;
  }
  return lit_true;
}

obj_Boolean Boolean_method_NOT(obj_Boolean this) {
  if (this == lit_true) { return lit_false; }
  return lit_true;
}

/* The Boolean Class (a singleton) */
struct  class_Boolean_struct  the_class_Boolean_struct = {
  new_Boolean,     /* Constructor */
  Boolean_method_STRING,
  Obj_method_EQUAL,
  Obj_method_PRINT,
  Boolean_method_EQUALS,
  Boolean_method_OR,
  Boolean_method_AND,
  Boolean_method_NOT
};

class_Boolean the_class_Boolean = &the_class_Boolean_struct;

/*
 * These are the only two objects of type Boolean that
 * should ever exist. The constructor just picks one of
 * them.
 */
struct obj_Boolean_struct lit_false_struct =
  { &the_class_Boolean_struct, 0 };
obj_Boolean lit_false = &lit_false_struct;
struct obj_Boolean_struct lit_true_struct =
  { &the_class_Boolean_struct, 1 };
obj_Boolean lit_true = &lit_true_struct;

/* ==============
 * Nothing (really just a singleton Obj)
 * Fields: None
 * Methods:
 *    Constructor  (called after allocation)
 *    STRING
 *    PRINT
 *    EQUALS
 *
 * ==============
 */
/*  Constructor */
obj_Nothing new_Nothing(  ) {
  return nothing;
}

/* Boolean:STRING */
obj_String Nothing_method_STRING(obj_Nothing this) {
    return str_literal("<nothing>");
}

/* Inherit Obj:EQUAL, since we have only one
 * object of class None
 */

/* Inherit Obj:PRINT, which will call Nothing:STRING */

/* The Nothing Class (a singleton) */
struct  class_Nothing_struct  the_class_Nothing_struct = {
  new_Nothing,     /* Constructor */
  Nothing_method_STRING,
  Obj_method_EQUAL,
  Obj_method_PRINT
};

class_Nothing the_class_Nothing = &the_class_Nothing_struct;

/*
 * This is the only instance of class Nothing that
 * should ever exist
 */
struct obj_Nothing_struct nothing_struct =
  { &the_class_Nothing_struct };
obj_Nothing nothing = &nothing_struct;

/* ================
 * Int
 * Fields:
 *    One hidden field, an int
 * Methods:
 *    Those of Obj
 *    PLUS
 *    LESS
 *    (add more later)
 * =================
 */

/* Constructor */
obj_Int new_Int() {
  obj_Int new_thing = (obj_Int)
  malloc(sizeof(struct obj_Int_struct));
  new_thing->clazz = the_class_Int;
  new_thing->value = 0;
  return new_thing;
}

/* Int:STRING */
obj_String Int_method_STRING(obj_Int this) {
  char *rep;
  asprintf(&rep, "%d", this->value);
  return str_literal(rep);
}

/* Int:EQUALS */
obj_Boolean Int_method_EQUALS(obj_Int this, obj_Int other) {
  if (this->value != other->value) {
    return lit_false;
  }
  return lit_true;
}

/* Inherit Obj:PRINT, which will call Int:STRING */

/* LESS (new method) */
obj_Boolean Int_method_LESS(obj_Int this, obj_Int other) {
  if (this->value < other->value) {
    return lit_true;
  }
  return lit_false;
}

obj_Boolean Int_method_ATMOST(obj_Int this, obj_Int other) {
  if (this->value <= other->value) {
    return lit_true;
  }
  return lit_false;
}

obj_Boolean Int_method_ATLEAST(obj_Int this, obj_Int other) {
  if (this->value >= other->value) {
    return lit_true;
  }
  return lit_false;
}

obj_Boolean Int_method_MORE(obj_Int this, obj_Int other) {
  if (this->value > other->value) {
    return lit_true;
  }
  return lit_false;
}

/* PLUS (new method) */
obj_Int Int_method_PLUS(obj_Int this, obj_Int other) {
  return int_literal(this->value + other->value);
}

obj_Int Int_method_MINUS(obj_Int this, obj_Int other) {
  return int_literal(this->value - other->value);
}

obj_Int Int_method_TIMES(obj_Int this, obj_Int other) {
  return int_literal(this->value * other->value);
}

obj_Int Int_method_DIVIDE(obj_Int this, obj_Int other) {
  return int_literal(this->value / other->value);
}

/* The Int Class (a singleton) */
struct  class_Int_struct  the_class_Int_struct = {
  new_Int,     /* Constructor */
  Int_method_STRING,
  Obj_method_EQUAL,
  Obj_method_PRINT,
  Int_method_PLUS,
  Int_method_MINUS,
  Int_method_TIMES,
  Int_method_DIVIDE,
  Int_method_ATMOST,
  Int_method_LESS,
  Int_method_ATLEAST,
  Int_method_MORE,
  Int_method_EQUALS
};

class_Int the_class_Int = &the_class_Int_struct;

/* Integer literals constructor,
 * used by compiler and not otherwise available in
 * Quack programs.
 */
obj_Int int_literal(int n) {
  obj_Int boxed = new_Int();
  boxed->value = n;
  return boxed;
}

