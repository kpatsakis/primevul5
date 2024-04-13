void simplestring_add(simplestring* target, const char* source) {
   if(target && source) {
      simplestring_addn(target, source, strlen(source));
   }
}