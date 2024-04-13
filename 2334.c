static void delayed_mem_copy_finish(delayed_mem_copy_status *status)
{
struct sljit_compiler *compiler = status->compiler;
int next_tmp_reg = status->next_tmp_reg;
int tmp_reg, saved_tmp_reg, i;

for (i = 0; i < RECURSE_TMP_REG_COUNT; i++)
  {
  if (status->store_bases[next_tmp_reg] != -1)
    {
    tmp_reg = status->tmp_regs[next_tmp_reg];
    saved_tmp_reg = status->saved_tmp_regs[next_tmp_reg];

    OP1(SLJIT_MOV, SLJIT_MEM1(status->store_bases[next_tmp_reg]), status->store_offsets[next_tmp_reg], tmp_reg, 0);

    /* Restore virtual registers. */
    if (sljit_get_register_index(saved_tmp_reg) < 0)
      OP1(SLJIT_MOV, tmp_reg, 0, saved_tmp_reg, 0);
    }

  next_tmp_reg = (next_tmp_reg + 1) % RECURSE_TMP_REG_COUNT;
  }
}