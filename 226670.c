LUA_API const char *lua_getlocal(lua_State *L, const lua_Debug *ar, int n)
{
  const char *name = NULL;
  if (ar) {
    TValue *o = debug_localname(L, ar, &name, (BCReg)n);
    if (name) {
      copyTV(L, L->top, o);
      incr_top(L);
    }
  } else if (tvisfunc(L->top-1) && isluafunc(funcV(L->top-1))) {
    name = debug_varname(funcproto(funcV(L->top-1)), 0, (BCReg)n-1);
  }
  return name;
}