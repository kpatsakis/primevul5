static void jumpto_if_not_utf_char_start(struct sljit_compiler *compiler, sljit_s32 reg, struct sljit_label *label)
{
#if PCRE2_CODE_UNIT_WIDTH == 8
OP2(SLJIT_AND, reg, 0, reg, 0, SLJIT_IMM, 0xc0);
CMPTO(SLJIT_EQUAL, reg, 0, SLJIT_IMM, 0x80, label);
#elif PCRE2_CODE_UNIT_WIDTH == 16
OP2(SLJIT_AND, reg, 0, reg, 0, SLJIT_IMM, 0xfc00);
CMPTO(SLJIT_EQUAL, reg, 0, SLJIT_IMM, 0xdc00, label);
#else
#error "Unknown code width"
#endif
}