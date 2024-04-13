static int trace_abort(jit_State *J)
{
  lua_State *L = J->L;
  TraceError e = LJ_TRERR_RECERR;
  TraceNo traceno;

  J->postproc = LJ_POST_NONE;
  lj_mcode_abort(J);
  if (tvisnumber(L->top-1))
    e = (TraceError)numberVint(L->top-1);
  if (e == LJ_TRERR_MCODELM) {
    L->top--;  /* Remove error object */
    J->state = LJ_TRACE_ASM;
    return 1;  /* Retry ASM with new MCode area. */
  }
  /* Penalize or blacklist starting bytecode instruction. */
  if (J->parent == 0 && !bc_isret(bc_op(J->cur.startins)))
    penalty_pc(J, &gcref(J->cur.startpt)->pt, mref(J->cur.startpc, BCIns), e);

  /* Is there anything to abort? */
  traceno = J->cur.traceno;
  if (traceno) {
    ptrdiff_t errobj = savestack(L, L->top-1);  /* Stack may be resized. */
    J->cur.link = 0;
    J->cur.linktype = LJ_TRLINK_NONE;
    lj_vmevent_send(L, TRACE,
      TValue *frame;
      const BCIns *pc;
      GCfunc *fn;
      setstrV(L, L->top++, lj_str_newlit(L, "abort"));
      setintV(L->top++, traceno);
      /* Find original Lua function call to generate a better error message. */
      frame = J->L->base-1;
      pc = J->pc;
      while (!isluafunc(frame_func(frame))) {
	pc = (frame_iscont(frame) ? frame_contpc(frame) : frame_pc(frame)) - 1;
	frame = frame_prev(frame);
      }
      fn = frame_func(frame);
      setfuncV(L, L->top++, fn);
      setintV(L->top++, proto_bcpos(funcproto(fn), pc));
      copyTV(L, L->top++, restorestack(L, errobj));
      copyTV(L, L->top++, &J->errinfo);
    );
    /* Drop aborted trace after the vmevent (which may still access it). */
    setgcrefnull(J->trace[traceno]);
    if (traceno < J->freetrace)
      J->freetrace = traceno;
    J->cur.traceno = 0;
  }
  L->top--;  /* Remove error object */
  if (e == LJ_TRERR_DOWNREC)
    return trace_downrec(J);
  else if (e == LJ_TRERR_MCODEAL)
    lj_trace_flushall(L);
  return 0;
}