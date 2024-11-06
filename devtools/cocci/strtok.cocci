@@
expression d;
@@
- strtok(NULL, d)
+ strtok_r(NULL, d, &sp)

@@
declaration D;
identifier t;
expression s != NULL;
expression d;
@@
D
+ char *sp;
...
- t = strtok(s, d)
+ t = strtok_r(s, d, &sp)

@@
identifier t;
expression s != NULL;
expression d;
@@
- t = strtok(s, d);
+ char *sp;
+ t = strtok_r(s, d, &sp);

@@
type T;
identifier t;
expression s != NULL;
expression d;
@@
- T t = strtok(s, d);
+ char *sp;
+ T t = strtok_r(s, d, &sp);
