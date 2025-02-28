void simplestring_addn(simplestring* target, const char* source, size_t add_len) {
   size_t newsize = target->size, incr = 0;
   if(target && source) {
      if(!target->str) {
         simplestring_init_str(target);
      }

      if((SIZE_MAX - add_len) < target->len || (SIZE_MAX - add_len - 1) < target->len) {
    	  /* check for overflows, if there's a potential overflow do nothing */
    	  return;
      }

      if(target->len + add_len + 1 > target->size) {
         /* newsize is current length + new length */
         newsize = target->len + add_len + 1;
         incr = target->size * 2;

         /* align to SIMPLESTRING_INCR increments */
         if (incr) {
            newsize = newsize - (newsize % incr) + incr;
         }
         if(newsize < (target->len + add_len + 1)) {
        	 /* some kind of overflow happened */
        	 return;
         }
         target->str = (char*)realloc(target->str, newsize);

         target->size = target->str ? newsize : 0;
      }

      if(target->str) {
         if(add_len) {
            memcpy(target->str + target->len, source, add_len);
         }
         target->len += add_len;
         target->str[target->len] = 0; /* null terminate */
      }
   }
}