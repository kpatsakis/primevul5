ModuleExport size_t RegisterDPSImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("DPS");
#if defined(MAGICKCORE_DPS_DELEGATE)
  entry->decoder=(DecodeImageHandler *) ReadDPSImage;
#endif
  entry->blob_support=MagickFalse;
  entry->description=ConstantString("Display Postscript Interpreter");
  entry->module=ConstantString("DPS");
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}