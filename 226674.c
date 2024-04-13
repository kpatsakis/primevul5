const char *lj_debug_uvname(GCproto *pt, uint32_t idx)
{
  const uint8_t *p = proto_uvinfo(pt);
  lua_assert(idx < pt->sizeuv);
  if (!p) return "";
  if (idx) while (*p++ || --idx) ;
  return (const char *)p;
}