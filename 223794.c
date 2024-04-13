static int ReadSingleWEBPImage(Image *image,const uint8_t *stream,
  size_t length,WebPDecoderConfig *configure,ExceptionInfo *exception,
  MagickBooleanType is_first)
{
  int
    webp_status;

  unsigned char
    *p;

  size_t
    canvas_width,
    canvas_height,
    image_width,
    image_height;

  ssize_t
    x_offset,
    y_offset,
    y;

  WebPDecBuffer
    *magick_restrict webp_image = &configure->output;

  MagickBooleanType
    status;

  if (is_first)
    {
      canvas_width=image->columns;
      canvas_height=image->rows;
      x_offset=image->page.x;
      y_offset=image->page.y;
      image->page.x=0;
      image->page.y=0;
    }
  else
    {
      x_offset=0;
      y_offset=0;
    }
  webp_status=FillBasicWEBPInfo(image,stream,length,configure);
  image_width=image->columns;
  image_height=image->rows;
  if (is_first)
    {
      image->columns=canvas_width;
      image->rows=canvas_height;
    }

  if (webp_status != VP8_STATUS_OK)
    return(webp_status);

  if (IsWEBPImageLossless(stream,length) != MagickFalse)
    image->quality=100;

  webp_status=WebPDecode(stream,length,configure);
  if (webp_status != VP8_STATUS_OK)
    return(webp_status);

  p=(unsigned char *) webp_image->u.RGBA.rgba;
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    PixelPacket
      *q;

    ssize_t
      x;

    q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
    if (q == (PixelPacket *) NULL)
      break;
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      if ((x >= x_offset && x < (ssize_t) (x_offset+image_width)) &&
          (y >= y_offset && y < (ssize_t) (y_offset+image_height)))
        {
          SetPixelRed(q,ScaleCharToQuantum(*p++));
          SetPixelGreen(q,ScaleCharToQuantum(*p++));
          SetPixelBlue(q,ScaleCharToQuantum(*p++));
          SetPixelAlpha(q,ScaleCharToQuantum(*p++));
        }
      else
        {
          SetPixelRed(q,0);
          SetPixelGreen(q,0);
          SetPixelBlue(q,0);
          SetPixelAlpha(q,0);
        }
      q++;
    }
    if (SyncAuthenticPixels(image,exception) == MagickFalse)
      break;
    status=SetImageProgress(image,LoadImageTag,(MagickOffsetType) y,
      image->rows);
    if (status == MagickFalse)
      break;
  }
  WebPFreeDecBuffer(webp_image);
#if defined(MAGICKCORE_WEBPMUX_DELEGATE)
  {
    StringInfo
      *profile;

    uint32_t
      webp_flags = 0;

    WebPData
     chunk,
     content;

    WebPMux
      *mux;

    /*
      Extract any profiles:
      https://developers.google.com/speed/webp/docs/container-api.
    */
    content.bytes=stream;
    content.size=length;
    mux=WebPMuxCreate(&content,0);
    (void) memset(&chunk,0,sizeof(chunk));
    WebPMuxGetFeatures(mux,&webp_flags);
    if (webp_flags & ICCP_FLAG)
      {
        WebPMuxGetChunk(mux,"ICCP",&chunk);
        profile=BlobToStringInfo(chunk.bytes,chunk.size);
        if (profile != (StringInfo *) NULL)
          {
            SetImageProfile(image,"ICC",profile);
            profile=DestroyStringInfo(profile);
          }
      }
    if (webp_flags & EXIF_FLAG)
      {
        WebPMuxGetChunk(mux,"EXIF",&chunk);
        profile=BlobToStringInfo(chunk.bytes,chunk.size);
        if (profile != (StringInfo *) NULL)
          {
            SetImageProfile(image,"EXIF",profile);
            profile=DestroyStringInfo(profile);
          }
      }
    if (webp_flags & XMP_FLAG)
      {
        WebPMuxGetChunk(mux,"XMP",&chunk);
        profile=BlobToStringInfo(chunk.bytes,chunk.size);
        if (profile != (StringInfo *) NULL)
          {
            SetImageProfile(image,"XMP",profile);
            profile=DestroyStringInfo(profile);
          }
      }
    WebPMuxDelete(mux);
  }
#endif
  return(webp_status);
}