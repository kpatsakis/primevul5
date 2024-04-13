static void fast_forward_char_pair_sse2_compare(struct sljit_compiler *compiler, PCRE2_UCHAR char1, PCRE2_UCHAR char2,
  sljit_u32 bit, sljit_s32 dst_ind, sljit_s32 cmp1_ind, sljit_s32 cmp2_ind, sljit_s32 tmp_ind)
{
sljit_u8 instruction[4];
instruction[0] = 0x66;
instruction[1] = 0x0f;

if (char1 == char2 || bit != 0)
  {
  if (bit != 0)
    {
    /* POR xmm1, xmm2/m128 */
    /* instruction[0] = 0x66; */
    /* instruction[1] = 0x0f; */
    instruction[2] = 0xeb;
    instruction[3] = 0xc0 | (dst_ind << 3) | cmp2_ind;
    sljit_emit_op_custom(compiler, instruction, 4);
    }

  /* PCMPEQB/W/D xmm1, xmm2/m128 */
  /* instruction[0] = 0x66; */
  /* instruction[1] = 0x0f; */
  instruction[2] = 0x74 + SSE2_COMPARE_TYPE_INDEX;
  instruction[3] = 0xc0 | (dst_ind << 3) | cmp1_ind;
  sljit_emit_op_custom(compiler, instruction, 4);
  }
else
  {
  /* MOVDQA xmm1, xmm2/m128 */
  /* instruction[0] = 0x66; */
  /* instruction[1] = 0x0f; */
  instruction[2] = 0x6f;
  instruction[3] = 0xc0 | (tmp_ind << 3) | dst_ind;
  sljit_emit_op_custom(compiler, instruction, 4);

  /* PCMPEQB/W/D xmm1, xmm2/m128 */
  /* instruction[0] = 0x66; */
  /* instruction[1] = 0x0f; */
  instruction[2] = 0x74 + SSE2_COMPARE_TYPE_INDEX;
  instruction[3] = 0xc0 | (dst_ind << 3) | cmp1_ind;
  sljit_emit_op_custom(compiler, instruction, 4);

  instruction[3] = 0xc0 | (tmp_ind << 3) | cmp2_ind;
  sljit_emit_op_custom(compiler, instruction, 4);

  /* POR xmm1, xmm2/m128 */
  /* instruction[0] = 0x66; */
  /* instruction[1] = 0x0f; */
  instruction[2] = 0xeb;
  instruction[3] = 0xc0 | (dst_ind << 3) | tmp_ind;
  sljit_emit_op_custom(compiler, instruction, 4);
  }
}