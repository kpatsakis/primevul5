ModuleExport size_t RegisterPCLImage(void)
{
  MagickInfo
    *entry;

  entry=AcquireMagickInfo("PCL","PCL","Printer Control Language");
  entry->decoder=(DecodeImageHandler *) ReadPCLImage;
  entry->encoder=(EncodeImageHandler *) WritePCLImage;
  entry->magick=(IsImageFormatHandler *) IsPCL;
  entry->flags^=CoderBlobSupportFlag;
  entry->flags^=CoderDecoderThreadSupportFlag;
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}