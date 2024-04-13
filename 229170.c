ModuleExport size_t RegisterXPMImage(void)
{
  MagickInfo
    *entry;

  if (xpm_symbolic == (SplayTreeInfo *) NULL)
    xpm_symbolic=NewSplayTree(CompareSplayTreeString,RelinquishMagickMemory,
      RelinquishMagickMemory);
  entry=SetMagickInfo("PICON");
  entry->decoder=(DecodeImageHandler *) ReadXPMImage;
  entry->encoder=(EncodeImageHandler *) WritePICONImage;
  entry->adjoin=MagickFalse;
  entry->description=ConstantString("Personal Icon");
  entry->magick_module=ConstantString("XPM");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("PM");
  entry->decoder=(DecodeImageHandler *) ReadXPMImage;
  entry->encoder=(EncodeImageHandler *) WriteXPMImage;
  entry->adjoin=MagickFalse;
  entry->stealth=MagickTrue;
  entry->description=ConstantString("X Windows system pixmap (color)");
  entry->magick_module=ConstantString("XPM");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("XPM");
  entry->decoder=(DecodeImageHandler *) ReadXPMImage;
  entry->encoder=(EncodeImageHandler *) WriteXPMImage;
  entry->magick=(IsImageFormatHandler *) IsXPM;
  entry->adjoin=MagickFalse;
  entry->description=ConstantString("X Windows system pixmap (color)");
  entry->magick_module=ConstantString("XPM");
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}