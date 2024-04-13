add_compile_string_length(UChar* s ARG_UNUSED, int mb_len, OnigDistance str_len,
                          regex_t* reg ARG_UNUSED, int ignore_case)
{
  int len;
  int op = select_str_opcode(mb_len, str_len, ignore_case);

  len = SIZE_OPCODE;

  if (op == OP_EXACTMBN)  len += SIZE_LENGTH;
  if (IS_NEED_STR_LEN_OP_EXACT(op))
    len += SIZE_LENGTH;

  len += mb_len * str_len;
  return len;
}