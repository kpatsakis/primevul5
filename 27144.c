static Image *ReadHDRImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  char
    format[MaxTextExtent],
    keyword[MaxTextExtent],
    tag[MaxTextExtent],
    value[MaxTextExtent];

  double
    gamma;

  Image
    *image;

  int
    c;

  MagickBooleanType
    status,
    value_expected;

  register Quantum
    *q;

  register ssize_t
    i,
    x;

  register unsigned char
    *p;

  ssize_t
    count,
    y;

  unsigned char
    *end,
    pixel[4],
    *pixels;

  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  if (image_info->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      image_info->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  image=AcquireImage(image_info,exception);
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == MagickFalse)
    {
      image=DestroyImageList(image);
      return((Image *) NULL);
    }
  /*
    Decode image header.
  */
  image->columns=0;
  image->rows=0;
  *format='\0';
  c=ReadBlobByte(image);
  if (c == EOF)
    {
      image=DestroyImage(image);
      return((Image *) NULL);
    }
  while (isgraph(c) && (image->columns == 0) && (image->rows == 0))
  {
    if (c == (int) '#')
      {
        char
          *comment;

        register char
          *p;

        size_t
          length;

        /*
          Read comment-- any text between # and end-of-line.
        */
        length=MaxTextExtent;
        comment=AcquireString((char *) NULL);
        for (p=comment; comment != (char *) NULL; p++)
        {
          c=ReadBlobByte(image);
          if ((c == EOF) || (c == (int) '\n'))
            break;
          if ((size_t) (p-comment+1) >= length)
            {
              *p='\0';
              length<<=1;
              comment=(char *) ResizeQuantumMemory(comment,length+
                MaxTextExtent,sizeof(*comment));
              if (comment == (char *) NULL)
                break;
              p=comment+strlen(comment);
            }
          *p=(char) c;
        }
        if (comment == (char *) NULL)
          ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
        *p='\0';
        (void) SetImageProperty(image,"comment",comment,exception);
        comment=DestroyString(comment);
        c=ReadBlobByte(image);
      }
    else
      if (isalnum(c) == MagickFalse)
        c=ReadBlobByte(image);
      else
        {
          register char
            *p;

          /*
            Determine a keyword and its value.
          */
          p=keyword;
          do
          {
            if ((size_t) (p-keyword) < (MaxTextExtent-1))
              *p++=c;
            c=ReadBlobByte(image);
          } while (isalnum(c) || (c == '_'));
          *p='\0';
          value_expected=MagickFalse;
          while ((isspace((int) ((unsigned char) c)) != 0) || (c == '='))
          {
            if (c == '=')
              value_expected=MagickTrue;
            c=ReadBlobByte(image);
          }
          if (LocaleCompare(keyword,"Y") == 0)
            value_expected=MagickTrue;
           if (value_expected == MagickFalse)
             continue;
           p=value;
          while ((c != '\n') && (c != '\0'))
           {
             if ((size_t) (p-value) < (MaxTextExtent-1))
               *p++=c;
            c=ReadBlobByte(image);
          }
          *p='\0';
          /*
            Assign a value to the specified keyword.
          */
          switch (*keyword)
          {
            case 'F':
            case 'f':
            {
              if (LocaleCompare(keyword,"format") == 0)
                {
                  (void) CopyMagickString(format,value,MaxTextExtent);
                  break;
                }
              (void) FormatLocaleString(tag,MaxTextExtent,"hdr:%s",keyword);
              (void) SetImageProperty(image,tag,value,exception);
              break;
            }
            case 'G':
            case 'g':
            {
              if (LocaleCompare(keyword,"gamma") == 0)
                {
                  image->gamma=StringToDouble(value,(char **) NULL);
                  break;
                }
              (void) FormatLocaleString(tag,MaxTextExtent,"hdr:%s",keyword);
              (void) SetImageProperty(image,tag,value,exception);
              break;
            }
            case 'P':
            case 'p':
            {
              if (LocaleCompare(keyword,"primaries") == 0)
                {
                  float
                     chromaticity[6],
                     white_point[2];
 
                  (void) sscanf(value,"%g %g %g %g %g %g %g %g",
                    &chromaticity[0],&chromaticity[1],&chromaticity[2],
                    &chromaticity[3],&chromaticity[4],&chromaticity[5],
                    &white_point[0],&white_point[1]);
                  image->chromaticity.red_primary.x=chromaticity[0];
                  image->chromaticity.red_primary.y=chromaticity[1];
                  image->chromaticity.green_primary.x=chromaticity[2];
                  image->chromaticity.green_primary.y=chromaticity[3];
                  image->chromaticity.blue_primary.x=chromaticity[4];
                  image->chromaticity.blue_primary.y=chromaticity[5];
                  image->chromaticity.white_point.x=white_point[0],
                  image->chromaticity.white_point.y=white_point[1];
                   break;
                 }
               (void) FormatLocaleString(tag,MaxTextExtent,"hdr:%s",keyword);
              (void) SetImageProperty(image,tag,value,exception);
              break;
            }
            case 'Y':
            case 'y':
            {
              char
                target[] = "Y";

              if (strcmp(keyword,target) == 0)
                {
                  int
                     height,
                     width;
 
                  (void) sscanf(value,"%d +X %d",&height,&width);
                  image->columns=(size_t) width;
                  image->rows=(size_t) height;
                   break;
                 }
               (void) FormatLocaleString(tag,MaxTextExtent,"hdr:%s",keyword);
              (void) SetImageProperty(image,tag,value,exception);
              break;
            }
            default:
            {
              (void) FormatLocaleString(tag,MaxTextExtent,"hdr:%s",keyword);
              (void) SetImageProperty(image,tag,value,exception);
              break;
            }
          }
        }
    if ((image->columns == 0) && (image->rows == 0))
      while (isspace((int) ((unsigned char) c)) != 0)
        c=ReadBlobByte(image);
  }
  if ((LocaleCompare(format,"32-bit_rle_rgbe") != 0) &&
      (LocaleCompare(format,"32-bit_rle_xyze") != 0))
    ThrowReaderException(CorruptImageError,"ImproperImageHeader");
  if ((image->columns == 0) || (image->rows == 0))
    ThrowReaderException(CorruptImageError,"NegativeOrZeroImageSize");
  (void) SetImageColorspace(image,RGBColorspace,exception);
  if (LocaleCompare(format,"32-bit_rle_xyze") == 0)
    (void) SetImageColorspace(image,XYZColorspace,exception);
  image->compression=(image->columns < 8) || (image->columns > 0x7ffff) ?
    NoCompression : RLECompression;
  if (image_info->ping != MagickFalse)
    {
      (void) CloseBlob(image);
      return(GetFirstImageInList(image));
    }
  status=SetImageExtent(image,image->columns,image->rows,exception);
  if (status == MagickFalse)
    return(DestroyImageList(image));
  /*
    Read RGBE (red+green+blue+exponent) pixels.
  */
  pixels=(unsigned char *) AcquireQuantumMemory(image->columns,4*
    sizeof(*pixels));
  if (pixels == (unsigned char *) NULL)
    ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    if (image->compression != RLECompression)
      {
        count=ReadBlob(image,4*image->columns*sizeof(*pixels),pixels);
        if (count != (ssize_t) (4*image->columns*sizeof(*pixels)))
          break;
      }
    else
      {
        count=ReadBlob(image,4*sizeof(*pixel),pixel);
        if (count != 4)
          break;
        if ((size_t) ((((size_t) pixel[2]) << 8) | pixel[3]) != image->columns)
          {
            (void) memcpy(pixels,pixel,4*sizeof(*pixel));
            count=ReadBlob(image,4*(image->columns-1)*sizeof(*pixels),pixels+4);
            image->compression=NoCompression;
          }
        else
          {
            p=pixels;
            for (i=0; i < 4; i++)
            {
              end=&pixels[(i+1)*image->columns];
              while (p < end)
              {
                count=ReadBlob(image,2*sizeof(*pixel),pixel);
                if (count < 1)
                  break;
                if (pixel[0] > 128)
                  {
                    count=(ssize_t) pixel[0]-128;
                    if ((count == 0) || (count > (ssize_t) (end-p)))
                      break;
                    while (count-- > 0)
                      *p++=pixel[1];
                  }
                else
                  {
                    count=(ssize_t) pixel[0];
                    if ((count == 0) || (count > (ssize_t) (end-p)))
                      break;
                    *p++=pixel[1];
                    if (--count > 0)
                      {
                        count=ReadBlob(image,(size_t) count*sizeof(*p),p);
                        if (count < 1)
                          break;
                        p+=count;
                      }
                  }
              }
            }
          }
      }
    q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
    if (q == (Quantum *) NULL)
      break;
    i=0;
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      if (image->compression == RLECompression)
        {
          pixel[0]=pixels[x];
          pixel[1]=pixels[x+image->columns];
          pixel[2]=pixels[x+2*image->columns];
          pixel[3]=pixels[x+3*image->columns];
        }
      else
        {
          pixel[0]=pixels[i++];
          pixel[1]=pixels[i++];
          pixel[2]=pixels[i++];
          pixel[3]=pixels[i++];
        }
      SetPixelRed(image,0,q);
      SetPixelGreen(image,0,q);
      SetPixelBlue(image,0,q);
      if (pixel[3] != 0)
        {
          gamma=pow(2.0,pixel[3]-(128.0+8.0));
          SetPixelRed(image,ClampToQuantum(QuantumRange*gamma*pixel[0]),q);
          SetPixelGreen(image,ClampToQuantum(QuantumRange*gamma*pixel[1]),q);
          SetPixelBlue(image,ClampToQuantum(QuantumRange*gamma*pixel[2]),q);
        }
      q+=GetPixelChannels(image);
    }
    if (SyncAuthenticPixels(image,exception) == MagickFalse)
      break;
    status=SetImageProgress(image,LoadImageTag,(MagickOffsetType) y,
      image->rows);
    if (status == MagickFalse)
      break;
  }
  pixels=(unsigned char *) RelinquishMagickMemory(pixels);
  if (EOFBlob(image) != MagickFalse)
    ThrowFileException(exception,CorruptImageError,"UnexpectedEndOfFile",
      image->filename);
  (void) CloseBlob(image);
  return(GetFirstImageInList(image));
}
