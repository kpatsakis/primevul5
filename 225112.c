ModuleExport size_t RegisterHEICImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("HEIC");
#if defined(MAGICKCORE_HEIC_DELEGATE)
  entry->decoder=(DecodeImageHandler *) ReadHEICImage;
#if !defined(MAGICKCORE_WINDOWS_SUPPORT)
  entry->encoder=(EncodeImageHandler *) WriteHEICImage;
#endif
#endif
  entry->magick=(IsImageFormatHandler *) IsHEIC;
  entry->description=ConstantString("Apple High efficiency Image Format");
  entry->mime_type=ConstantString("image/x-heic");
#if defined(LIBHEIF_VERSION)
  entry->version=ConstantString(LIBHEIF_VERSION);
#endif
  entry->magick_module=ConstantString("HEIC");
  entry->adjoin=MagickFalse;
  entry->seekable_stream=MagickTrue;
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}