MagickExport MagickBooleanType SetImageProperty(Image *image,
  const char *property,const char *value)
{
  ExceptionInfo
    *exception;

  MagickBooleanType
    status;

  MagickStatusType
    flags;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      image->filename);
  if (image->properties == (void *) NULL)
    image->properties=NewSplayTree(CompareSplayTreeString,
      RelinquishMagickMemory,RelinquishMagickMemory);  /* create splay-tree */
  if (value == (const char *) NULL)
    return(DeleteImageProperty(image,property));  /* delete if NULL */
  /*
    FUTURE: These should produce 'illegal settings'
     * binary chars in p[roperty key
     * first letter must be a alphabetic
     * single letter property keys (read only)
     * known special prefix (read only, they don't get saved!)
  */
  status=MagickTrue;
  exception=(&image->exception);
  switch (*property)
  {
    case 'B':
    case 'b':
    {
      if (LocaleCompare("background",property) == 0)
        {
          (void) QueryColorDatabase(value,&image->background_color,exception);
          break;
        }
      if (LocaleCompare("bias",property) == 0)
        {
          image->bias=StringToDoubleInterval(value,(double) QuantumRange+1.0);
          break;
        }
      status=AddValueToSplayTree((SplayTreeInfo *) image->properties,
        ConstantString(property),ConstantString(value));
      break;
    }
    case 'C':
    case 'c':
    {
      if (LocaleCompare("colorspace",property) == 0)
        {
          ssize_t
            colorspace;

          colorspace=ParseCommandOption(MagickColorspaceOptions,MagickFalse,
            value);
          if (colorspace < 0)
            break;
          status=SetImageColorspace(image,(ColorspaceType) colorspace);
          break;
        }
      if (LocaleCompare("compose",property) == 0)
        {
          ssize_t
            compose;

          compose=ParseCommandOption(MagickComposeOptions,MagickFalse,value);
          if (compose < 0)
            break;
          image->compose=(CompositeOperator) compose;
          break;
        }
      if (LocaleCompare("compress",property) == 0)
        {
          ssize_t
            compression;

          compression=ParseCommandOption(MagickCompressOptions,MagickFalse,
            value);
          if (compression < 0)
            break;
          image->compression=(CompressionType) compression;
          break;
        }
      status=AddValueToSplayTree((SplayTreeInfo *) image->properties,
        ConstantString(property),ConstantString(value));
      break;
    }
    case 'D':
    case 'd':
    {
      if (LocaleCompare("delay",property) == 0)
        {
          GeometryInfo
            geometry_info;

          flags=ParseGeometry(value,&geometry_info);
          if ((flags & GreaterValue) != 0)
            {
              if (image->delay > (size_t) floor(geometry_info.rho+0.5))
                image->delay=(size_t) floor(geometry_info.rho+0.5);
            }
          else
            if ((flags & LessValue) != 0)
              {
                if (image->delay < (size_t) floor(geometry_info.rho+0.5))
                  image->ticks_per_second=(ssize_t)
                    floor(geometry_info.sigma+0.5);
              }
            else
              image->delay=(size_t) floor(geometry_info.rho+0.5);
          if ((flags & SigmaValue) != 0)
            image->ticks_per_second=(ssize_t) floor(geometry_info.sigma+0.5);
          break;
        }
      if (LocaleCompare("density",property) == 0)
        {
          GeometryInfo
            geometry_info;

          flags=ParseGeometry(value,&geometry_info);
          if ((flags & RhoValue) != 0)
            image->x_resolution=geometry_info.rho;
          image->y_resolution=image->x_resolution;
          if ((flags & SigmaValue) != 0)
            image->y_resolution=geometry_info.sigma;
        }
      if (LocaleCompare("depth",property) == 0)
        {
          image->depth=StringToUnsignedLong(value);
          break;
        }
      if (LocaleCompare("dispose",property) == 0)
        {
          ssize_t
            dispose;

          dispose=ParseCommandOption(MagickDisposeOptions,MagickFalse,value);
          if (dispose < 0)
            break;
          image->dispose=(DisposeType) dispose;
          break;
        }
      status=AddValueToSplayTree((SplayTreeInfo *) image->properties,
        ConstantString(property),ConstantString(value));
      break;
    }
    case 'G':
    case 'g':
    {
      if (LocaleCompare("gamma",property) == 0)
        {
          image->gamma=StringToDouble(value,(char **) NULL);
          break;
        }
      if (LocaleCompare("gravity",property) == 0)
        {
          ssize_t
            gravity;

          gravity=ParseCommandOption(MagickGravityOptions,MagickFalse,value);
          if (gravity < 0)
            break;
          image->gravity=(GravityType) gravity;
          break;
        }
      status=AddValueToSplayTree((SplayTreeInfo *) image->properties,
        ConstantString(property),ConstantString(value));
      break;
    }
    case 'I':
    case 'i':
    {
      if (LocaleCompare("intensity",property) == 0)
        {
          ssize_t
            intensity;

          intensity=ParseCommandOption(MagickPixelIntensityOptions,MagickFalse,
            value);
          if (intensity < 0)
            break;
          image->intensity=(PixelIntensityMethod) intensity;
          break;
        }
      if (LocaleCompare("interpolate",property) == 0)
        {
          ssize_t
            interpolate;

          interpolate=ParseCommandOption(MagickInterpolateOptions,MagickFalse,
            value);
          if (interpolate < 0)
            break;
          image->interpolate=(InterpolatePixelMethod) interpolate;
          break;
        }
      status=AddValueToSplayTree((SplayTreeInfo *) image->properties,
        ConstantString(property),ConstantString(value));
      break;
    }
    case 'L':
    case 'l':
    {
      if (LocaleCompare("loop",property) == 0)
        {
          image->iterations=StringToUnsignedLong(value);
          break;
        }
      status=AddValueToSplayTree((SplayTreeInfo *) image->properties,
        ConstantString(property),ConstantString(value));
      break;
    }
    case 'P':
    case 'p':
    {
      if (LocaleCompare("page",property) == 0)
        {
          char
            *geometry;

          geometry=GetPageGeometry(value);
          flags=ParseAbsoluteGeometry(geometry,&image->page);
          geometry=DestroyString(geometry);
          break;
        }
      if (LocaleCompare("profile",property) == 0)
        {
          ImageInfo
            *image_info;

          StringInfo
            *profile;

          image_info=AcquireImageInfo();
          (void) CopyMagickString(image_info->filename,value,MaxTextExtent);
          (void) SetImageInfo(image_info,1,exception);
          profile=FileToStringInfo(image_info->filename,~0UL,exception);
          if (profile != (StringInfo *) NULL)
            status=SetImageProfile(image,image_info->magick,profile);
          image_info=DestroyImageInfo(image_info);
          break;
        }
      status=AddValueToSplayTree((SplayTreeInfo *) image->properties,
        ConstantString(property),ConstantString(value));
      break;
    }
    case 'R':
    case 'r':
    {
      if (LocaleCompare("rendering-intent",property) == 0)
        {
          ssize_t
            rendering_intent;

          rendering_intent=ParseCommandOption(MagickIntentOptions,MagickFalse,
            value);
          if (rendering_intent < 0)
            break;
          image->rendering_intent=(RenderingIntent) rendering_intent;
          break;
        }
      status=AddValueToSplayTree((SplayTreeInfo *) image->properties,
        ConstantString(property),ConstantString(value));
      break;
    }
    case 'T':
    case 't':
    {
      if (LocaleCompare("tile-offset",property) == 0)
        {
          char
            *geometry;

          geometry=GetPageGeometry(value);
          flags=ParseAbsoluteGeometry(geometry,&image->tile_offset);
          geometry=DestroyString(geometry);
          break;
        }
     if (LocaleCompare("type",property) == 0)
        {
          ssize_t
            type;

          type=ParseCommandOption(MagickTypeOptions,MagickFalse,value);
          if (type < 0)
            return(MagickFalse);
          image->type=(ImageType) type;
          break;
        }
      status=AddValueToSplayTree((SplayTreeInfo *) image->properties,
        ConstantString(property),ConstantString(value));
      break;
    }
    case 'U':
    case 'u':
    {
      if (LocaleCompare("units",property) == 0)
        {
          ssize_t
            units;

          units=ParseCommandOption(MagickResolutionOptions,MagickFalse,value);
          if (units < 0)
            break;
          image->units=(ResolutionType) units;
          break;
        }
      status=AddValueToSplayTree((SplayTreeInfo *) image->properties,
        ConstantString(property),ConstantString(value));
      break;
    }
    default:
    {
      status=AddValueToSplayTree((SplayTreeInfo *) image->properties,
        ConstantString(property),ConstantString(value));
      break;
    }
  }
  return(status);
}