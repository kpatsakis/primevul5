LUA_API int lua_getstack(lua_State *L, int level, lua_Debug *ar)
{
  int size;
  cTValue *frame = lj_debug_frame(L, level, &size);
  if (frame) {
    ar->i_ci = (size << 16) + (int)(frame - tvref(L->stack));
    return 1;
  } else {
    ar->i_ci = level - size;
    return 0;
  }
}