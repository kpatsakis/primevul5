static void SVGProcessStyleElement(void *context,const xmlChar *name,
  const char *style)
{
  char
    background[MagickPathExtent],
    *color,
    *keyword,
    *units,
    *value;

  char
    **tokens;

  register ssize_t
    i;

  size_t
    number_tokens;

  SVGInfo
    *svg_info;

  (void) LogMagickEvent(CoderEvent,GetMagickModule(),"  ");
  svg_info=(SVGInfo *) context;
  tokens=SVGKeyValuePairs(context,':',';',style,&number_tokens);
  if (tokens == (char **) NULL)
    return;
  for (i=0; i < (ssize_t) (number_tokens-1); i+=2)
  {
    keyword=(char *) tokens[i];
    value=(char *) tokens[i+1];
    if (LocaleCompare(keyword,"font-size") != 0)
      continue;
    svg_info->pointsize=GetUserSpaceCoordinateValue(svg_info,0,value);
    (void) FormatLocaleFile(svg_info->file,"font-size %g\n",
      svg_info->pointsize);
  }
  color=AcquireString("none");
  units=AcquireString("userSpaceOnUse");
  for (i=0; i < (ssize_t) (number_tokens-1); i+=2)
  {
    keyword=(char *) tokens[i];
    value=(char *) tokens[i+1];
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),"    %s: %s",keyword,
      value);
    switch (*keyword)
    {
      case 'B':
      case 'b':
      {
        if (LocaleCompare((const char *) name,"background") == 0)
          {
            if (LocaleCompare((const char *) name,"svg") == 0)
              (void) CopyMagickString(background,value,MagickPathExtent);
            break;
          }
        break;
      }
      case 'C':
      case 'c':
      {
         if (LocaleCompare(keyword,"clip-path") == 0)
           {
             (void) FormatLocaleFile(svg_info->file,"clip-path \"%s\"\n",value);
             break;
           }
        if (LocaleCompare(keyword,"clip-rule") == 0)
          {
            (void) FormatLocaleFile(svg_info->file,"clip-rule \"%s\"\n",value);
            break;
          }
         if (LocaleCompare(keyword,"clipPathUnits") == 0)
           {
             (void) CloneString(&units,value);
             (void) FormatLocaleFile(svg_info->file,"clip-units \"%s\"\n",
               value);
             break;
           }
        if (LocaleCompare(keyword,"color") == 0)
          {
            (void) CloneString(&color,value);
            (void) FormatLocaleFile(svg_info->file,"currentColor \"%s\"\n",
              color);
            break;
          }
        break;
      }
      case 'F':
      case 'f':
      {
        if (LocaleCompare(keyword,"fill") == 0)
          {
             if (LocaleCompare(value,"currentColor") == 0)
               {
                 (void) FormatLocaleFile(svg_info->file,"fill \"%s\"\n",color);
                 break;
               }
            if (LocaleCompare(value,"#000000ff") == 0)
              (void) FormatLocaleFile(svg_info->file,"fill '#000000'\n");
            else
              (void) FormatLocaleFile(svg_info->file,"fill \"%s\"\n",value);
            break;
          }
        if (LocaleCompare(keyword,"fillcolor") == 0)
          {
            (void) FormatLocaleFile(svg_info->file,"fill \"%s\"\n",value);
            break;
          }
        if (LocaleCompare(keyword,"fill-rule") == 0)
          {
            (void) FormatLocaleFile(svg_info->file,"fill-rule \"%s\"\n",value);
            break;
          }
        if (LocaleCompare(keyword,"fill-opacity") == 0)
          {
            (void) FormatLocaleFile(svg_info->file,"fill-opacity \"%s\"\n",
              value);
            break;
          }
        if (LocaleCompare(keyword,"font") == 0)
          {
            char
              font_family[MagickPathExtent],
              font_size[MagickPathExtent],
              font_style[MagickPathExtent];

            if (sscanf(value,"%2048s %2048s %2048s",font_style,font_size,
                  font_family) != 3)
              break;
            if (GetUserSpaceCoordinateValue(svg_info,0,font_style) == 0)
              (void) FormatLocaleFile(svg_info->file,"font-style \"%s\"\n",
                style);
            else
              if (sscanf(value,"%2048s %2048s",font_size,font_family) != 2)
                break;
            (void) FormatLocaleFile(svg_info->file,"font-size \"%s\"\n",
              font_size);
            (void) FormatLocaleFile(svg_info->file,"font-family \"%s\"\n",
              font_family);
            break;
          }
        if (LocaleCompare(keyword,"font-family") == 0)
          {
            (void) FormatLocaleFile(svg_info->file,"font-family \"%s\"\n",
              value);
            break;
          }
        if (LocaleCompare(keyword,"font-stretch") == 0)
          {
            (void) FormatLocaleFile(svg_info->file,"font-stretch \"%s\"\n",
              value);
            break;
          }
        if (LocaleCompare(keyword,"font-style") == 0)
          {
            (void) FormatLocaleFile(svg_info->file,"font-style \"%s\"\n",value);
            break;
          }
        if (LocaleCompare(keyword,"font-size") == 0)
          {
            svg_info->pointsize=GetUserSpaceCoordinateValue(svg_info,0,value);
            (void) FormatLocaleFile(svg_info->file,"font-size %g\n",
              svg_info->pointsize);
            break;
          }
        if (LocaleCompare(keyword,"font-weight") == 0)
          {
            (void) FormatLocaleFile(svg_info->file,"font-weight \"%s\"\n",
              value);
            break;
          }
        break;
      }
      case 'K':
      case 'k':
      {
        if (LocaleCompare(keyword,"kerning") == 0)
          {
            (void) FormatLocaleFile(svg_info->file,"kerning \"%s\"\n",value);
            break;
          }
        break;
      }
      case 'L':
      case 'l':
      {
        if (LocaleCompare(keyword,"letter-spacing") == 0)
          {
            (void) FormatLocaleFile(svg_info->file,"letter-spacing \"%s\"\n",
              value);
            break;
          }
        break;
      }
      case 'M':
      case 'm':
      {
        if (LocaleCompare(keyword,"mask") == 0)
          {
            (void) FormatLocaleFile(svg_info->file,"mask \"%s\"\n",value);
            break;
          }
        break;
      }
      case 'O':
      case 'o':
      {
        if (LocaleCompare(keyword,"offset") == 0)
          {
            (void) FormatLocaleFile(svg_info->file,"offset %g\n",
              GetUserSpaceCoordinateValue(svg_info,1,value));
            break;
          }
        if (LocaleCompare(keyword,"opacity") == 0)
          {
            (void) FormatLocaleFile(svg_info->file,"opacity \"%s\"\n",value);
            break;
          }
        break;
      }
      case 'S':
      case 's':
      {
        if (LocaleCompare(keyword,"stop-color") == 0)
          {
            (void) CloneString(&svg_info->stop_color,value);
            break;
          }
        if (LocaleCompare(keyword,"stroke") == 0)
          {
            if (LocaleCompare(value,"currentColor") == 0)
              {
                (void) FormatLocaleFile(svg_info->file,"stroke \"%s\"\n",color);
                break;
              }
            if (LocaleCompare(value,"#000000ff") == 0)
              (void) FormatLocaleFile(svg_info->file,"fill '#000000'\n");
            else
              (void) FormatLocaleFile(svg_info->file,
                "stroke \"%s\"\n",value);
            break;
          }
        if (LocaleCompare(keyword,"stroke-antialiasing") == 0)
          {
            (void) FormatLocaleFile(svg_info->file,"stroke-antialias %d\n",
              LocaleCompare(value,"true") == 0);
            break;
          }
        if (LocaleCompare(keyword,"stroke-dasharray") == 0)
          {
            (void) FormatLocaleFile(svg_info->file,"stroke-dasharray %s\n",
              value);
            break;
          }
        if (LocaleCompare(keyword,"stroke-dashoffset") == 0)
          {
            (void) FormatLocaleFile(svg_info->file,"stroke-dashoffset %g\n",
              GetUserSpaceCoordinateValue(svg_info,1,value));
            break;
          }
        if (LocaleCompare(keyword,"stroke-linecap") == 0)
          {
            (void) FormatLocaleFile(svg_info->file,"stroke-linecap \"%s\"\n",
              value);
            break;
          }
        if (LocaleCompare(keyword,"stroke-linejoin") == 0)
          {
            (void) FormatLocaleFile(svg_info->file,"stroke-linejoin \"%s\"\n",
              value);
            break;
          }
        if (LocaleCompare(keyword,"stroke-miterlimit") == 0)
          {
            (void) FormatLocaleFile(svg_info->file,"stroke-miterlimit \"%s\"\n",
              value);
            break;
          }
        if (LocaleCompare(keyword,"stroke-opacity") == 0)
          {
            (void) FormatLocaleFile(svg_info->file,"stroke-opacity \"%s\"\n",
              value);
            break;
          }
        if (LocaleCompare(keyword,"stroke-width") == 0)
          {
            (void) FormatLocaleFile(svg_info->file,"stroke-width %g\n",
              GetUserSpaceCoordinateValue(svg_info,1,value));
            break;
          }
        break;
      }
      case 't':
      case 'T':
      {
        if (LocaleCompare(keyword,"text-align") == 0)
          {
            (void) FormatLocaleFile(svg_info->file,"text-align \"%s\"\n",value);
            break;
          }
        if (LocaleCompare(keyword,"text-anchor") == 0)
          {
            (void) FormatLocaleFile(svg_info->file,"text-anchor \"%s\"\n",
              value);
            break;
          }
        if (LocaleCompare(keyword,"text-decoration") == 0)
          {
            if (LocaleCompare(value,"underline") == 0)
              (void) FormatLocaleFile(svg_info->file,"decorate underline\n");
            if (LocaleCompare(value,"line-through") == 0)
              (void) FormatLocaleFile(svg_info->file,"decorate line-through\n");
            if (LocaleCompare(value,"overline") == 0)
              (void) FormatLocaleFile(svg_info->file,"decorate overline\n");
            break;
          }
        if (LocaleCompare(keyword,"text-antialiasing") == 0)
          {
            (void) FormatLocaleFile(svg_info->file,"text-antialias %d\n",
              LocaleCompare(value,"true") == 0);
            break;
          }
        break;
      }
      default:
        break;
    }
  }
  if (units != (char *) NULL)
    units=DestroyString(units);
  if (color != (char *) NULL)
    color=DestroyString(color);
  for (i=0; tokens[i] != (char *) NULL; i++)
    tokens[i]=DestroyString(tokens[i]);
  tokens=(char **) RelinquishMagickMemory(tokens);
}