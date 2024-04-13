static MagickBooleanType SyncExifProfile(Image *image, StringInfo *profile)
{
#define MaxDirectoryStack  16
#define EXIF_DELIMITER  "\n"
#define EXIF_NUM_FORMATS  12
#define TAG_EXIF_OFFSET  0x8769
#define TAG_INTEROP_OFFSET  0xa005

  typedef struct _DirectoryInfo
  {
    unsigned char
      *directory;

    size_t
      entry;
  } DirectoryInfo;

  DirectoryInfo
    directory_stack[MaxDirectoryStack];

  EndianType
    endian;

  size_t
    entry,
    length,
    number_entries;

  SplayTreeInfo
    *exif_resources;

  ssize_t
    id,
    level,
    offset;

  static int
    format_bytes[] = {0, 1, 1, 2, 4, 8, 1, 1, 2, 4, 8, 4, 8};

  unsigned char
    *directory,
    *exif;

  /*
    Set EXIF resolution tag.
  */
  length=GetStringInfoLength(profile);
  exif=GetStringInfoDatum(profile);
  if (length < 16)
    return(MagickFalse);
  id=(ssize_t) ReadProfileShort(LSBEndian,exif);
  if ((id != 0x4949) && (id != 0x4D4D))
    {
      while (length != 0)
      {
        if (ReadProfileByte(&exif,&length) != 0x45)
          continue;
        if (ReadProfileByte(&exif,&length) != 0x78)
          continue;
        if (ReadProfileByte(&exif,&length) != 0x69)
          continue;
        if (ReadProfileByte(&exif,&length) != 0x66)
          continue;
        if (ReadProfileByte(&exif,&length) != 0x00)
          continue;
        if (ReadProfileByte(&exif,&length) != 0x00)
          continue;
        break;
      }
      if (length < 16)
        return(MagickFalse);
      id=(ssize_t) ReadProfileShort(LSBEndian,exif);
    }
  endian=LSBEndian;
  if (id == 0x4949)
    endian=LSBEndian;
  else
    if (id == 0x4D4D)
      endian=MSBEndian;
    else
      return(MagickFalse);
  if (ReadProfileShort(endian,exif+2) != 0x002a)
    return(MagickFalse);
  /*
    This the offset to the first IFD.
  */
  offset=(ssize_t) ReadProfileLong(endian,exif+4);
  if ((offset < 0) || ((size_t) offset >= length))
    return(MagickFalse);
  directory=exif+offset;
  level=0;
  entry=0;
  exif_resources=NewSplayTree((int (*)(const void *,const void *)) NULL,
    (void *(*)(void *)) NULL,(void *(*)(void *)) NULL);
  do
  {
    if (level > 0)
      {
        level--;
        directory=directory_stack[level].directory;
        entry=directory_stack[level].entry;
      }
    if ((directory < exif) || (directory > (exif+length-2)))
      break;
    /*
      Determine how many entries there are in the current IFD.
    */
    number_entries=ReadProfileShort(endian,directory);
    for ( ; entry < number_entries; entry++)
    {
      register unsigned char
        *p,
        *q;

      size_t
        number_bytes;

      ssize_t
        components,
        format,
        tag_value;

      q=(unsigned char *) (directory+2+(12*entry));
      if (q > (exif+length-12))
        break;  /* corrupt EXIF */
      if (GetValueFromSplayTree(exif_resources,q) == q)
        break;
      (void) AddValueToSplayTree(exif_resources,q,q);
      tag_value=(ssize_t) ReadProfileShort(endian,q);
      format=(ssize_t) ReadProfileShort(endian,q+2);
      if ((format < 0) || ((format-1) >= EXIF_NUM_FORMATS))
        break;
      components=(ssize_t) ReadProfileLong(endian,q+4);
      if (components < 0)
        break;  /* corrupt EXIF */
      number_bytes=(size_t) components*format_bytes[format];
      if ((ssize_t) number_bytes < components)
        break;  /* prevent overflow */
      if (number_bytes <= 4)
        p=q+8;
      else
        {
          /*
             The directory entry contains an offset.
           */
           offset=(ssize_t) ReadProfileLong(endian,q+8);
          if ((ssize_t) (offset+number_bytes) < offset)
            continue;  /* prevent overflow */
          if ((size_t) (offset+number_bytes) > length)
             continue;
           p=(unsigned char *) (exif+offset);
         }
       switch (tag_value)
      {
        case 0x011a:
        {
          (void) WriteProfileLong(endian,(size_t) (image->x_resolution+0.5),p);
          (void) WriteProfileLong(endian,1UL,p+4);
          break;
        }
        case 0x011b:
        {
          (void) WriteProfileLong(endian,(size_t) (image->y_resolution+0.5),p);
          (void) WriteProfileLong(endian,1UL,p+4);
          break;
        }
        case 0x0112:
        {
          if (number_bytes == 4)
            {
              (void) WriteProfileLong(endian,(size_t) image->orientation,p);
              break;
            }
          (void) WriteProfileShort(endian,(unsigned short) image->orientation,
            p);
          break;
        }
        case 0x0128:
        {
          if (number_bytes == 4)
            {
              (void) WriteProfileLong(endian,(size_t) (image->units+1),p);
              break;
            }
          (void) WriteProfileShort(endian,(unsigned short) (image->units+1),p);
          break;
        }
        default:
          break;
      }
      if ((tag_value == TAG_EXIF_OFFSET) || (tag_value == TAG_INTEROP_OFFSET))
        {
          offset=(ssize_t) ReadProfileLong(endian,p);
          if (((size_t) offset < length) && (level < (MaxDirectoryStack-2)))
            {
              directory_stack[level].directory=directory;
              entry++;
              directory_stack[level].entry=entry;
              level++;
              directory_stack[level].directory=exif+offset;
              directory_stack[level].entry=0;
              level++;
              if ((directory+2+(12*number_entries)) > (exif+length))
                break;
              offset=(ssize_t) ReadProfileLong(endian,directory+2+(12*
                number_entries));
              if ((offset != 0) && ((size_t) offset < length) &&
                  (level < (MaxDirectoryStack-2)))
                {
                  directory_stack[level].directory=exif+offset;
                  directory_stack[level].entry=0;
                  level++;
                }
            }
          break;
        }
    }
  } while (level > 0);
  exif_resources=DestroySplayTree(exif_resources);
  return(MagickTrue);
}
