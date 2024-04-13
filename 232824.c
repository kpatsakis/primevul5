void luaE_enterCcall (lua_State *L) {
  int ncalls = getCcalls(L);
  L->nCcalls--;
  if (ncalls <= CSTACKERR) {  /* possible overflow? */
    luaE_freeCI(L);  /* release unused CIs */
    ncalls = getCcalls(L);  /* update call count */
    if (ncalls <= CSTACKERR) {  /* still overflow? */
      if (ncalls <= CSTACKERRMARK)  /* below error-handling zone? */
        luaD_throw(L, LUA_ERRERR);  /* error while handling stack error */
      else if (ncalls >= CSTACKMARK) {
        /* not in error-handling zone; raise the error now */
        L->nCcalls = (CSTACKMARK - 1);  /* enter error-handling zone */
        luaG_runerror(L, "C stack overflow");
      }
      /* else stack is in the error-handling zone;
         allow message handler to work */
    }
  }
}