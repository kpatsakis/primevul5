LUA_API int lua_getinfo(lua_State *L, const char *what, lua_Debug *ar)
{
  return lj_debug_getinfo(L, what, (lj_Debug *)ar, 0);
}