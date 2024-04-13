static StkId rethook (lua_State *L, CallInfo *ci, StkId firstres, int nres) {
  ptrdiff_t oldtop = savestack(L, L->top);  /* hook may change top */
  int delta = 0;
  if (isLuacode(ci)) {
    Proto *p = ci_func(ci)->p;
    if (p->is_vararg)
      delta = ci->u.l.nextraargs + p->numparams + 1;
    if (L->top < ci->top)
      L->top = ci->top;  /* correct top to run hook */
  }
  if (L->hookmask & LUA_MASKRET) {  /* is return hook on? */
    int ftransfer;
    ci->func += delta;  /* if vararg, back to virtual 'func' */
    ftransfer = cast(unsigned short, firstres - ci->func);
    luaD_hook(L, LUA_HOOKRET, -1, ftransfer, nres);  /* call it */
    ci->func -= delta;
  }
  if (isLua(ci = ci->previous))
    L->oldpc = pcRel(ci->u.l.savedpc, ci_func(ci)->p);  /* update 'oldpc' */
  return restorestack(L, oldtop);
}