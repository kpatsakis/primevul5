ModuleExport size_t RegisterVIFFImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("VIFF");
  entry->decoder=(DecodeImageHandler *) ReadVIFFImage;
  entry->encoder=(EncodeImageHandler *) WriteVIFFImage;
  entry->magick=(IsImageFormatHandler *) IsVIFF;
  entry->seekable_stream=MagickTrue;
  entry->description=ConstantString("Khoros Visualization image");
  entry->module=ConstantString("VIFF");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("XV");
  entry->decoder=(DecodeImageHandler *) ReadVIFFImage;
  entry->encoder=(EncodeImageHandler *) WriteVIFFImage;
  entry->seekable_stream=MagickTrue;
  entry->description=ConstantString("Khoros Visualization image");
  entry->module=ConstantString("VIFF");
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}