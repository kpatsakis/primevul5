ModuleExport size_t RegisterHEICImage(void)
{
  MagickInfo
    *entry;

  entry=AcquireMagickInfo("HEIC","HEIC","High Efficiency Image Format");
#if defined(MAGICKCORE_HEIC_DELEGATE)
  entry->decoder=(DecodeImageHandler *) ReadHEICImage;
#if !defined(MAGICKCORE_WINDOWS_SUPPORT)
  entry->encoder=(EncodeImageHandler *) WriteHEICImage;
#endif
#endif
  entry->magick=(IsImageFormatHandler *) IsHEIC;
  entry->mime_type=ConstantString("image/x-heic");
#if defined(LIBHEIF_VERSION)
  entry->version=ConstantString(LIBHEIF_VERSION);
#endif
  entry->flags|=CoderDecoderSeekableStreamFlag;
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}