void simplestring_free(simplestring* string) {
   if(string && string->str) {
      my_free(string->str);
      string->len = 0;
   }
}