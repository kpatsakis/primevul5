static MagickBooleanType TraceSVGImage(Image *image,ExceptionInfo *exception)
{
#if defined(MAGICKCORE_AUTOTRACE_DELEGATE)
  {
    at_bitmap
      *trace;

    at_fitting_opts_type
      *fitting_options;

    at_output_opts_type
      *output_options;

    at_splines_type
      *splines;

    ImageType
      type;

    register const Quantum
      *p;

    register ssize_t
      i,
      x;

    size_t
      number_planes;

    ssize_t
      y;

    /*
      Trace image and write as SVG.
    */
    fitting_options=at_fitting_opts_new();
    output_options=at_output_opts_new();
    (void) SetImageGray(image,exception);
    type=GetImageType(image);
    number_planes=3;
    if ((type == BilevelType) || (type == GrayscaleType))
      number_planes=1;
    trace=at_bitmap_new(image->columns,image->rows,number_planes);
    i=0;
    for (y=0; y < (ssize_t) image->rows; y++)
    {
      p=GetVirtualPixels(image,0,y,image->columns,1,exception);
      if (p == (const Quantum *) NULL)
        break;
      for (x=0; x < (ssize_t) image->columns; x++)
      {
        trace->bitmap[i++]=GetPixelRed(image,p);
        if (number_planes == 3)
          {
            trace->bitmap[i++]=GetPixelGreen(image,p);
            trace->bitmap[i++]=GetPixelBlue(image,p);
          }
        p+=GetPixelChannels(image);
      }
    }
    splines=at_splines_new_full(trace,fitting_options,NULL,NULL,NULL,NULL,NULL,
      NULL);
    at_splines_write(at_output_get_handler_by_suffix((char *) "svg"),
      GetBlobFileHandle(image),image->filename,output_options,splines,NULL,
      NULL);
    /*
      Free resources.
    */
    at_splines_free(splines);
    at_bitmap_free(trace);
    at_output_opts_free(output_options);
    at_fitting_opts_free(fitting_options);
  }
#else
  {
    char
      *base64,
      filename[MagickPathExtent],
      message[MagickPathExtent];

    const DelegateInfo
      *delegate_info;

    Image
      *clone_image;

    ImageInfo
      *image_info;

    MagickBooleanType
      status;

    register char
      *p;

    size_t
      blob_length,
      encode_length;

    ssize_t
      i;

    unsigned char
      *blob;

    delegate_info=GetDelegateInfo((char *) NULL,"TRACE",exception);
    if (delegate_info != (DelegateInfo *) NULL)
      {
        /*
          Trace SVG with tracing delegate.
        */
        image_info=AcquireImageInfo();
        (void) CopyMagickString(image_info->magick,"TRACE",MagickPathExtent);
        (void) FormatLocaleString(filename,MagickPathExtent,"trace:%s",
          image_info->filename);
        (void) CopyMagickString(image_info->filename,filename,MagickPathExtent);
        status=WriteImage(image_info,image,exception);
        image_info=DestroyImageInfo(image_info);
        return(status);
      }
    (void) WriteBlobString(image,
      "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n");
    (void) WriteBlobString(image,
      "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\"");
    (void) WriteBlobString(image,
      " \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n");
    (void) FormatLocaleString(message,MagickPathExtent,
      "<svg version=\"1.1\" id=\"Layer_1\" "
      "xmlns=\"http://www.w3.org/2000/svg\" "
      "xmlns:xlink=\"http://www.w3.org/1999/xlink\" x=\"0px\" y=\"0px\" "
      "width=\"%.20gpx\" height=\"%.20gpx\" viewBox=\"0 0 %.20g %.20g\" "
      "enable-background=\"new 0 0 %.20g %.20g\" xml:space=\"preserve\">",
      (double) image->columns,(double) image->rows,
      (double) image->columns,(double) image->rows,
      (double) image->columns,(double) image->rows);
    (void) WriteBlobString(image,message);
    clone_image=CloneImage(image,0,0,MagickTrue,exception);
    if (clone_image == (Image *) NULL)
      return(MagickFalse);
    image_info=AcquireImageInfo();
    (void) CopyMagickString(image_info->magick,"PNG",MagickPathExtent);
    blob_length=2048;
    blob=(unsigned char *) ImageToBlob(image_info,clone_image,&blob_length,
      exception);
    clone_image=DestroyImage(clone_image);
    image_info=DestroyImageInfo(image_info);
    if (blob == (unsigned char *) NULL)
      return(MagickFalse);
    encode_length=0;
    base64=Base64Encode(blob,blob_length,&encode_length);
    blob=(unsigned char *) RelinquishMagickMemory(blob);
    (void) FormatLocaleString(message,MagickPathExtent,
      "  <image id=\"image%.20g\" width=\"%.20g\" height=\"%.20g\" "
      "x=\"%.20g\" y=\"%.20g\"\n    href=\"data:image/png;base64,",
      (double) image->scene,(double) image->columns,(double) image->rows,
      (double) image->page.x,(double) image->page.y);
    (void) WriteBlobString(image,message);
    p=base64;
    for (i=(ssize_t) encode_length; i > 0; i-=76)
    {
      (void) FormatLocaleString(message,MagickPathExtent,"%.76s",p);
      (void) WriteBlobString(image,message);
      p+=76;
      if (i > 76)
        (void) WriteBlobString(image,"\n");
    }
    base64=DestroyString(base64);
    (void) WriteBlobString(image,"\" />\n");
    (void) WriteBlobString(image,"</svg>\n");
  }
#endif
  (void) CloseBlob(image);
  return(MagickTrue);
}