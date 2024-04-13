void lj_debug_pushloc(lua_State *L, GCproto *pt, BCPos pc)
{
  GCstr *name = proto_chunkname(pt);
  const char *s = strdata(name);
  MSize i, len = name->len;
  BCLine line = lj_debug_line(pt, pc);
  if (*s == '@') {
    s++; len--;
    for (i = len; i > 0; i--)
      if (s[i] == '/' || s[i] == '\\') {
	s += i+1;
	break;
      }
    lj_str_pushf(L, "%s:%d", s, line);
  } else if (len > 40) {
    lj_str_pushf(L, "%p:%d", pt, line);
  } else if (*s == '=') {
    lj_str_pushf(L, "%s:%d", s+1, line);
  } else {
    lj_str_pushf(L, "\"%s\":%d", s, line);
  }
}