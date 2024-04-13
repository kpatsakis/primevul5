static void detect_partial_match_to(compiler_common *common, struct sljit_label *label)
{
DEFINE_COMPILER;

CMPTO(SLJIT_LESS, STR_PTR, 0, STR_END, 0, label);
process_partial_match(common);
}