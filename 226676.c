static const char *debug_varname(const GCproto *pt, BCPos pc, BCReg slot)
{
  const uint8_t *p = proto_varinfo(pt);
  if (p) {
    BCPos lastpc = 0;
    for (;;) {
      const char *name = (const char *)p;
      uint32_t vn = *p++;
      BCPos startpc, endpc;
      if (vn < VARNAME__MAX) {
	if (vn == VARNAME_END) break;  /* End of varinfo. */
      } else {
	while (*p++) ;  /* Skip over variable name string. */
      }
      lastpc = startpc = lastpc + debug_read_uleb128(&p);
      if (startpc > pc) break;
      endpc = startpc + debug_read_uleb128(&p);
      if (pc < endpc && slot-- == 0) {
	if (vn < VARNAME__MAX) {
#define VARNAMESTR(name, str)	str "\0"
	  name = VARNAMEDEF(VARNAMESTR);
#undef VARNAMESTR
	  if (--vn) while (*name++ || --vn) ;
	}
	return name;
      }
    }
  }
  return NULL;
}