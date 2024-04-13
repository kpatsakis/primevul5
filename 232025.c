static void PushRunlengthPacket(Image *image,const unsigned char *pixels,
  size_t *length,PixelInfo *pixel,ExceptionInfo *exception)
{
  const unsigned char
    *p;

  p=pixels;
  if (image->storage_class == PseudoClass)
    {
      pixel->index=0;
      switch (image->depth)
      {
        case 32:
        {
          pixel->index=ConstrainColormapIndex(image,((size_t) *p << 24) |
            ((size_t) *(p+1) << 16) | ((size_t) *(p+2) << 8) | (size_t) *(p+3),
            exception);
          p+=4;
          break;
        }
        case 16:
        {
          pixel->index=ConstrainColormapIndex(image,(*p << 8) | *(p+1),
            exception);
          p+=2;
          break;
        }
        case 8:
        {
          pixel->index=ConstrainColormapIndex(image,*p,exception);
          p++;
          break;
        }
        default:
          (void) ThrowMagickException(exception,GetMagickModule(),
            CorruptImageError,"ImageDepthNotSupported","`%s'",image->filename);
      }
      switch (image->depth)
      {
        case 8:
        {
          unsigned char
            quantum;

          if (image->alpha_trait != UndefinedPixelTrait)
            {
              p=PushCharPixel(p,&quantum);
              pixel->alpha=ScaleCharToQuantum(quantum);
            }
          break;
        }
        case 16:
        {
          unsigned short
            quantum;

          if (image->alpha_trait != UndefinedPixelTrait)
            {
              p=PushShortPixel(MSBEndian,p,&quantum);
              pixel->alpha=(Quantum) (quantum >> (image->depth-
                MAGICKCORE_QUANTUM_DEPTH));
            }
          break;
        }
        case 32:
        {
          unsigned int
            quantum;

          if (image->alpha_trait != UndefinedPixelTrait)
            {
              p=PushLongPixel(MSBEndian,p,&quantum);
              pixel->alpha=(Quantum) (quantum >> (image->depth-
                MAGICKCORE_QUANTUM_DEPTH));
            }
          break;
        }
        default:
          (void) ThrowMagickException(exception,GetMagickModule(),
            CorruptImageError,"ImageDepthNotSupported","`%s'",image->filename);
      }
      *length=(size_t) (*p++)+1;
      return;
    }
  switch (image->depth)
  {
    case 8:
    {
      unsigned char
        quantum;

      p=PushCharPixel(p,&quantum);
      pixel->red=ScaleCharToQuantum(quantum);
      pixel->green=pixel->red;
      pixel->blue=pixel->red;
      if (IsGrayColorspace(image->colorspace) == MagickFalse)
        {
          p=PushCharPixel(p,&quantum);
          pixel->green=ScaleCharToQuantum(quantum);
          p=PushCharPixel(p,&quantum);
          pixel->blue=ScaleCharToQuantum(quantum);
        }
      if (image->colorspace == CMYKColorspace)
        {
          p=PushCharPixel(p,&quantum);
          pixel->black=ScaleCharToQuantum(quantum);
        }
      if (image->alpha_trait != UndefinedPixelTrait)
        {
          p=PushCharPixel(p,&quantum);
          pixel->alpha=ScaleCharToQuantum(quantum);
        }
      break;
    }
    case 16:
    {
      unsigned short
        quantum;

      p=PushShortPixel(MSBEndian,p,&quantum);
      pixel->red=quantum >> (image->depth-MAGICKCORE_QUANTUM_DEPTH);
      pixel->green=pixel->red;
      pixel->blue=pixel->red;
      if (IsGrayColorspace(image->colorspace) == MagickFalse)
        {
          p=PushShortPixel(MSBEndian,p,&quantum);
          pixel->green=quantum >> (image->depth-MAGICKCORE_QUANTUM_DEPTH);
          p=PushShortPixel(MSBEndian,p,&quantum);
          pixel->blue=quantum >> (image->depth-MAGICKCORE_QUANTUM_DEPTH);
        }
      if (image->colorspace == CMYKColorspace)
        {
          p=PushShortPixel(MSBEndian,p,&quantum);
          pixel->black=quantum >> (image->depth-MAGICKCORE_QUANTUM_DEPTH);
        }
      if (image->alpha_trait != UndefinedPixelTrait)
        {
          p=PushShortPixel(MSBEndian,p,&quantum);
          pixel->alpha=quantum >> (image->depth-MAGICKCORE_QUANTUM_DEPTH);
        }
      break;
    }
    case 32:
    {
      unsigned int
        quantum;

      p=PushLongPixel(MSBEndian,p,&quantum);
      pixel->red=quantum >> (image->depth-MAGICKCORE_QUANTUM_DEPTH);
      pixel->green=pixel->red;
      pixel->blue=pixel->red;
      if (IsGrayColorspace(image->colorspace) == MagickFalse)
        {
          p=PushLongPixel(MSBEndian,p,&quantum);
          pixel->green=quantum >> (image->depth-MAGICKCORE_QUANTUM_DEPTH);
          p=PushLongPixel(MSBEndian,p,&quantum);
          pixel->blue=quantum >> (image->depth-MAGICKCORE_QUANTUM_DEPTH);
        }
      if (image->colorspace == CMYKColorspace)
        {
          p=PushLongPixel(MSBEndian,p,&quantum);
          pixel->black=quantum >> (image->depth-MAGICKCORE_QUANTUM_DEPTH);
        }
      if (image->alpha_trait != UndefinedPixelTrait)
        {
          p=PushLongPixel(MSBEndian,p,&quantum);
          pixel->alpha=quantum >> (image->depth-MAGICKCORE_QUANTUM_DEPTH);
        }
      break;
    }
    default:
      (void) ThrowMagickException(exception,GetMagickModule(),CorruptImageError,
        "ImageDepthNotSupported","`%s'",image->filename);
  }
  *length=(size_t) (*p++)+1;
}