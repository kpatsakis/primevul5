static MagickBooleanType WriteXPMImage(const ImageInfo *image_info,Image *image)
{
#define MaxCixels  92

  static const char
    Cixel[MaxCixels+1] = " .XoO+@#$%&*=-;:>,<1234567890qwertyuipasdfghjk"
                         "lzxcvbnmMNBVCZASDFGHJKLPIUYTREWQ!~^/()_`'][{}|";

  char
    buffer[MaxTextExtent],
    basename[MaxTextExtent],
    name[MaxTextExtent],
    symbol[MaxTextExtent];

  ExceptionInfo
    *exception;

  MagickBooleanType
    status;

  MagickPixelPacket
    pixel;

  register const IndexPacket
    *indexes;

  register const PixelPacket
    *p;

  register ssize_t
    i,
    x;

  size_t
    characters_per_pixel;

  ssize_t
    j,
    k,
    opacity,
    y;

  /*
    Open output image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  exception=(&image->exception);
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,exception);
  if (status == MagickFalse)
    return(status);
  if (IssRGBCompatibleColorspace(image->colorspace) == MagickFalse)
    (void) TransformImageColorspace(image,sRGBColorspace);
  opacity=(-1);
  if (image->matte == MagickFalse)
    {
      if ((image->storage_class == DirectClass) || (image->colors > 256))
        (void) SetImageType(image,PaletteType);
    }
  else
    {
      MagickRealType
        alpha,
        beta;

      /*
        Identify transparent colormap index.
      */
      if ((image->storage_class == DirectClass) || (image->colors > 256))
        (void) SetImageType(image,PaletteBilevelMatteType);
      for (i=0; i < (ssize_t) image->colors; i++)
        if (image->colormap[i].opacity != OpaqueOpacity)
          {
            if (opacity < 0)
              {
                opacity=i;
                continue;
              }
            alpha=(Quantum) TransparentOpacity-(MagickRealType)
              image->colormap[i].opacity;
            beta=(Quantum) TransparentOpacity-(MagickRealType)
              image->colormap[opacity].opacity;
            if (alpha < beta)
              opacity=i;
          }
      if (opacity == -1)
        {
          (void) SetImageType(image,PaletteBilevelMatteType);
          for (i=0; i < (ssize_t) image->colors; i++)
            if (image->colormap[i].opacity != OpaqueOpacity)
              {
                if (opacity < 0)
                  {
                    opacity=i;
                    continue;
                  }
                alpha=(Quantum) TransparentOpacity-(MagickRealType)
                  image->colormap[i].opacity;
                beta=(Quantum) TransparentOpacity-(MagickRealType)
                  image->colormap[opacity].opacity;
                if (alpha < beta)
                  opacity=i;
              }
        }
      if (opacity >= 0)
        {
          image->colormap[opacity].red=image->transparent_color.red;
          image->colormap[opacity].green=image->transparent_color.green;
          image->colormap[opacity].blue=image->transparent_color.blue;
        }
    }
  /*
    Compute the character per pixel.
  */
  characters_per_pixel=1;
  for (k=MaxCixels; (ssize_t) image->colors > k; k*=MaxCixels)
    characters_per_pixel++;
  /*
    XPM header.
  */
  (void) WriteBlobString(image,"/* XPM */\n");
  GetPathComponent(image->filename,BasePath,basename);
  if (isalnum((int) ((unsigned char) *basename)) == 0)
    {
      (void) FormatLocaleString(buffer,MaxTextExtent,"xpm_%.1024s",basename);
      (void) CopyMagickString(basename,buffer,MaxTextExtent);
    }
  if (isalpha((int) ((unsigned char) basename[0])) == 0)
    basename[0]='_';
  for (i=1; basename[i] != '\0'; i++)
    if (isalnum((int) ((unsigned char) basename[i])) == 0)
      basename[i]='_';
  (void) FormatLocaleString(buffer,MaxTextExtent,
    "static char *%.1024s[] = {\n",basename);
  (void) WriteBlobString(image,buffer);
  (void) WriteBlobString(image,"/* columns rows colors chars-per-pixel */\n");
  (void) FormatLocaleString(buffer,MaxTextExtent,
    "\"%.20g %.20g %.20g %.20g \",\n",(double) image->columns,(double)
    image->rows,(double) image->colors,(double) characters_per_pixel);
  (void) WriteBlobString(image,buffer);
  GetMagickPixelPacket(image,&pixel);
  for (i=0; i < (ssize_t) image->colors; i++)
  {
    /*
      Define XPM color.
    */
    SetMagickPixelPacket(image,image->colormap+i,(IndexPacket *) NULL,&pixel);
    pixel.colorspace=sRGBColorspace;
    pixel.depth=8;
    pixel.opacity=(MagickRealType) OpaqueOpacity;
    (void) QueryMagickColorname(image,&pixel,XPMCompliance,name,exception);
    if (i == opacity)
      (void) CopyMagickString(name,"None",MaxTextExtent);
    /*
      Write XPM color.
    */
    k=i % MaxCixels;
    symbol[0]=Cixel[k];
    for (j=1; j < (ssize_t) characters_per_pixel; j++)
    {
      k=((i-k)/MaxCixels) % MaxCixels;
      symbol[j]=Cixel[k];
    }
    symbol[j]='\0';
    (void) FormatLocaleString(buffer,MaxTextExtent,
      "\"%.1024s c %.1024s\",\n",symbol,name);
    (void) WriteBlobString(image,buffer);
  }
  /*
    Define XPM pixels.
  */
  (void) WriteBlobString(image,"/* pixels */\n");
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    p=GetVirtualPixels(image,0,y,image->columns,1,exception);
    if (p == (const PixelPacket *) NULL)
      break;
    indexes=GetVirtualIndexQueue(image);
    (void) WriteBlobString(image,"\"");
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      k=((ssize_t) GetPixelIndex(indexes+x) % MaxCixels);
      symbol[0]=Cixel[k];
      for (j=1; j < (ssize_t) characters_per_pixel; j++)
      {
        k=(((int) GetPixelIndex(indexes+x)-k)/MaxCixels) % MaxCixels;
        symbol[j]=Cixel[k];
      }
      symbol[j]='\0';
      (void) CopyMagickString(buffer,symbol,MaxTextExtent);
      (void) WriteBlobString(image,buffer);
    }
    (void) FormatLocaleString(buffer,MaxTextExtent,"\"%.1024s\n",
      (y == (ssize_t) (image->rows-1) ? "" : ","));
    (void) WriteBlobString(image,buffer);
    if (image->previous == (Image *) NULL)
      {
        status=SetImageProgress(image,SaveImageTag,(MagickOffsetType) y,
          image->rows);
        if (status == MagickFalse)
          break;
      }
  }
  (void) WriteBlobString(image,"};\n");
  (void) CloseBlob(image);
  return(MagickTrue);
}