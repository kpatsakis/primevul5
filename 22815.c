utf16be_code_to_mbclen(OnigCodePoint code,
		       OnigEncoding enc)
{
  return (code > 0xffff ? 4 : 2);
}