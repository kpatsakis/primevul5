static void delayed_mem_copy_init(delayed_mem_copy_status *status, compiler_common *common)
{
int i;

for (i = 0; i < RECURSE_TMP_REG_COUNT; i++)
  {
  SLJIT_ASSERT(status->tmp_regs[i] >= 0);
  SLJIT_ASSERT(sljit_get_register_index(status->saved_tmp_regs[i]) < 0 || status->tmp_regs[i] == status->saved_tmp_regs[i]);

  status->store_bases[i] = -1;
  }
status->next_tmp_reg = 0;
status->compiler = common->compiler;
}