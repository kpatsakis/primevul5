static unsigned char *PopRunlengthPacket(Image *image,unsigned char *pixels,
  size_t length,PixelInfo *pixel,ExceptionInfo *exception)
{
  if (image->storage_class != DirectClass)
    {
      unsigned int
        value;

      value=(unsigned int) ClampToQuantum(pixel->index);
      switch (image->depth)
      {
        case 32:
        {
          *pixels++=(unsigned char) (value >> 24);
          *pixels++=(unsigned char) (value >> 16);
        }
        case 16:
          *pixels++=(unsigned char) (value >> 8);
        case 8:
        {
          *pixels++=(unsigned char) value;
          break;
        }
        default:
          (void) ThrowMagickException(exception,GetMagickModule(),
            CorruptImageError,"ImageDepthNotSupported","`%s'",image->filename);
      }
      switch (image->depth)
      {
        case 32:
        {
          unsigned int
            value;

          if (image->alpha_trait != UndefinedPixelTrait)
            {
              value=ScaleQuantumToLong(ClampToQuantum(pixel->alpha));
              pixels=PopLongPixel(MSBEndian,value,pixels);
            }
          break;
        }
        case 16:
        {
          unsigned short
            value;

          if (image->alpha_trait != UndefinedPixelTrait)
            {
              value=ScaleQuantumToShort(ClampToQuantum(pixel->alpha));
              pixels=PopShortPixel(MSBEndian,value,pixels);
            }
          break;
        }
        case 8:
        {
          unsigned char
            value;

          if (image->alpha_trait != UndefinedPixelTrait)
            {
              value=(unsigned char) ScaleQuantumToChar(ClampToQuantum(
                pixel->alpha));
              pixels=PopCharPixel(value,pixels);
            }
          break;
        }
        default:
          (void) ThrowMagickException(exception,GetMagickModule(),
            CorruptImageError,"ImageDepthNotSupported","`%s'",image->filename);
      }
      *pixels++=(unsigned char) length;
      return(pixels);
    }
  switch (image->depth)
  {
    case 32:
    {
      unsigned int
        value;

      value=ScaleQuantumToLong(ClampToQuantum(pixel->red));
      pixels=PopLongPixel(MSBEndian,value,pixels);
      if (IsGrayColorspace(image->colorspace) == MagickFalse)
        {
          value=ScaleQuantumToLong(ClampToQuantum(pixel->green));
          pixels=PopLongPixel(MSBEndian,value,pixels);
          value=ScaleQuantumToLong(ClampToQuantum(pixel->blue));
          pixels=PopLongPixel(MSBEndian,value,pixels);
        }
      if (image->colorspace == CMYKColorspace)
        {
          value=ScaleQuantumToLong(ClampToQuantum(pixel->black));
          pixels=PopLongPixel(MSBEndian,value,pixels);
        }
      if (image->alpha_trait != UndefinedPixelTrait)
        {
          value=ScaleQuantumToLong(ClampToQuantum(pixel->alpha));
          pixels=PopLongPixel(MSBEndian,value,pixels);
        }
      break;
    }
    case 16:
    {
      unsigned short
        value;

      value=ScaleQuantumToShort(ClampToQuantum(pixel->red));
      pixels=PopShortPixel(MSBEndian,value,pixels);
      if (IsGrayColorspace(image->colorspace) == MagickFalse)
        {
          value=ScaleQuantumToShort(ClampToQuantum(pixel->green));
          pixels=PopShortPixel(MSBEndian,value,pixels);
          value=ScaleQuantumToShort(ClampToQuantum(pixel->blue));
          pixels=PopShortPixel(MSBEndian,value,pixels);
        }
      if (image->colorspace == CMYKColorspace)
        {
          value=ScaleQuantumToShort(ClampToQuantum(pixel->black));
          pixels=PopShortPixel(MSBEndian,value,pixels);
        }
      if (image->alpha_trait != UndefinedPixelTrait)
        {
          value=ScaleQuantumToShort(ClampToQuantum(pixel->alpha));
          pixels=PopShortPixel(MSBEndian,value,pixels);
        }
      break;
    }
    case 8:
    {
      unsigned char
        value;

      value=(unsigned char) ScaleQuantumToChar(ClampToQuantum(pixel->red));
      pixels=PopCharPixel(value,pixels);
      if (IsGrayColorspace(image->colorspace) == MagickFalse)
        {
          value=(unsigned char) ScaleQuantumToChar(ClampToQuantum(
            pixel->green));
          pixels=PopCharPixel(value,pixels);
          value=(unsigned char) ScaleQuantumToChar(ClampToQuantum(pixel->blue));
          pixels=PopCharPixel(value,pixels);
        }
      if (image->colorspace == CMYKColorspace)
        {
          value=(unsigned char) ScaleQuantumToChar(ClampToQuantum(
            pixel->black));
          pixels=PopCharPixel(value,pixels);
        }
      if (image->alpha_trait != UndefinedPixelTrait)
        {
          value=(unsigned char) ScaleQuantumToChar(ClampToQuantum(
            pixel->alpha));
          pixels=PopCharPixel(value,pixels);
        }
      break;
    }
    default:
      (void) ThrowMagickException(exception,GetMagickModule(),CorruptImageError,
        "ImageDepthNotSupported","`%s'",image->filename);
  }
  *pixels++=(unsigned char) length;
  return(pixels);
}