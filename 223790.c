static int FillBasicWEBPInfo(Image *image,const uint8_t *stream,size_t length,
  WebPDecoderConfig *configure)
{
  WebPBitstreamFeatures
    *magick_restrict features = &configure->input;

  int
    webp_status;

  webp_status=WebPGetFeatures(stream,length,features);

  if (webp_status != VP8_STATUS_OK)
    return(webp_status);

  image->columns=(size_t) features->width;
  image->rows=(size_t) features->height;
  image->depth=8;
  image->matte=features->has_alpha != 0 ? MagickTrue : MagickFalse;

  return(webp_status);
}