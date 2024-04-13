LUA_API int lua_setcstacklimit (lua_State *L, unsigned int limit) {
  global_State *g = G(L);
  int ccalls;
  luaE_freeCI(L);  /* release unused CIs */
  ccalls = getCcalls(L);
  if (limit >= 40000)
    return 0;  /* out of bounds */
  limit += CSTACKERR;
  if (L != g-> mainthread)
    return 0;  /* only main thread can change the C stack */
  else if (ccalls <= CSTACKERR)
    return 0;  /* handling overflow */
  else {
    int diff = limit - g->Cstacklimit;
    if (ccalls + diff <= CSTACKERR)
      return 0;  /* new limit would cause an overflow */
    g->Cstacklimit = limit;  /* set new limit */
    L->nCcalls += diff;  /* correct 'nCcalls' */
    return limit - diff - CSTACKERR;  /* success; return previous limit */
  }
}