static MagickBooleanType IsPCL(const unsigned char *magick,const size_t length)
{
  if (length < 4)
    return(MagickFalse);
  if (memcmp(magick,"\033E\033&",4) == 0)
    return(MagickFalse);
  if (memcmp(magick,"\033E\033",3) == 0)
    return(MagickTrue);
  return(MagickFalse);
}