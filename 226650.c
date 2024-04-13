const char *lj_debug_uvnamev(cTValue *o, uint32_t idx, TValue **tvp, GCobj **op)
{
  if (tvisfunc(o)) {
    GCfunc *fn = funcV(o);
    if (isluafunc(fn)) {
      GCproto *pt = funcproto(fn);
      if (idx < pt->sizeuv) {
	GCobj *uvo = gcref(fn->l.uvptr[idx]);
	*tvp = uvval(&uvo->uv);
	*op = uvo;
	return lj_debug_uvname(pt, idx);
      }
    } else {
      if (idx < fn->c.nupvalues) {
	*tvp = &fn->c.upvalue[idx];
	*op = obj2gco(fn);
	return "";
      }
    }
  }
  return NULL;
}