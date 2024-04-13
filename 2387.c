static void delayed_mem_copy_move(delayed_mem_copy_status *status, int load_base, sljit_sw load_offset,
  int store_base, sljit_sw store_offset)
{
struct sljit_compiler *compiler = status->compiler;
int next_tmp_reg = status->next_tmp_reg;
int tmp_reg = status->tmp_regs[next_tmp_reg];

SLJIT_ASSERT(load_base > 0 && store_base > 0);

if (status->store_bases[next_tmp_reg] == -1)
  {
  /* Preserve virtual registers. */
  if (sljit_get_register_index(status->saved_tmp_regs[next_tmp_reg]) < 0)
    OP1(SLJIT_MOV, status->saved_tmp_regs[next_tmp_reg], 0, tmp_reg, 0);
  }
else
  OP1(SLJIT_MOV, SLJIT_MEM1(status->store_bases[next_tmp_reg]), status->store_offsets[next_tmp_reg], tmp_reg, 0);

OP1(SLJIT_MOV, tmp_reg, 0, SLJIT_MEM1(load_base), load_offset);
status->store_bases[next_tmp_reg] = store_base;
status->store_offsets[next_tmp_reg] = store_offset;

status->next_tmp_reg = (next_tmp_reg + 1) % RECURSE_TMP_REG_COUNT;
}