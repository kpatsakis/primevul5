static void load_from_mem_sse2(struct sljit_compiler *compiler, sljit_s32 dst_xmm_reg, sljit_s32 src_general_reg)
{
#if (defined SLJIT_CONFIG_X86_64 && SLJIT_CONFIG_X86_64)
sljit_u8 instruction[5];
#else
sljit_u8 instruction[4];
#endif

SLJIT_ASSERT(dst_xmm_reg < 8);

/* MOVDQA xmm1, xmm2/m128 */
#if (defined SLJIT_CONFIG_X86_64 && SLJIT_CONFIG_X86_64)
if (src_general_reg < 8)
  {
  instruction[0] = 0x66;
  instruction[1] = 0x0f;
  instruction[2] = 0x6f;
  instruction[3] = (dst_xmm_reg << 3) | src_general_reg;
  sljit_emit_op_custom(compiler, instruction, 4);
  }
else
  {
  instruction[0] = 0x66;
  instruction[1] = 0x41;
  instruction[2] = 0x0f;
  instruction[3] = 0x6f;
  instruction[4] = (dst_xmm_reg << 3) | (src_general_reg & 0x7);
  sljit_emit_op_custom(compiler, instruction, 4);
  }
#else
instruction[0] = 0x66;
instruction[1] = 0x0f;
instruction[2] = 0x6f;
instruction[3] = (dst_xmm_reg << 3) | src_general_reg;
sljit_emit_op_custom(compiler, instruction, 4);
#endif
}