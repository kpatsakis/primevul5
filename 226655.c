static void trace_exit_regs(lua_State *L, ExitState *ex)
{
  int32_t i;
  setintV(L->top++, RID_NUM_GPR);
  setintV(L->top++, RID_NUM_FPR);
  for (i = 0; i < RID_NUM_GPR; i++) {
    if (sizeof(ex->gpr[i]) == sizeof(int32_t))
      setintV(L->top++, (int32_t)ex->gpr[i]);
    else
      setnumV(L->top++, (lua_Number)ex->gpr[i]);
  }
#if !LJ_SOFTFP
  for (i = 0; i < RID_NUM_FPR; i++) {
    setnumV(L->top, ex->fpr[i]);
    if (LJ_UNLIKELY(tvisnan(L->top)))
      setnanV(L->top);
    L->top++;
  }
#endif
}