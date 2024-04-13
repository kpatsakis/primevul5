utf16le_code_to_mbclen(OnigCodePoint code,
		       OnigEncoding enc ARG_UNUSED)
{
  return (code > 0xffff ? 4 : 2);
}