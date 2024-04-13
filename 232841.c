MagickExport const char *GetMagickProperty(const ImageInfo *image_info,
  Image *image,const char *property)
{
  char
    value[MaxTextExtent];

  const char
    *string;

  assert(property[0] != '\0');
  if (property[1] == '\0')  /* single letter property request */
    return(GetMagickPropertyLetter(image_info,image,*property));
  *value='\0';  /* formatted string */
  string=(char *) NULL;  /* constant string reference */
  switch (*property)
  {
    case 'b':
    {
      if ((LocaleCompare("base",property) == 0) ||
          (LocaleCompare("basename",property) == 0) )
        {
          GetPathComponent(image->magick_filename,BasePath,value);
          break;
        }
      if (LocaleCompare("bit-depth",property) == 0)
        {
          (void) FormatLocaleString(value,MaxTextExtent,"%.20g",(double)
            GetImageDepth(image,&image->exception));
          break;
        }
      if (LocaleCompare("bounding-box",property) == 0)
        {
          RectangleInfo
            geometry;

          geometry=GetImageBoundingBox(image,&image->exception);
          (void) FormatLocaleString(value,MagickPathExtent,"%g,%g %g,%g\n",
            (double) geometry.x,(double) geometry.y,
            (double) geometry.x+geometry.width,
            (double) geometry.y+geometry.height);
          break;
        }
      break;
    }
    case 'c':
    {
      if (LocaleCompare("channels",property) == 0)
        {
          /*
            Image channels.
          */
          (void) FormatLocaleString(value,MaxTextExtent,"%s",
            CommandOptionToMnemonic(MagickColorspaceOptions,(ssize_t)
            image->colorspace));
          LocaleLower(value);
          if (image->matte != MagickFalse)
            (void) ConcatenateMagickString(value,"a",MaxTextExtent);
          break;
        }
      if (LocaleCompare("colors",property) == 0)
        {
          image->colors=GetNumberColors(image,(FILE *) NULL,&image->exception);
          (void) FormatLocaleString(value,MaxTextExtent,"%.20g",(double)
            image->colors);
          break;
        }
      if (LocaleCompare("colorspace",property) == 0)
        {
          string=CommandOptionToMnemonic(MagickColorspaceOptions,(ssize_t)
            image->colorspace);
          break;
        }
      if (LocaleCompare("compose",property) == 0)
        {
          string=CommandOptionToMnemonic(MagickComposeOptions,(ssize_t)
            image->compose);
          break;
        }
      if (LocaleCompare("compression",property) == 0)
        {
          string=CommandOptionToMnemonic(MagickCompressOptions,(ssize_t)
            image->compression);
          break;
        }
      if (LocaleCompare("copyright",property) == 0)
        {
          (void) CopyMagickString(value,GetMagickCopyright(),MaxTextExtent);
          break;
        }
      break;
    }
    case 'd':
    {
      if (LocaleCompare("depth",property) == 0)
        {
          (void) FormatLocaleString(value,MaxTextExtent,"%.20g",(double)
            image->depth);
          break;
        }
      if (LocaleCompare("directory",property) == 0)
        {
          GetPathComponent(image->magick_filename,HeadPath,value);
          break;
        }
      break;
    }
    case 'e':
    {
      if (LocaleCompare("entropy",property) == 0)
        {
          double
            entropy;

          (void) GetImageChannelEntropy(image,image_info->channel,&entropy,
            &image->exception);
          (void) FormatLocaleString(value,MaxTextExtent,"%.*g",
            GetMagickPrecision(),entropy);
          break;
        }
      if (LocaleCompare("extension",property) == 0)
        {
          GetPathComponent(image->magick_filename,ExtensionPath,value);
          break;
        }
      break;
    }
    case 'g':
    {
      if (LocaleCompare("gamma",property) == 0)
        {
          (void) FormatLocaleString(value,MaxTextExtent,"%.*g",
            GetMagickPrecision(),image->gamma);
          break;
        }
      if ((image_info != (ImageInfo *) NULL) &&
          (LocaleCompare("group",property) == 0))
        {
          (void) FormatLocaleString(value,MaxTextExtent,"0x%lx",(unsigned long)
            image_info->group);
          break;
        }
      break;
    }
    case 'h':
    {
      if (LocaleCompare("height",property) == 0)
        {
          (void) FormatLocaleString(value,MaxTextExtent,"%.20g",
            image->magick_rows != 0 ? (double) image->magick_rows : 256.0);
          break;
        }
      break;
    }
    case 'i':
    {
      if (LocaleCompare("input",property) == 0)
        {
          string=image->filename;
          break;
        }
      if (LocaleCompare("interlace",property) == 0)
        {
          string=CommandOptionToMnemonic(MagickInterlaceOptions,(ssize_t)
            image->interlace);
          break;
        }
      break;
    }
    case 'k':
    {
      if (LocaleCompare("kurtosis",property) == 0)
        {
          double
            kurtosis,
            skewness;

          (void) GetImageChannelKurtosis(image,image_info->channel,&kurtosis,
            &skewness,&image->exception);
          (void) FormatLocaleString(value,MaxTextExtent,"%.*g",
            GetMagickPrecision(),kurtosis);
          break;
        }
      break;
    }
    case 'm':
    {
      if (LocaleCompare("magick",property) == 0)
        {
          string=image->magick;
          break;
        }
      if ((LocaleCompare("max",property) == 0) ||
          (LocaleCompare("maxima",property) == 0))
        {
          double
            maximum,
            minimum;

          (void) GetImageChannelRange(image,image_info->channel,&minimum,
            &maximum,&image->exception);
          (void) FormatLocaleString(value,MaxTextExtent,"%.*g",
            GetMagickPrecision(),maximum);
          break;
        }
      if (LocaleCompare("mean",property) == 0)
        {
          double
            mean,
            standard_deviation;

          (void) GetImageChannelMean(image,image_info->channel,&mean,
            &standard_deviation,&image->exception);
          (void) FormatLocaleString(value,MaxTextExtent,"%.*g",
            GetMagickPrecision(),mean);
          break;
        }
      if ((LocaleCompare("min",property) == 0) ||
          (LocaleCompare("minima",property) == 0))
        {
          double
            maximum,
            minimum;

          (void) GetImageChannelRange(image,image_info->channel,&minimum,
            &maximum,&image->exception);
          (void) FormatLocaleString(value,MaxTextExtent,"%.*g",
            GetMagickPrecision(),minimum);
          break;
        }
      break;
    }
    case 'o':
    {
      if (LocaleCompare("opaque",property) == 0)
        {
          MagickBooleanType
            opaque;

          opaque=IsOpaqueImage(image,&image->exception);
          (void) CopyMagickString(value,opaque != MagickFalse ? "true" :
            "false",MaxTextExtent);
          break;
        }
      if (LocaleCompare("orientation",property) == 0)
        {
          string=CommandOptionToMnemonic(MagickOrientationOptions,(ssize_t)
            image->orientation);
          break;
        }
      if ((image_info != (ImageInfo *) NULL) &&
          (LocaleCompare("output",property) == 0))
        {
          (void) CopyMagickString(value,image_info->filename,MaxTextExtent);
          break;
        }
     break;
    }
    case 'p':
    {
      if (LocaleCompare("page",property) == 0)
        {
          (void) FormatLocaleString(value,MaxTextExtent,"%.20gx%.20g",(double)
            image->page.width,(double) image->page.height);
          break;
        }
#if defined(MAGICKCORE_LCMS_DELEGATE)
      if (LocaleCompare("profile:icc",property) == 0 ||
          LocaleCompare("profile:icm",property) == 0)
        {
#if !defined(LCMS_VERSION) || (LCMS_VERSION < 2000)
#define cmsUInt32Number  DWORD
#endif

          const StringInfo
            *profile;

          cmsHPROFILE
            icc_profile;

          profile=GetImageProfile(image,property+8);
          if (profile == (StringInfo *) NULL)
            break;

          icc_profile=cmsOpenProfileFromMem(GetStringInfoDatum(profile),
            (cmsUInt32Number) GetStringInfoLength(profile));
          if (icc_profile != (cmsHPROFILE *) NULL)
            {
#if defined(LCMS_VERSION) && (LCMS_VERSION < 2000)
              string=cmsTakeProductName(icc_profile);
#else
              (void) cmsGetProfileInfoASCII(icc_profile,cmsInfoDescription,
                "en","US",value,MaxTextExtent);
#endif
              (void) cmsCloseProfile(icc_profile);
            }
      }
#endif
      if (LocaleCompare("printsize.x",property) == 0)
        {
          (void) FormatLocaleString(value,MagickPathExtent,"%.*g",
            GetMagickPrecision(),PerceptibleReciprocal(image->x_resolution)*
              image->columns);
          break;
        }
      if (LocaleCompare("printsize.y",property) == 0)
        {
          (void) FormatLocaleString(value,MagickPathExtent,"%.*g",
            GetMagickPrecision(),PerceptibleReciprocal(image->y_resolution)*
              image->rows);
          break;
        }
      if (LocaleCompare("profiles",property) == 0)
        {
          const char
            *name;

          ResetImageProfileIterator(image);
          name=GetNextImageProfile(image);
          if (name != (char *) NULL)
            {
              (void) CopyMagickString(value,name,MaxTextExtent);
              name=GetNextImageProfile(image);
              while (name != (char *) NULL)
              {
                ConcatenateMagickString(value,",",MaxTextExtent);
                ConcatenateMagickString(value,name,MaxTextExtent);
                name=GetNextImageProfile(image);
              }
            }
          break;
        }
      break;
    }
    case 'q':
    {
      if (LocaleCompare("quality",property) == 0)
        {
          (void) FormatLocaleString(value,MaxTextExtent,"%.20g",(double)
            image->quality);
          break;
        }
      break;
    }
    case 'r':
    {
      if (LocaleCompare("rendering-intent",property) == 0)
        {
          string=CommandOptionToMnemonic(MagickIntentOptions,(ssize_t)
            image->rendering_intent);
          break;
        }
      if (LocaleCompare("resolution.x",property) == 0)
        {
          (void) FormatLocaleString(value,MaxTextExtent,"%g",
            image->x_resolution);
          break;
        }
      if (LocaleCompare("resolution.y",property) == 0)
        {
          (void) FormatLocaleString(value,MaxTextExtent,"%g",
            image->y_resolution);
          break;
        }
      break;
    }
    case 's':
    {
      if (LocaleCompare("scene",property) == 0)
        {
          if ((image_info != (ImageInfo *) NULL) &&
              (image_info->number_scenes != 0))
            (void) FormatLocaleString(value,MaxTextExtent,"%.20g",(double)
              image_info->scene);
          else
            (void) FormatLocaleString(value,MaxTextExtent,"%.20g",(double)
              image->scene);
          break;
        }
      if (LocaleCompare("scenes",property) == 0)
        {
          (void) FormatLocaleString(value,MaxTextExtent,"%.20g",(double)
            GetImageListLength(image));
          break;
        }
      if (LocaleCompare("size",property) == 0)
        {
          (void) FormatMagickSize(GetBlobSize(image),MagickFalse,value);
          break;
        }
      if (LocaleCompare("skewness",property) == 0)
        {
          double
            kurtosis,
            skewness;

          (void) GetImageChannelKurtosis(image,image_info->channel,&kurtosis,
            &skewness,&image->exception);
          (void) FormatLocaleString(value,MaxTextExtent,"%.*g",
            GetMagickPrecision(),skewness);
          break;
        }
      if ((LocaleCompare("standard-deviation",property) == 0) ||
          (LocaleCompare("standard_deviation",property) == 0))
        {
          double
            mean,
            standard_deviation;

          (void) GetImageChannelMean(image,image_info->channel,&mean,
            &standard_deviation,&image->exception);
          (void) FormatLocaleString(value,MaxTextExtent,"%.*g",
            GetMagickPrecision(),standard_deviation);
          break;
        }
       break;
    }
    case 't':
    {
      if (LocaleCompare("type",property) == 0)
        {
          string=CommandOptionToMnemonic(MagickTypeOptions,(ssize_t)
            IdentifyImageType(image,&image->exception));
          break;
        }
       break;
    }
    case 'u':
    {
      if ((image_info != (ImageInfo *) NULL) &&
          (LocaleCompare("unique",property) == 0))
        {
          string=image_info->unique;
          break;
        }
      if (LocaleCompare("units",property) == 0)
        {
          /*
            Image resolution units.
          */
          string=CommandOptionToMnemonic(MagickResolutionOptions,(ssize_t)
            image->units);
          break;
        }
      break;
    }
    case 'v':
    {
      if (LocaleCompare("version",property) == 0)
        {
          string=GetMagickVersion((size_t *) NULL);
          break;
        }
      break;
    }
    case 'w':
    {
      if (LocaleCompare("width",property) == 0)
        {
          (void) FormatLocaleString(value,MaxTextExtent,"%.20g",(double)
            (image->magick_columns != 0 ? image->magick_columns : 256));
          break;
        }
      break;
    }
    case 'x': /* FUTURE: Obsolete X resolution */
    {
      if ((LocaleCompare("xresolution",property) == 0) ||
          (LocaleCompare("x-resolution",property) == 0) )
        {
          (void) FormatLocaleString(value,MaxTextExtent,"%.20g",
            image->x_resolution);
          break;
        }
      break;
    }
    case 'y': /* FUTURE: Obsolete Y resolution */
    {
      if ((LocaleCompare("yresolution",property) == 0) ||
          (LocaleCompare("y-resolution",property) == 0) )
        {
          (void) FormatLocaleString(value,MaxTextExtent,"%.20g",
            image->y_resolution);
          break;
        }
      break;
    }
    case 'z':
    {
      if ((image_info != (ImageInfo *) NULL) &&
          (LocaleCompare("zero",property) == 0))
        {
          string=image_info->zero;
          break;
        }
      break;
    }
  }
  if (*value != '\0')
    string=value;
  if (string != (char *) NULL)
    {
      (void) SetImageArtifact(image,"get-property", string);
      return(GetImageArtifact(image,"get-property"));
    }
  return((char *) NULL);
}