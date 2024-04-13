void lj_debug_addloc(lua_State *L, const char *msg,
		     cTValue *frame, cTValue *nextframe)
{
  if (frame) {
    GCfunc *fn = frame_func(frame);
    if (isluafunc(fn)) {
      BCLine line = debug_frameline(L, fn, nextframe);
      if (line >= 0) {
	char buf[LUA_IDSIZE];
	lj_debug_shortname(buf, proto_chunkname(funcproto(fn)));
	lj_str_pushf(L, "%s:%d: %s", buf, line, msg);
	return;
      }
    }
  }
  lj_str_pushf(L, "%s", msg);
}