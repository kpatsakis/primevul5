static SLJIT_INLINE unsigned int char_othercase(compiler_common *common, unsigned int c)
{
/* Returns with the othercase. */
#ifdef SUPPORT_UNICODE
if (common->utf && c > 127)
  {
  return UCD_OTHERCASE(c);
  }
#endif
return TABLE_GET(c, common->fcc, c);
}