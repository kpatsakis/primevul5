static void trace_unpatch(jit_State *J, GCtrace *T)
{
  BCOp op = bc_op(T->startins);
  BCIns *pc = mref(T->startpc, BCIns);
  UNUSED(J);
  if (op == BC_JMP)
    return;  /* No need to unpatch branches in parent traces (yet). */
  switch (bc_op(*pc)) {
  case BC_JFORL:
    lua_assert(traceref(J, bc_d(*pc)) == T);
    *pc = T->startins;
    pc += bc_j(T->startins);
    lua_assert(bc_op(*pc) == BC_JFORI);
    setbc_op(pc, BC_FORI);
    break;
  case BC_JITERL:
  case BC_JLOOP:
    lua_assert(op == BC_ITERL || op == BC_LOOP || bc_isret(op));
    *pc = T->startins;
    break;
  case BC_JMP:
    lua_assert(op == BC_ITERL);
    pc += bc_j(*pc)+2;
    if (bc_op(*pc) == BC_JITERL) {
      lua_assert(traceref(J, bc_d(*pc)) == T);
      *pc = T->startins;
    }
    break;
  case BC_JFUNCF:
    lua_assert(op == BC_FUNCF);
    *pc = T->startins;
    break;
  default:  /* Already unpatched. */
    break;
  }
}