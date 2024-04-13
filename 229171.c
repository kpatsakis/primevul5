static MagickBooleanType WritePICONImage(const ImageInfo *image_info,
  Image *image)
{
#define ColormapExtent  155
#define GraymapExtent  95
#define PiconGeometry  "48x48>"

  static unsigned char
    Colormap[]=
    {
      0x47, 0x49, 0x46, 0x38, 0x37, 0x61, 0x06, 0x00, 0x05, 0x00, 0xf4, 0x05,
      0x00, 0x00, 0x00, 0x00, 0x2f, 0x4f, 0x4f, 0x70, 0x80, 0x90, 0x7e, 0x7e,
      0x7e, 0xdc, 0xdc, 0xdc, 0xff, 0xff, 0xff, 0x00, 0x00, 0x80, 0x00, 0x00,
      0xff, 0x1e, 0x90, 0xff, 0x87, 0xce, 0xeb, 0xe6, 0xe6, 0xfa, 0x00, 0xff,
      0xff, 0x80, 0x00, 0x80, 0xb2, 0x22, 0x22, 0x2e, 0x8b, 0x57, 0x32, 0xcd,
      0x32, 0x00, 0xff, 0x00, 0x98, 0xfb, 0x98, 0xff, 0x00, 0xff, 0xff, 0x00,
      0x00, 0xff, 0x63, 0x47, 0xff, 0xa5, 0x00, 0xff, 0xd7, 0x00, 0xff, 0xff,
      0x00, 0xee, 0x82, 0xee, 0xa0, 0x52, 0x2d, 0xcd, 0x85, 0x3f, 0xd2, 0xb4,
      0x8c, 0xf5, 0xde, 0xb3, 0xff, 0xfa, 0xcd, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x21, 0xf9, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2c, 0x00, 0x00,
      0x00, 0x00, 0x06, 0x00, 0x05, 0x00, 0x00, 0x05, 0x18, 0x20, 0x10, 0x08,
      0x03, 0x51, 0x18, 0x07, 0x92, 0x28, 0x0b, 0xd3, 0x38, 0x0f, 0x14, 0x49,
      0x13, 0x55, 0x59, 0x17, 0x96, 0x69, 0x1b, 0xd7, 0x85, 0x00, 0x3b,
    },
    Graymap[]=
    {
      0x47, 0x49, 0x46, 0x38, 0x37, 0x61, 0x04, 0x00, 0x04, 0x00, 0xf3, 0x0f,
      0x00, 0x00, 0x00, 0x00, 0x12, 0x12, 0x12, 0x21, 0x21, 0x21, 0x33, 0x33,
      0x33, 0x45, 0x45, 0x45, 0x54, 0x54, 0x54, 0x66, 0x66, 0x66, 0x78, 0x78,
      0x78, 0x87, 0x87, 0x87, 0x99, 0x99, 0x99, 0xab, 0xab, 0xab, 0xba, 0xba,
      0xba, 0xcc, 0xcc, 0xcc, 0xde, 0xde, 0xde, 0xed, 0xed, 0xed, 0xff, 0xff,
      0xff, 0x21, 0xf9, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2c, 0x00, 0x00,
      0x00, 0x00, 0x04, 0x00, 0x04, 0x00, 0x00, 0x04, 0x0c, 0x10, 0x04, 0x31,
      0x48, 0x31, 0x07, 0x25, 0xb5, 0x58, 0x73, 0x4f, 0x04, 0x00, 0x3b,
    };

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

  Image
    *affinity_image,
    *picon;

  ImageInfo
    *blob_info;

  MagickBooleanType
    status,
    transparent;

  MagickPixelPacket
    pixel;

  QuantizeInfo
    *quantize_info;

  RectangleInfo
    geometry;

  register const IndexPacket
    *indexes;

  register const PixelPacket
    *p;

  register ssize_t
    i,
    x;

  register PixelPacket
    *q;

  size_t
    characters_per_pixel,
    colors;

  ssize_t
    j,
    k,
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
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
  if (status == MagickFalse)
    return(status);
  (void) TransformImageColorspace(image,sRGBColorspace);
  SetGeometry(image,&geometry);
  (void) ParseMetaGeometry(PiconGeometry,&geometry.x,&geometry.y,
    &geometry.width,&geometry.height);
  picon=ResizeImage(image,geometry.width,geometry.height,TriangleFilter,1.0,
    &image->exception);
  blob_info=CloneImageInfo(image_info);
  (void) AcquireUniqueFilename(blob_info->filename);
  if ((image_info->type != TrueColorType) &&
      (SetImageGray(image,&image->exception) != MagickFalse))
    affinity_image=BlobToImage(blob_info,Graymap,GraymapExtent,
      &image->exception);
  else
    affinity_image=BlobToImage(blob_info,Colormap,ColormapExtent,
      &image->exception);
  (void) RelinquishUniqueFileResource(blob_info->filename);
  blob_info=DestroyImageInfo(blob_info);
  if ((picon == (Image *) NULL) || (affinity_image == (Image *) NULL))
    {
      if (affinity_image != (Image *) NULL)
        affinity_image=DestroyImage(affinity_image);
      if (picon != (Image *) NULL)
        picon=DestroyImage(picon);
      return(MagickFalse);
    }
  quantize_info=AcquireQuantizeInfo(image_info);
  status=RemapImage(quantize_info,picon,affinity_image);
  quantize_info=DestroyQuantizeInfo(quantize_info);
  affinity_image=DestroyImage(affinity_image);
  transparent=MagickFalse;
  exception=(&image->exception);
  if (picon->storage_class == PseudoClass)
    {
      (void) CompressImageColormap(picon);
      if (picon->matte != MagickFalse)
        transparent=MagickTrue;
    }
  else
    {
      /*
        Convert DirectClass to PseudoClass picon.
      */
      if (picon->matte != MagickFalse)
        {
          /*
            Map all the transparent pixels.
          */
          for (y=0; y < (ssize_t) picon->rows; y++)
          {
            q=GetAuthenticPixels(picon,0,y,picon->columns,1,exception);
            if (q == (PixelPacket *) NULL)
              break;
            for (x=0; x < (ssize_t) picon->columns; x++)
            {
              if (q->opacity == (Quantum) TransparentOpacity)
                transparent=MagickTrue;
              else
                SetPixelOpacity(q,OpaqueOpacity);
              q++;
            }
            if (SyncAuthenticPixels(picon,exception) == MagickFalse)
              break;
          }
        }
      (void) SetImageType(picon,PaletteType);
    }
  colors=picon->colors;
  if (transparent != MagickFalse)
    {
      register IndexPacket
        *indexes;

      colors++;
      picon->colormap=(PixelPacket *) ResizeQuantumMemory((void **)
        picon->colormap,(size_t) colors,sizeof(*picon->colormap));
      if (picon->colormap == (PixelPacket *) NULL)
        ThrowWriterException(ResourceLimitError,"MemoryAllocationError");
      picon->colormap[colors-1].red=0;
      picon->colormap[colors-1].green=0;
      picon->colormap[colors-1].blue=0;
      picon->colormap[colors-1].opacity=TransparentOpacity;
      for (y=0; y < (ssize_t) picon->rows; y++)
      {
        q=GetAuthenticPixels(picon,0,y,picon->columns,1,exception);
        if (q == (PixelPacket *) NULL)
          break;
        indexes=GetAuthenticIndexQueue(picon);
        for (x=0; x < (ssize_t) picon->columns; x++)
        {
          if (q->opacity == (Quantum) TransparentOpacity)
            SetPixelIndex(indexes+x,picon->colors);
          q++;
        }
        if (SyncAuthenticPixels(picon,exception) == MagickFalse)
          break;
      }
    }
  /*
    Compute the character per pixel.
  */
  characters_per_pixel=1;
  for (k=MaxCixels; (ssize_t) colors > k; k*=MaxCixels)
    characters_per_pixel++;
  /*
    XPM header.
  */
  (void) WriteBlobString(image,"/* XPM */\n");
  GetPathComponent(picon->filename,BasePath,basename);
  (void) FormatLocaleString(buffer,MaxTextExtent,
    "static char *%.1024s[] = {\n",basename);
  (void) WriteBlobString(image,buffer);
  (void) WriteBlobString(image,"/* columns rows colors chars-per-pixel */\n");
  (void) FormatLocaleString(buffer,MaxTextExtent,
    "\"%.20g %.20g %.20g %.20g\",\n",(double) picon->columns,(double)
    picon->rows,(double) colors,(double) characters_per_pixel);
  (void) WriteBlobString(image,buffer);
  GetMagickPixelPacket(image,&pixel);
  for (i=0; i < (ssize_t) colors; i++)
  {
    const char
      *symbolic;

    /*
      Define XPM color.
    */
    SetMagickPixelPacket(image,picon->colormap+i,(IndexPacket *) NULL,&pixel);
    pixel.colorspace=sRGBColorspace;
    pixel.depth=8;
    pixel.opacity=(MagickRealType) OpaqueOpacity;
    (void) QueryMagickColorname(image,&pixel,XPMCompliance,name,
      &image->exception);
    if (transparent != MagickFalse)
      {
        if (i == (ssize_t) (colors-1))
          (void) CopyMagickString(name,"grey75",MaxTextExtent);
      }
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
    symbolic=(const char *) GetValueFromSplayTree(xpm_symbolic,name);
    if (symbolic == (const char *) NULL)
      (void) FormatLocaleString(buffer,MaxTextExtent,"\"%.1024s c %.1024s\",\n",
        symbol,name);
    else
      (void) FormatLocaleString(buffer,MaxTextExtent,
        "\"%.1024s c %.1024s %.1024s\",\n",symbol,name,symbolic);
    (void) WriteBlobString(image,buffer);
  }
  /*
    Define XPM pixels.
  */
  (void) WriteBlobString(image,"/* pixels */\n");
  for (y=0; y < (ssize_t) picon->rows; y++)
  {
    p=GetVirtualPixels(picon,0,y,picon->columns,1,&picon->exception);
    if (p == (const PixelPacket *) NULL)
      break;
    indexes=GetVirtualIndexQueue(picon);
    (void) WriteBlobString(image,"\"");
    for (x=0; x < (ssize_t) picon->columns; x++)
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
      y == (ssize_t) (picon->rows-1) ? "" : ",");
    (void) WriteBlobString(image,buffer);
    status=SetImageProgress(image,SaveImageTag,(MagickOffsetType) y,
      picon->rows);
    if (status == MagickFalse)
      break;
  }
  picon=DestroyImage(picon);
  (void) WriteBlobString(image,"};\n");
  (void) CloseBlob(image);
  return(MagickTrue);
}