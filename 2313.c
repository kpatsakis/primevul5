static BOOL optimize_class(compiler_common *common, const sljit_u8 *bits, BOOL nclass, BOOL invert, jump_list **backtracks)
{
/* May destroy TMP1. */
if (optimize_class_ranges(common, bits, nclass, invert, backtracks))
  return TRUE;
return optimize_class_chars(common, bits, nclass, invert, backtracks);
}