static SLJIT_INLINE sljit_u32 max_fast_forward_char_pair_sse2_offset(void)
{
#if PCRE2_CODE_UNIT_WIDTH == 8
return 15;
#elif PCRE2_CODE_UNIT_WIDTH == 16
return 7;
#elif PCRE2_CODE_UNIT_WIDTH == 32
return 3;
#else
#error "Unsupported unit width"
#endif
}