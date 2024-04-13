ModuleExport size_t RegisterWEBPImage(void)
{
  char
    version[MagickPathExtent];

  MagickInfo
    *entry;

  *version='\0';
  entry=SetMagickInfo("WEBP");
#if defined(MAGICKCORE_WEBP_DELEGATE)
  entry->decoder=(DecodeImageHandler *) ReadWEBPImage;
  entry->encoder=(EncodeImageHandler *) WriteWEBPImage;
  (void) FormatLocaleString(version,MagickPathExtent,"libwebp %d.%d.%d [%04X]",
    (WebPGetEncoderVersion() >> 16) & 0xff,
    (WebPGetEncoderVersion() >> 8) & 0xff,
    (WebPGetEncoderVersion() >> 0) & 0xff,WEBP_ENCODER_ABI_VERSION);
#endif
  entry->description=ConstantString("WebP Image Format");
  entry->mime_type=ConstantString("image/webp");
  entry->seekable_stream=MagickTrue;
#if !defined(MAGICKCORE_WEBPMUX_DELEGATE)
  entry->adjoin=MagickFalse;
#endif
  entry->magick_module=ConstantString("WEBP");
  entry->magick=(IsImageFormatHandler *) IsWEBP;
  if (*version != '\0')
    entry->version=ConstantString(version);
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}