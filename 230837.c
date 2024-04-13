static sljit_s32 character_to_int32(PCRE2_UCHAR chr)
{
sljit_s32 value = (sljit_s32)chr;
#if PCRE2_CODE_UNIT_WIDTH == 8
#define SSE2_COMPARE_TYPE_INDEX 0
return (value << 24) | (value << 16) | (value << 8) | value;
#elif PCRE2_CODE_UNIT_WIDTH == 16
#define SSE2_COMPARE_TYPE_INDEX 1
return (value << 16) | value;
#elif PCRE2_CODE_UNIT_WIDTH == 32
#define SSE2_COMPARE_TYPE_INDEX 2
return value;
#else
#error "Unsupported unit width"
#endif
}