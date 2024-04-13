ModuleExport size_t RegisterPS3Image(void)
{
  MagickInfo
    *entry;

  entry=AcquireMagickInfo("PS3","EPS3","Level III Encapsulated PostScript");
  entry->encoder=(EncodeImageHandler *) WritePS3Image;
  entry->mime_type=ConstantString("application/postscript");
  entry->flags|=CoderEncoderSeekableStreamFlag;
  entry->flags^=CoderBlobSupportFlag;
  (void) RegisterMagickInfo(entry);
  entry=AcquireMagickInfo("PS3","PS3","Level III PostScript");
  entry->encoder=(EncodeImageHandler *) WritePS3Image;
  entry->mime_type=ConstantString("application/postscript");
  entry->flags|=CoderEncoderSeekableStreamFlag;
  entry->flags^=CoderBlobSupportFlag;
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}