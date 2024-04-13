void simplestring_clear(simplestring* string) {
   if(string->str) {
      string->str[0] = 0;
   }
   string->len = 0;
}