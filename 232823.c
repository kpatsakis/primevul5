void luaE_freeCI (lua_State *L) {
  CallInfo *ci = L->ci;
  CallInfo *next = ci->next;
  ci->next = NULL;
  L->nCcalls += L->nci;  /* add removed elements back to 'nCcalls' */
  while ((ci = next) != NULL) {
    next = ci->next;
    luaM_free(L, ci);
    L->nci--;
  }
  L->nCcalls -= L->nci;  /* adjust result */
}