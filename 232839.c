MagickExport const char *GetImageProperty(const Image *image,
  const char *property)
{
  double
    alpha;

  ExceptionInfo
    *exception;

  FxInfo
    *fx_info;

  MagickStatusType
    status;

  register const char
    *p;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  p=(const char *) NULL;
  if (image->properties != (void *) NULL)
    {
      if (property == (const char *) NULL)
        {
          ResetSplayTreeIterator((SplayTreeInfo *) image->properties);
          p=(const char *) GetNextValueInSplayTree((SplayTreeInfo *)
            image->properties);
          return(p);
        }
      if (LocaleNCompare("fx:",property,3) != 0) /* NOT fx: !!!! */
        {
          p=(const char *) GetValueFromSplayTree((SplayTreeInfo *)
            image->properties,property);
          if (p != (const char *) NULL)
            return(p);
        }
    }
  if ((property == (const char *) NULL) ||
      (strchr(property,':') == (char *) NULL))
    return(p);
  exception=(&((Image *) image)->exception);
  switch (*property)
  {
    case '8':
    {
      if (LocaleNCompare("8bim:",property,5) == 0)
        {
          (void) Get8BIMProperty(image,property);
          break;
        }
      break;
    }
    case 'E':
    case 'e':
    {
      if (LocaleNCompare("exif:",property,5) == 0)
        {
          (void) GetEXIFProperty(image,property);
          break;
        }
      break;
    }
    case 'F':
    case 'f':
    {
      if (LocaleNCompare("fx:",property,3) == 0)
        {
          if ((image->columns == 0) || (image->rows == 0))
            break;
          fx_info=AcquireFxInfo(image,property+3);
          status=FxEvaluateChannelExpression(fx_info,DefaultChannels,0,0,&alpha,
            exception);
          fx_info=DestroyFxInfo(fx_info);
          if (status != MagickFalse)
            {
              char
                value[MaxTextExtent];

              (void) FormatLocaleString(value,MaxTextExtent,"%.*g",
                GetMagickPrecision(),(double) alpha);
              (void) SetImageProperty((Image *) image,property,value);
            }
          break;
        }
      break;
    }
    case 'H':
    case 'h':
    {
      if (LocaleNCompare("hex:",property,4) == 0)
        {
          MagickPixelPacket
            pixel;

          if ((image->columns == 0) || (image->rows == 0))
            break;
          GetMagickPixelPacket(image,&pixel);
          fx_info=AcquireFxInfo(image,property+4);
          status=FxEvaluateChannelExpression(fx_info,RedChannel,0,0,&alpha,
            exception);
          pixel.red=(MagickRealType) QuantumRange*alpha;
          status&=FxEvaluateChannelExpression(fx_info,GreenChannel,0,0,&alpha,
            exception);
          pixel.green=(MagickRealType) QuantumRange*alpha;
          status&=FxEvaluateChannelExpression(fx_info,BlueChannel,0,0,&alpha,
            exception);
          pixel.blue=(MagickRealType) QuantumRange*alpha;
          status&=FxEvaluateChannelExpression(fx_info,OpacityChannel,0,0,&alpha,
            exception);
          pixel.opacity=(MagickRealType) QuantumRange*(1.0-alpha);
          if (image->colorspace == CMYKColorspace)
            {
              status&=FxEvaluateChannelExpression(fx_info,BlackChannel,0,0,
                &alpha,exception);
              pixel.index=(MagickRealType) QuantumRange*alpha;
            }
          fx_info=DestroyFxInfo(fx_info);
          if (status != MagickFalse)
            {
              char
                hex[MaxTextExtent],
                name[MaxTextExtent];

              (void) QueryMagickColorname(image,&pixel,SVGCompliance,name,
                exception);
              GetColorTuple(&pixel,MagickTrue,hex);
              (void) SetImageProperty((Image *) image,property,hex+1);
            }
          break;
        }
      break;
    }
    case 'I':
    case 'i':
    {
      if ((LocaleNCompare("icc:",property,4) == 0) ||
          (LocaleNCompare("icm:",property,4) == 0))
        {
          (void) GetICCProperty(image,property);
          break;
        }
      if (LocaleNCompare("iptc:",property,5) == 0)
        {
          (void) GetIPTCProperty(image,property);
          break;
        }
      break;
    }
    case 'P':
    case 'p':
    {
      if (LocaleNCompare("pixel:",property,6) == 0)
        {
          MagickPixelPacket
            pixel;

          GetMagickPixelPacket(image,&pixel);
          fx_info=AcquireFxInfo(image,property+6);
          status=FxEvaluateChannelExpression(fx_info,RedChannel,0,0,&alpha,
            exception);
          pixel.red=(MagickRealType) QuantumRange*alpha;
          status&=FxEvaluateChannelExpression(fx_info,GreenChannel,0,0,&alpha,
            exception);
          pixel.green=(MagickRealType) QuantumRange*alpha;
          status&=FxEvaluateChannelExpression(fx_info,BlueChannel,0,0,&alpha,
            exception);
          pixel.blue=(MagickRealType) QuantumRange*alpha;
          status&=FxEvaluateChannelExpression(fx_info,OpacityChannel,0,0,&alpha,
            exception);
          pixel.opacity=(MagickRealType) QuantumRange*(1.0-alpha);
          if (image->colorspace == CMYKColorspace)
            {
              status&=FxEvaluateChannelExpression(fx_info,BlackChannel,0,0,
                &alpha,exception);
              pixel.index=(MagickRealType) QuantumRange*alpha;
            }
          fx_info=DestroyFxInfo(fx_info);
          if (status != MagickFalse)
            {
              char
                name[MaxTextExtent];

              (void) QueryMagickColorname(image,&pixel,SVGCompliance,name,
                exception);
              (void) SetImageProperty((Image *) image,property,name);
            }
          break;
        }
      break;
    }
    case 'X':
    case 'x':
    {
      if (LocaleNCompare("xmp:",property,4) == 0)
        {
          (void) GetXMPProperty(image,property);
          break;
        }
      break;
    }
    default:
      break;
  }
  if (image->properties != (void *) NULL)
    {
      p=(const char *) GetValueFromSplayTree((SplayTreeInfo *)
        image->properties,property);
      return(p);
    }
  return((const char *) NULL);
}