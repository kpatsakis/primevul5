static GCtrace *trace_save_alloc(jit_State *J)
{
  size_t sztr = ((sizeof(GCtrace)+7)&~7);
  size_t szins = (J->cur.nins-J->cur.nk)*sizeof(IRIns);
  size_t sz = sztr + szins +
	      J->cur.nsnap*sizeof(SnapShot) +
	      J->cur.nsnapmap*sizeof(SnapEntry);
  return lj_mem_newt(J->L, (MSize)sz, GCtrace);
}