static void simplestring_init_str(simplestring* string) {
   string->str = (char*)malloc(SIMPLESTRING_INCR);
   if(string->str) {
      string->str[0] = 0;
      string->len = 0;
      string->size = SIMPLESTRING_INCR;
   }
   else {
      string->size = 0;
   }
}