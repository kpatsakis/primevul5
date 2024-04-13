static void close_state (lua_State *L) {
  global_State *g = G(L);
  luaF_close(L, L->stack, CLOSEPROTECT);  /* close all upvalues */
  luaC_freeallobjects(L);  /* collect all objects */
  if (ttisnil(&g->nilvalue))  /* closing a fully built state? */
    luai_userstateclose(L);
  luaM_freearray(L, G(L)->strt.hash, G(L)->strt.size);
  freestack(L);
  lua_assert(gettotalbytes(g) == sizeof(LG));
  (*g->frealloc)(g->ud, fromstate(L), sizeof(LG), 0);  /* free main block */
}