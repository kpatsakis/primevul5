int LJ_FASTCALL lj_trace_exit(jit_State *J, void *exptr)
{
  ERRNO_SAVE
  lua_State *L = J->L;
  ExitState *ex = (ExitState *)exptr;
  ExitDataCP exd;
  int errcode;
  const BCIns *pc;
  void *cf;
  GCtrace *T;
#ifdef EXITSTATE_PCREG
  J->parent = trace_exit_find(J, (MCode *)(intptr_t)ex->gpr[EXITSTATE_PCREG]);
#endif
  T = traceref(J, J->parent); UNUSED(T);
#ifdef EXITSTATE_CHECKEXIT
  if (J->exitno == T->nsnap) {  /* Treat stack check like a parent exit. */
    lua_assert(T->root != 0);
    J->exitno = T->ir[REF_BASE].op2;
    J->parent = T->ir[REF_BASE].op1;
    T = traceref(J, J->parent);
  }
#endif
  lua_assert(T != NULL && J->exitno < T->nsnap);
  exd.J = J;
  exd.exptr = exptr;
  errcode = lj_vm_cpcall(L, NULL, &exd, trace_exit_cp);
  if (errcode)
    return -errcode;  /* Return negated error code. */

  lj_vmevent_send(L, TEXIT,
    lj_state_checkstack(L, 4+RID_NUM_GPR+RID_NUM_FPR+LUA_MINSTACK);
    setintV(L->top++, J->parent);
    setintV(L->top++, J->exitno);
    trace_exit_regs(L, ex);
  );

  pc = exd.pc;
  cf = cframe_raw(L->cframe);
  setcframe_pc(cf, pc);
  if (G(L)->gc.state == GCSatomic || G(L)->gc.state == GCSfinalize) {
    if (!(G(L)->hookmask & HOOK_GC))
      lj_gc_step(L);  /* Exited because of GC: drive GC forward. */
  } else {
    trace_hotside(J, pc);
  }
  if (bc_op(*pc) == BC_JLOOP) {
    BCIns *retpc = &traceref(J, bc_d(*pc))->startins;
    if (bc_isret(bc_op(*retpc))) {
      if (J->state == LJ_TRACE_RECORD) {
	J->patchins = *pc;
	J->patchpc = (BCIns *)pc;
	*J->patchpc = *retpc;
	J->bcskip = 1;
      } else {
	pc = retpc;
	setcframe_pc(cf, pc);
      }
    }
  }
  /* Return MULTRES or 0. */
  ERRNO_RESTORE
  switch (bc_op(*pc)) {
  case BC_CALLM: case BC_CALLMT:
    return (int)((BCReg)(L->top - L->base) - bc_a(*pc) - bc_c(*pc));
  case BC_RETM:
    return (int)((BCReg)(L->top - L->base) + 1 - bc_a(*pc) - bc_d(*pc));
  case BC_TSETM:
    return (int)((BCReg)(L->top - L->base) + 1 - bc_a(*pc));
  default:
    if (bc_op(*pc) >= BC_FUNCF)
      return (int)((BCReg)(L->top - L->base) + 1);
    return 0;
  }
}