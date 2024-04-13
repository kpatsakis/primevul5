static MagickBooleanType IsHEIC(const unsigned char *magick,const size_t length)
{
  if (length < 12)
    return(MagickFalse);
  if (LocaleNCompare((const char *) magick+4,"ftyp",4) != 0)
  return(MagickFalse);
  if (LocaleNCompare((const char *) magick+8,"heic",4) == 0)
    return(MagickTrue);
  if (LocaleNCompare((const char *) magick+8,"heix",4) == 0)
    return(MagickTrue);
  if (LocaleNCompare((const char *) magick+8,"mif1",4) == 0)
    return(MagickTrue);
  return(MagickFalse);
}