static struct sljit_jump *jump_if_utf_char_start(struct sljit_compiler *compiler, sljit_s32 reg)
{
#if PCRE2_CODE_UNIT_WIDTH == 8
OP2(SLJIT_AND, reg, 0, reg, 0, SLJIT_IMM, 0xc0);
return CMP(SLJIT_NOT_EQUAL, reg, 0, SLJIT_IMM, 0x80);
#elif PCRE2_CODE_UNIT_WIDTH == 16
OP2(SLJIT_AND, reg, 0, reg, 0, SLJIT_IMM, 0xfc00);
return CMP(SLJIT_NOT_EQUAL, reg, 0, SLJIT_IMM, 0xdc00);
#else
#error "Unknown code width"
#endif
}