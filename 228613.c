static MagickBooleanType WritePDFImage(const ImageInfo *image_info,Image *image,
  ExceptionInfo *exception)
{
#define CFormat  "/Filter [ /%s ]\n"
#define ObjectsPerImage  14
#define ThrowPDFException(exception,message) \
{ \
  if (xref != (MagickOffsetType *) NULL) \
    xref=(MagickOffsetType *) RelinquishMagickMemory(xref); \
  ThrowWriterException((exception),(message)); \
}

DisableMSCWarning(4310)
  static const char
    XMPProfile[]=
    {
      "<?xpacket begin=\"%s\" id=\"W5M0MpCehiHzreSzNTczkc9d\"?>\n"
      "<x:xmpmeta xmlns:x=\"adobe:ns:meta/\" x:xmptk=\"Adobe XMP Core 4.0-c316 44.253921, Sun Oct 01 2006 17:08:23\">\n"
      "   <rdf:RDF xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\">\n"
      "      <rdf:Description rdf:about=\"\"\n"
      "            xmlns:xap=\"http://ns.adobe.com/xap/1.0/\">\n"
      "         <xap:ModifyDate>%s</xap:ModifyDate>\n"
      "         <xap:CreateDate>%s</xap:CreateDate>\n"
      "         <xap:MetadataDate>%s</xap:MetadataDate>\n"
      "         <xap:CreatorTool>%s</xap:CreatorTool>\n"
      "      </rdf:Description>\n"
      "      <rdf:Description rdf:about=\"\"\n"
      "            xmlns:dc=\"http://purl.org/dc/elements/1.1/\">\n"
      "         <dc:format>application/pdf</dc:format>\n"
      "         <dc:title>\n"
      "           <rdf:Alt>\n"
      "              <rdf:li xml:lang=\"x-default\">%s</rdf:li>\n"
      "           </rdf:Alt>\n"
      "         </dc:title>\n"
      "      </rdf:Description>\n"
      "      <rdf:Description rdf:about=\"\"\n"
      "            xmlns:xapMM=\"http://ns.adobe.com/xap/1.0/mm/\">\n"
      "         <xapMM:DocumentID>uuid:6ec119d7-7982-4f56-808d-dfe64f5b35cf</xapMM:DocumentID>\n"
      "         <xapMM:InstanceID>uuid:a79b99b4-6235-447f-9f6c-ec18ef7555cb</xapMM:InstanceID>\n"
      "      </rdf:Description>\n"
      "      <rdf:Description rdf:about=\"\"\n"
      "            xmlns:pdf=\"http://ns.adobe.com/pdf/1.3/\">\n"
      "         <pdf:Producer>%s</pdf:Producer>\n"
      "      </rdf:Description>\n"
      "      <rdf:Description rdf:about=\"\"\n"
      "            xmlns:pdfaid=\"http://www.aiim.org/pdfa/ns/id/\">\n"
      "         <pdfaid:part>3</pdfaid:part>\n"
      "         <pdfaid:conformance>B</pdfaid:conformance>\n"
      "      </rdf:Description>\n"
      "   </rdf:RDF>\n"
      "</x:xmpmeta>\n"
      "<?xpacket end=\"w\"?>\n"
    },
    XMPProfileMagick[4]= { (char) 0xef, (char) 0xbb, (char) 0xbf, (char) 0x00 };
RestoreMSCWarning

  char
    basename[MagickPathExtent],
    buffer[MagickPathExtent],
    *escape,
    date[MagickPathExtent],
    **labels,
    page_geometry[MagickPathExtent],
    *url;

  CompressionType
    compression;

  const char
    *device,
    *option,
    *value;

  const StringInfo
    *profile;

  double
    pointsize;

  GeometryInfo
    geometry_info;

  Image
    *next,
    *tile_image;

  MagickBooleanType
    status;

  MagickOffsetType
    offset,
    scene,
    *xref;

  MagickSizeType
    number_pixels;

  MagickStatusType
    flags;

  PointInfo
    delta,
    resolution,
    scale;

  RectangleInfo
    geometry,
    media_info,
    page_info;

  register const Quantum
    *p;

  register unsigned char
    *q;

  register ssize_t
    i,
    x;

  size_t
    channels,
    imageListLength,
    info_id,
    length,
    object,
    pages_id,
    root_id,
    text_size,
    version;

  ssize_t
    count,
    page_count,
    y;

  struct tm
    utc_time;

  time_t
    seconds;

  unsigned char
    *pixels;

  /*
    Open output image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,exception);
  if (status == MagickFalse)
    return(status);
  /*
    Allocate X ref memory.
  */
  xref=(MagickOffsetType *) AcquireQuantumMemory(2048UL,sizeof(*xref));
  if (xref == (MagickOffsetType *) NULL)
    ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");
  (void) memset(xref,0,2048UL*sizeof(*xref));
  /*
    Write Info object.
  */
  object=0;
  version=3;
  if (image_info->compression == JPEG2000Compression)
    version=(size_t) MagickMax(version,5);
  for (next=image; next != (Image *) NULL; next=GetNextImageInList(next))
    if (next->alpha_trait != UndefinedPixelTrait)
      version=(size_t) MagickMax(version,4);
  if (LocaleCompare(image_info->magick,"PDFA") == 0)
    version=(size_t) MagickMax(version,6);
  profile=GetImageProfile(image,"icc");
  if (profile != (StringInfo *) NULL)
    version=(size_t) MagickMax(version,7);
  (void) FormatLocaleString(buffer,MagickPathExtent,"%%PDF-1.%.20g \n",(double)
    version);
  (void) WriteBlobString(image,buffer);
  if (LocaleCompare(image_info->magick,"PDFA") == 0)
    {
      (void) WriteBlobByte(image,'%');
      (void) WriteBlobByte(image,0xe2);
      (void) WriteBlobByte(image,0xe3);
      (void) WriteBlobByte(image,0xcf);
      (void) WriteBlobByte(image,0xd3);
      (void) WriteBlobByte(image,'\n');
    }
  /*
    Write Catalog object.
  */
  xref[object++]=TellBlob(image);
  root_id=object;
  (void) FormatLocaleString(buffer,MagickPathExtent,"%.20g 0 obj\n",(double)
    object);
  (void) WriteBlobString(image,buffer);
  (void) WriteBlobString(image,"<<\n");
  if (LocaleCompare(image_info->magick,"PDFA") != 0)
    (void) FormatLocaleString(buffer,MagickPathExtent,"/Pages %.20g 0 R\n",
      (double) object+1);
  else
    {
      (void) FormatLocaleString(buffer,MagickPathExtent,"/Metadata %.20g 0 R\n",
        (double) object+1);
      (void) WriteBlobString(image,buffer);
      (void) FormatLocaleString(buffer,MagickPathExtent,"/Pages %.20g 0 R\n",
        (double) object+2);
    }
  (void) WriteBlobString(image,buffer);
  (void) WriteBlobString(image,"/Type /Catalog");
  option=GetImageOption(image_info,"pdf:page-direction");
  if ((option != (const char *) NULL) &&
      (LocaleCompare(option,"right-to-left") == 0))
    (void) WriteBlobString(image,"/ViewerPreferences<</PageDirection/R2L>>\n");
  (void) WriteBlobString(image,"\n");
  (void) WriteBlobString(image,">>\n");
  (void) WriteBlobString(image,"endobj\n");
  GetPathComponent(image->filename,BasePath,basename);
  if (LocaleCompare(image_info->magick,"PDFA") == 0)
    {
      char
        create_date[MagickPathExtent],
        modify_date[MagickPathExtent],
        timestamp[MagickPathExtent],
        *url,
        xmp_profile[MagickPathExtent];

      /*
        Write XMP object.
      */
      xref[object++]=TellBlob(image);
      (void) FormatLocaleString(buffer,MagickPathExtent,"%.20g 0 obj\n",(double)
        object);
      (void) WriteBlobString(image,buffer);
      (void) WriteBlobString(image,"<<\n");
      (void) WriteBlobString(image,"/Subtype /XML\n");
      *modify_date='\0';
      value=GetImageProperty(image,"date:modify",exception);
      if (value != (const char *) NULL)
        (void) CopyMagickString(modify_date,value,MagickPathExtent);
      *create_date='\0';
      value=GetImageProperty(image,"date:create",exception);
      if (value != (const char *) NULL)
        (void) CopyMagickString(create_date,value,MagickPathExtent);
      (void) FormatMagickTime(GetMagickTime(),MagickPathExtent,timestamp);
      url=(char *) MagickAuthoritativeURL;
      escape=EscapeParenthesis(basename);
      i=FormatLocaleString(xmp_profile,MagickPathExtent,XMPProfile,
        XMPProfileMagick,modify_date,create_date,timestamp,url,escape,url);
      escape=DestroyString(escape);
      (void) FormatLocaleString(buffer,MagickPathExtent,"/Length %.20g\n",
        (double) i);
      (void) WriteBlobString(image,buffer);
      (void) WriteBlobString(image,"/Type /Metadata\n");
      (void) WriteBlobString(image,">>\nstream\n");
      (void) WriteBlobString(image,xmp_profile);
      (void) WriteBlobString(image,"\nendstream\n");
      (void) WriteBlobString(image,"endobj\n");
    }
  /*
    Write Pages object.
  */
  xref[object++]=TellBlob(image);
  pages_id=object;
  (void) FormatLocaleString(buffer,MagickPathExtent,"%.20g 0 obj\n",(double)
    object);
  (void) WriteBlobString(image,buffer);
  (void) WriteBlobString(image,"<<\n");
  (void) WriteBlobString(image,"/Type /Pages\n");
  (void) FormatLocaleString(buffer,MagickPathExtent,"/Kids [ %.20g 0 R ",
    (double) object+1);
  (void) WriteBlobString(image,buffer);
  count=(ssize_t) (pages_id+ObjectsPerImage+1);
  page_count=1;
  if (image_info->adjoin != MagickFalse)
    {
      Image
        *kid_image;

      /*
        Predict page object id's.
      */
      kid_image=image;
      for ( ; GetNextImageInList(kid_image) != (Image *) NULL; count+=ObjectsPerImage)
      {
        page_count++;
        profile=GetImageProfile(kid_image,"icc");
        if (profile != (StringInfo *) NULL)
          count+=2;
        (void) FormatLocaleString(buffer,MagickPathExtent,"%.20g 0 R ",(double)
          count);
        (void) WriteBlobString(image,buffer);
        kid_image=GetNextImageInList(kid_image);
      }
      xref=(MagickOffsetType *) ResizeQuantumMemory(xref,(size_t) count+2048UL,
        sizeof(*xref));
      if (xref == (MagickOffsetType *) NULL)
        ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");
    }
  (void) WriteBlobString(image,"]\n");
  (void) FormatLocaleString(buffer,MagickPathExtent,"/Count %.20g\n",(double)
    page_count);
  (void) WriteBlobString(image,buffer);
  (void) WriteBlobString(image,">>\n");
  (void) WriteBlobString(image,"endobj\n");
  scene=0;
  imageListLength=GetImageListLength(image);
  do
  {
    MagickBooleanType
      has_icc_profile;

    profile=GetImageProfile(image,"icc");
    has_icc_profile=(profile != (StringInfo *) NULL) ? MagickTrue : MagickFalse;
    compression=image->compression;
    if (image_info->compression != UndefinedCompression)
      compression=image_info->compression;
    switch (compression)
    {
      case FaxCompression:
      case Group4Compression:
      {
        if ((SetImageMonochrome(image,exception) == MagickFalse) ||
            (image->alpha_trait != UndefinedPixelTrait))
          compression=RLECompression;
        break;
      }
#if !defined(MAGICKCORE_JPEG_DELEGATE)
      case JPEGCompression:
      {
        compression=RLECompression;
        (void) ThrowMagickException(exception,GetMagickModule(),
          MissingDelegateError,"DelegateLibrarySupportNotBuiltIn","`%s' (JPEG)",
          image->filename);
        break;
      }
#endif
#if !defined(MAGICKCORE_LIBOPENJP2_DELEGATE)
      case JPEG2000Compression:
      {
        compression=RLECompression;
        (void) ThrowMagickException(exception,GetMagickModule(),
          MissingDelegateError,"DelegateLibrarySupportNotBuiltIn","`%s' (JP2)",
          image->filename);
        break;
      }
#endif
#if !defined(MAGICKCORE_ZLIB_DELEGATE)
      case ZipCompression:
      {
        compression=RLECompression;
        (void) ThrowMagickException(exception,GetMagickModule(),
          MissingDelegateError,"DelegateLibrarySupportNotBuiltIn","`%s' (ZLIB)",
          image->filename);
        break;
      }
#endif
      case LZWCompression:
      {
        if (LocaleCompare(image_info->magick,"PDFA") == 0)
          compression=RLECompression;  /* LZW compression is forbidden */
        break;
      }
      case NoCompression:
      {
        if (LocaleCompare(image_info->magick,"PDFA") == 0)
          compression=RLECompression; /* ASCII 85 compression is forbidden */
        break;
      }
      default:
        break;
    }
    if (compression == JPEG2000Compression)
      (void) TransformImageColorspace(image,sRGBColorspace,exception);
    /*
      Scale relative to dots-per-inch.
    */
    delta.x=DefaultResolution;
    delta.y=DefaultResolution;
    resolution.x=image->resolution.x;
    resolution.y=image->resolution.y;
    if ((resolution.x == 0.0) || (resolution.y == 0.0))
      {
        flags=ParseGeometry(PSDensityGeometry,&geometry_info);
        resolution.x=geometry_info.rho;
        resolution.y=geometry_info.sigma;
        if ((flags & SigmaValue) == 0)
          resolution.y=resolution.x;
      }
    if (image_info->density != (char *) NULL)
      {
        flags=ParseGeometry(image_info->density,&geometry_info);
        resolution.x=geometry_info.rho;
        resolution.y=geometry_info.sigma;
        if ((flags & SigmaValue) == 0)
          resolution.y=resolution.x;
      }
    if (image->units == PixelsPerCentimeterResolution)
      {
        resolution.x=(double) ((size_t) (100.0*2.54*resolution.x+0.5)/100.0);
        resolution.y=(double) ((size_t) (100.0*2.54*resolution.y+0.5)/100.0);
      }
    SetGeometry(image,&geometry);
    (void) FormatLocaleString(page_geometry,MagickPathExtent,"%.20gx%.20g",
      (double) image->columns,(double) image->rows);
    if (image_info->page != (char *) NULL)
      (void) CopyMagickString(page_geometry,image_info->page,MagickPathExtent);
    else
      if ((image->page.width != 0) && (image->page.height != 0))
        (void) FormatLocaleString(page_geometry,MagickPathExtent,
          "%.20gx%.20g%+.20g%+.20g",(double) image->page.width,(double)
          image->page.height,(double) image->page.x,(double) image->page.y);
      else
        if ((image->gravity != UndefinedGravity) &&
            (LocaleCompare(image_info->magick,"PDF") == 0))
          (void) CopyMagickString(page_geometry,PSPageGeometry,
            MagickPathExtent);
    (void) ConcatenateMagickString(page_geometry,">",MagickPathExtent);
    (void) ParseMetaGeometry(page_geometry,&geometry.x,&geometry.y,
      &geometry.width,&geometry.height);
    scale.x=(double) (geometry.width*delta.x)/resolution.x;
    geometry.width=(size_t) floor(scale.x+0.5);
    scale.y=(double) (geometry.height*delta.y)/resolution.y;
    geometry.height=(size_t) floor(scale.y+0.5);
    (void) ParseAbsoluteGeometry(page_geometry,&media_info);
    (void) ParseGravityGeometry(image,page_geometry,&page_info,exception);
    if (image->gravity != UndefinedGravity)
      {
        geometry.x=(-page_info.x);
        geometry.y=(ssize_t) (media_info.height+page_info.y-image->rows);
      }
    pointsize=12.0;
    if (image_info->pointsize != 0.0)
      pointsize=image_info->pointsize;
    text_size=0;
    value=GetImageProperty(image,"label",exception);
    if (value != (const char *) NULL)
      text_size=(size_t) (MultilineCensus(value)*pointsize+12);
    (void) text_size;
    /*
      Write Page object.
    */
    xref[object++]=TellBlob(image);
    (void) FormatLocaleString(buffer,MagickPathExtent,"%.20g 0 obj\n",(double)
      object);
    (void) WriteBlobString(image,buffer);
    (void) WriteBlobString(image,"<<\n");
    (void) WriteBlobString(image,"/Type /Page\n");
    (void) FormatLocaleString(buffer,MagickPathExtent,"/Parent %.20g 0 R\n",
      (double) pages_id);
    (void) WriteBlobString(image,buffer);
    (void) WriteBlobString(image,"/Resources <<\n");
    labels=(char **) NULL;
    value=GetImageProperty(image,"label",exception);
    if (value != (const char *) NULL)
      labels=StringToList(value);
    if (labels != (char **) NULL)
      {
        (void) FormatLocaleString(buffer,MagickPathExtent,
          "/Font << /F%.20g %.20g 0 R >>\n",(double) image->scene,(double)
          object+4);
        (void) WriteBlobString(image,buffer);
      }
    (void) FormatLocaleString(buffer,MagickPathExtent,
      "/XObject << /Im%.20g %.20g 0 R >>\n",(double) image->scene,(double)
      object+5);
    (void) WriteBlobString(image,buffer);
    (void) FormatLocaleString(buffer,MagickPathExtent,"/ProcSet %.20g 0 R >>\n",
      (double) object+3);
    (void) WriteBlobString(image,buffer);
    (void) FormatLocaleString(buffer,MagickPathExtent,
      "/MediaBox [0 0 %g %g]\n",72.0*media_info.width/resolution.x,
      72.0*media_info.height/resolution.y);
    (void) WriteBlobString(image,buffer);
    (void) FormatLocaleString(buffer,MagickPathExtent,
      "/CropBox [0 0 %g %g]\n",72.0*media_info.width/resolution.x,
      72.0*media_info.height/resolution.y);
    (void) WriteBlobString(image,buffer);
    (void) FormatLocaleString(buffer,MagickPathExtent,"/Contents %.20g 0 R\n",
      (double) object+1);
    (void) WriteBlobString(image,buffer);
    (void) FormatLocaleString(buffer,MagickPathExtent,"/Thumb %.20g 0 R\n",
      (double) object+(has_icc_profile != MagickFalse ? 10 : 8));
    (void) WriteBlobString(image,buffer);
    (void) WriteBlobString(image,">>\n");
    (void) WriteBlobString(image,"endobj\n");
    /*
      Write Contents object.
    */
    xref[object++]=TellBlob(image);
    (void) FormatLocaleString(buffer,MagickPathExtent,"%.20g 0 obj\n",(double)
      object);
    (void) WriteBlobString(image,buffer);
    (void) WriteBlobString(image,"<<\n");
    (void) FormatLocaleString(buffer,MagickPathExtent,"/Length %.20g 0 R\n",
      (double) object+1);
    (void) WriteBlobString(image,buffer);
    (void) WriteBlobString(image,">>\n");
    (void) WriteBlobString(image,"stream\n");
    offset=TellBlob(image);
    (void) WriteBlobString(image,"q\n");
    if (labels != (char **) NULL)
      for (i=0; labels[i] != (char *) NULL; i++)
      {
        (void) WriteBlobString(image,"BT\n");
        (void) FormatLocaleString(buffer,MagickPathExtent,"/F%.20g %g Tf\n",
          (double) image->scene,pointsize);
        (void) WriteBlobString(image,buffer);
        (void) FormatLocaleString(buffer,MagickPathExtent,"%.20g %.20g Td\n",
          (double) geometry.x,(double) (geometry.y+geometry.height+i*pointsize+
          12));
        (void) WriteBlobString(image,buffer);
        (void) FormatLocaleString(buffer,MagickPathExtent,"(%s) Tj\n",
           labels[i]);
        (void) WriteBlobString(image,buffer);
        (void) WriteBlobString(image,"ET\n");
        labels[i]=DestroyString(labels[i]);
      }
    (void) FormatLocaleString(buffer,MagickPathExtent,
      "%g 0 0 %g %.20g %.20g cm\n",scale.x,scale.y,(double) geometry.x,
      (double) geometry.y);
    (void) WriteBlobString(image,buffer);
    (void) FormatLocaleString(buffer,MagickPathExtent,"/Im%.20g Do\n",(double)
      image->scene);
    (void) WriteBlobString(image,buffer);
    (void) WriteBlobString(image,"Q\n");
    offset=TellBlob(image)-offset;
    (void) WriteBlobString(image,"\nendstream\n");
    (void) WriteBlobString(image,"endobj\n");
    /*
      Write Length object.
    */
    xref[object++]=TellBlob(image);
    (void) FormatLocaleString(buffer,MagickPathExtent,"%.20g 0 obj\n",(double)
      object);
    (void) WriteBlobString(image,buffer);
    (void) FormatLocaleString(buffer,MagickPathExtent,"%.20g\n",(double)
      offset);
    (void) WriteBlobString(image,buffer);
    (void) WriteBlobString(image,"endobj\n");
    /*
      Write Procset object.
    */
    xref[object++]=TellBlob(image);
    (void) FormatLocaleString(buffer,MagickPathExtent,"%.20g 0 obj\n",(double)
      object);
    (void) WriteBlobString(image,buffer);
    if ((image->storage_class == DirectClass) || (image->colors > 256))
      (void) CopyMagickString(buffer,"[ /PDF /Text /ImageC",MagickPathExtent);
    else
      if ((compression == FaxCompression) || (compression == Group4Compression))
        (void) CopyMagickString(buffer,"[ /PDF /Text /ImageB",MagickPathExtent);
      else
        (void) CopyMagickString(buffer,"[ /PDF /Text /ImageI",MagickPathExtent);
    (void) WriteBlobString(image,buffer);
    (void) WriteBlobString(image," ]\n");
    (void) WriteBlobString(image,"endobj\n");
    /*
      Write Font object.
    */
    xref[object++]=TellBlob(image);
    (void) FormatLocaleString(buffer,MagickPathExtent,"%.20g 0 obj\n",(double)
      object);
    (void) WriteBlobString(image,buffer);
    (void) WriteBlobString(image,"<<\n");
    if (labels != (char **) NULL)
      {
        (void) WriteBlobString(image,"/Type /Font\n");
        (void) WriteBlobString(image,"/Subtype /Type1\n");
        (void) FormatLocaleString(buffer,MagickPathExtent,"/Name /F%.20g\n",
          (double) image->scene);
        (void) WriteBlobString(image,buffer);
        (void) WriteBlobString(image,"/BaseFont /Helvetica\n");
        (void) WriteBlobString(image,"/Encoding /MacRomanEncoding\n");
        labels=(char **) RelinquishMagickMemory(labels);
      }
    (void) WriteBlobString(image,">>\n");
    (void) WriteBlobString(image,"endobj\n");
    /*
      Write XObject object.
    */
    xref[object++]=TellBlob(image);
    (void) FormatLocaleString(buffer,MagickPathExtent,"%.20g 0 obj\n",(double)
      object);
    (void) WriteBlobString(image,buffer);
    (void) WriteBlobString(image,"<<\n");
    (void) WriteBlobString(image,"/Type /XObject\n");
    (void) WriteBlobString(image,"/Subtype /Image\n");
    (void) FormatLocaleString(buffer,MagickPathExtent,"/Name /Im%.20g\n",
      (double) image->scene);
    (void) WriteBlobString(image,buffer);
    switch (compression)
    {
      case NoCompression:
      {
        (void) FormatLocaleString(buffer,MagickPathExtent,CFormat,
          "ASCII85Decode");
        break;
      }
      case JPEGCompression:
      {
        (void) FormatLocaleString(buffer,MagickPathExtent,CFormat,"DCTDecode");
        if (image->colorspace != CMYKColorspace)
          break;
        (void) WriteBlobString(image,buffer);
        (void) CopyMagickString(buffer,"/Decode [1 0 1 0 1 0 1 0]\n",
          MagickPathExtent);
        break;
      }
      case JPEG2000Compression:
      {
        (void) FormatLocaleString(buffer,MagickPathExtent,CFormat,"JPXDecode");
        if (image->colorspace != CMYKColorspace)
          break;
        (void) WriteBlobString(image,buffer);
        (void) CopyMagickString(buffer,"/Decode [1 0 1 0 1 0 1 0]\n",
          MagickPathExtent);
        break;
      }
      case LZWCompression:
      {
        (void) FormatLocaleString(buffer,MagickPathExtent,CFormat,"LZWDecode");
        break;
      }
      case ZipCompression:
      {
        (void) FormatLocaleString(buffer,MagickPathExtent,CFormat,
          "FlateDecode");
        break;
      }
      case FaxCompression:
      case Group4Compression:
      {
        (void) CopyMagickString(buffer,"/Filter [ /CCITTFaxDecode ]\n",
          MagickPathExtent);
        (void) WriteBlobString(image,buffer);
        (void) FormatLocaleString(buffer,MagickPathExtent,"/DecodeParms [ << "
          "/K %s /BlackIs1 false /Columns %.20g /Rows %.20g >> ]\n",CCITTParam,
          (double) image->columns,(double) image->rows);
        break;
      }
      default:
      {
        (void) FormatLocaleString(buffer,MagickPathExtent,CFormat,
          "RunLengthDecode");
        break;
      }
    }
    (void) WriteBlobString(image,buffer);
    (void) FormatLocaleString(buffer,MagickPathExtent,"/Width %.20g\n",(double)
      image->columns);
    (void) WriteBlobString(image,buffer);
    (void) FormatLocaleString(buffer,MagickPathExtent,"/Height %.20g\n",(double)
      image->rows);
    (void) WriteBlobString(image,buffer);
    (void) FormatLocaleString(buffer,MagickPathExtent,"/ColorSpace %.20g 0 R\n",
      (double) object+2);
    (void) WriteBlobString(image,buffer);
    (void) FormatLocaleString(buffer,MagickPathExtent,"/BitsPerComponent %d\n",
      (compression == FaxCompression) || (compression == Group4Compression) ?
      1 : 8);
    (void) WriteBlobString(image,buffer);
    if (image->alpha_trait != UndefinedPixelTrait)
      {
        (void) FormatLocaleString(buffer,MagickPathExtent,"/SMask %.20g 0 R\n",
          (double) object+(has_icc_profile != MagickFalse ? 9 : 7));
        (void) WriteBlobString(image,buffer);
      }
    (void) FormatLocaleString(buffer,MagickPathExtent,"/Length %.20g 0 R\n",
      (double) object+1);
    (void) WriteBlobString(image,buffer);
    (void) WriteBlobString(image,">>\n");
    (void) WriteBlobString(image,"stream\n");
    offset=TellBlob(image);
    number_pixels=(MagickSizeType) image->columns*image->rows;
    if ((4*number_pixels) != (MagickSizeType) ((size_t) (4*number_pixels)))
      ThrowPDFException(ResourceLimitError,"MemoryAllocationFailed");
    if ((compression == FaxCompression) || (compression == Group4Compression) ||
        ((image_info->type != TrueColorType) &&
         (SetImageGray(image,exception) != MagickFalse)))
      {
        switch (compression)
        {
          case FaxCompression:
          case Group4Compression:
          {
            if (LocaleCompare(CCITTParam,"0") == 0)
              {
                (void) HuffmanEncodeImage(image_info,image,image,exception);
                break;
              }
            (void) Huffman2DEncodeImage(image_info,image,image,exception);
            break;
          }
          case JPEGCompression:
          {
            status=InjectImageBlob(image_info,image,image,"jpeg",exception);
            if (status == MagickFalse)
              {
                xref=(MagickOffsetType *) RelinquishMagickMemory(xref);
                (void) CloseBlob(image);
                return(MagickFalse);
              }
            break;
          }
          case JPEG2000Compression:
          {
            status=InjectImageBlob(image_info,image,image,"jp2",exception);
            if (status == MagickFalse)
              {
                xref=(MagickOffsetType *) RelinquishMagickMemory(xref);
                (void) CloseBlob(image);
                return(MagickFalse);
              }
            break;
          }
          case RLECompression:
          default:
          {
            MemoryInfo
              *pixel_info;

            /*
              Allocate pixel array.
            */
            length=(size_t) number_pixels;
            pixel_info=AcquireVirtualMemory(length,sizeof(*pixels));
            if (pixel_info == (MemoryInfo *) NULL)
              ThrowPDFException(ResourceLimitError,"MemoryAllocationFailed");
            pixels=(unsigned char *) GetVirtualMemoryBlob(pixel_info);
            /*
              Dump Runlength encoded pixels.
            */
            q=pixels;
            for (y=0; y < (ssize_t) image->rows; y++)
            {
              p=GetVirtualPixels(image,0,y,image->columns,1,exception);
              if (p == (const Quantum *) NULL)
                break;
              for (x=0; x < (ssize_t) image->columns; x++)
              {
                *q++=ScaleQuantumToChar(ClampToQuantum(GetPixelLuma(image,p)));
                p+=GetPixelChannels(image);
              }
              if (image->previous == (Image *) NULL)
                {
                  status=SetImageProgress(image,SaveImageTag,(MagickOffsetType)
                    y,image->rows);
                  if (status == MagickFalse)
                    break;
                }
            }
#if defined(MAGICKCORE_ZLIB_DELEGATE)
            if (compression == ZipCompression)
              status=ZLIBEncodeImage(image,length,pixels,exception);
            else
#endif
              if (compression == LZWCompression)
                status=LZWEncodeImage(image,length,pixels,exception);
              else
                status=PackbitsEncodeImage(image,length,pixels,exception);
            pixel_info=RelinquishVirtualMemory(pixel_info);
            if (status == MagickFalse)
              {
                xref=(MagickOffsetType *) RelinquishMagickMemory(xref);
                (void) CloseBlob(image);
                return(MagickFalse);
              }
            break;
          }
          case NoCompression:
          {
            /*
              Dump uncompressed PseudoColor packets.
            */
            Ascii85Initialize(image);
            for (y=0; y < (ssize_t) image->rows; y++)
            {
              p=GetVirtualPixels(image,0,y,image->columns,1,exception);
              if (p == (const Quantum *) NULL)
                break;
              for (x=0; x < (ssize_t) image->columns; x++)
              {
                Ascii85Encode(image,ScaleQuantumToChar(ClampToQuantum(
                  GetPixelLuma(image,p))));
                p+=GetPixelChannels(image);
              }
              if (image->previous == (Image *) NULL)
                {
                  status=SetImageProgress(image,SaveImageTag,(MagickOffsetType)
                    y,image->rows);
                  if (status == MagickFalse)
                    break;
                }
            }
            Ascii85Flush(image);
            break;
          }
        }
      }
    else
      if ((image->storage_class == DirectClass) || (image->colors > 256) ||
          (compression == JPEGCompression) ||
          (compression == JPEG2000Compression))
        switch (compression)
        {
          case JPEGCompression:
          {
            status=InjectImageBlob(image_info,image,image,"jpeg",exception);
            if (status == MagickFalse)
              {
                xref=(MagickOffsetType *) RelinquishMagickMemory(xref);
                (void) CloseBlob(image);
                return(MagickFalse);
              }
            break;
          }
          case JPEG2000Compression:
          {
            status=InjectImageBlob(image_info,image,image,"jp2",exception);
            if (status == MagickFalse)
              {
                xref=(MagickOffsetType *) RelinquishMagickMemory(xref);
                (void) CloseBlob(image);
                return(MagickFalse);
              }
            break;
          }
          case RLECompression:
          default:
          {
            MemoryInfo
              *pixel_info;

            /*
              Allocate pixel array.
            */
            length=(size_t) number_pixels;
            length*=image->colorspace == CMYKColorspace ? 4UL : 3UL;
            pixel_info=AcquireVirtualMemory(length,sizeof(*pixels));
            if (pixel_info == (MemoryInfo *) NULL)
              ThrowPDFException(ResourceLimitError,"MemoryAllocationFailed");
            pixels=(unsigned char *) GetVirtualMemoryBlob(pixel_info);
            /*
              Dump runoffset encoded pixels.
            */
            q=pixels;
            for (y=0; y < (ssize_t) image->rows; y++)
            {
              p=GetVirtualPixels(image,0,y,image->columns,1,exception);
              if (p == (const Quantum *) NULL)
                break;
              for (x=0; x < (ssize_t) image->columns; x++)
              {
                *q++=ScaleQuantumToChar(GetPixelRed(image,p));
                *q++=ScaleQuantumToChar(GetPixelGreen(image,p));
                *q++=ScaleQuantumToChar(GetPixelBlue(image,p));
                if (image->colorspace == CMYKColorspace)
                  *q++=ScaleQuantumToChar(GetPixelBlack(image,p));
                p+=GetPixelChannels(image);
              }
              if (image->previous == (Image *) NULL)
                {
                  status=SetImageProgress(image,SaveImageTag,(MagickOffsetType)
                    y,image->rows);
                  if (status == MagickFalse)
                    break;
                }
            }
#if defined(MAGICKCORE_ZLIB_DELEGATE)
            if (compression == ZipCompression)
              status=ZLIBEncodeImage(image,length,pixels,exception);
            else
#endif
              if (compression == LZWCompression)
                status=LZWEncodeImage(image,length,pixels,exception);
              else
                status=PackbitsEncodeImage(image,length,pixels,exception);
            pixel_info=RelinquishVirtualMemory(pixel_info);
            if (status == MagickFalse)
              {
                xref=(MagickOffsetType *) RelinquishMagickMemory(xref);
                (void) CloseBlob(image);
                return(MagickFalse);
              }
            break;
          }
          case NoCompression:
          {
            /*
              Dump uncompressed DirectColor packets.
            */
            Ascii85Initialize(image);
            for (y=0; y < (ssize_t) image->rows; y++)
            {
              p=GetVirtualPixels(image,0,y,image->columns,1,exception);
              if (p == (const Quantum *) NULL)
                break;
              for (x=0; x < (ssize_t) image->columns; x++)
              {
                Ascii85Encode(image,ScaleQuantumToChar(GetPixelRed(image,p)));
                Ascii85Encode(image,ScaleQuantumToChar(GetPixelGreen(image,p)));
                Ascii85Encode(image,ScaleQuantumToChar(GetPixelBlue(image,p)));
                if (image->colorspace == CMYKColorspace)
                  Ascii85Encode(image,ScaleQuantumToChar(
                    GetPixelBlack(image,p)));
                p+=GetPixelChannels(image);
              }
              if (image->previous == (Image *) NULL)
                {
                  status=SetImageProgress(image,SaveImageTag,(MagickOffsetType)
                    y,image->rows);
                  if (status == MagickFalse)
                    break;
                }
            }
            Ascii85Flush(image);
            break;
          }
        }
      else
        {
          /*
            Dump number of colors and colormap.
          */
          switch (compression)
          {
            case RLECompression:
            default:
            {
              MemoryInfo
                *pixel_info;

              /*
                Allocate pixel array.
              */
              length=(size_t) number_pixels;
              pixel_info=AcquireVirtualMemory(length,sizeof(*pixels));
              if (pixel_info == (MemoryInfo *) NULL)
                ThrowPDFException(ResourceLimitError,"MemoryAllocationFailed");
              pixels=(unsigned char *) GetVirtualMemoryBlob(pixel_info);
              /*
                Dump Runlength encoded pixels.
              */
              q=pixels;
              for (y=0; y < (ssize_t) image->rows; y++)
              {
                p=GetVirtualPixels(image,0,y,image->columns,1,exception);
                if (p == (const Quantum *) NULL)
                  break;
                for (x=0; x < (ssize_t) image->columns; x++)
                {
                  *q++=(unsigned char) GetPixelIndex(image,p);
                  p+=GetPixelChannels(image);
                }
                if (image->previous == (Image *) NULL)
                  {
                    status=SetImageProgress(image,SaveImageTag,
                      (MagickOffsetType) y,image->rows);
                    if (status == MagickFalse)
                      break;
                  }
              }
#if defined(MAGICKCORE_ZLIB_DELEGATE)
              if (compression == ZipCompression)
                status=ZLIBEncodeImage(image,length,pixels,exception);
              else
#endif
                if (compression == LZWCompression)
                  status=LZWEncodeImage(image,length,pixels,exception);
                else
                  status=PackbitsEncodeImage(image,length,pixels,exception);
              pixel_info=RelinquishVirtualMemory(pixel_info);
              if (status == MagickFalse)
                {
                  xref=(MagickOffsetType *) RelinquishMagickMemory(xref);
                  (void) CloseBlob(image);
                  return(MagickFalse);
                }
              break;
            }
            case NoCompression:
            {
              /*
                Dump uncompressed PseudoColor packets.
              */
              Ascii85Initialize(image);
              for (y=0; y < (ssize_t) image->rows; y++)
              {
                p=GetVirtualPixels(image,0,y,image->columns,1,exception);
                if (p == (const Quantum *) NULL)
                  break;
                for (x=0; x < (ssize_t) image->columns; x++)
                {
                  Ascii85Encode(image,(unsigned char) GetPixelIndex(image,p));
                  p+=GetPixelChannels(image);
                }
                if (image->previous == (Image *) NULL)
                  {
                    status=SetImageProgress(image,SaveImageTag,
                      (MagickOffsetType) y,image->rows);
                    if (status == MagickFalse)
                      break;
                  }
              }
              Ascii85Flush(image);
              break;
            }
          }
        }
    offset=TellBlob(image)-offset;
    (void) WriteBlobString(image,"\nendstream\n");
    (void) WriteBlobString(image,"endobj\n");
    /*
      Write Length object.
    */
    xref[object++]=TellBlob(image);
    (void) FormatLocaleString(buffer,MagickPathExtent,"%.20g 0 obj\n",(double)
      object);
    (void) WriteBlobString(image,buffer);
    (void) FormatLocaleString(buffer,MagickPathExtent,"%.20g\n",(double)
      offset);
    (void) WriteBlobString(image,buffer);
    (void) WriteBlobString(image,"endobj\n");
    /*
      Write Colorspace object.
    */
    xref[object++]=TellBlob(image);
    (void) FormatLocaleString(buffer,MagickPathExtent,"%.20g 0 obj\n",(double)
      object);
    (void) WriteBlobString(image,buffer);
    device="DeviceRGB";
    channels=0;
    if (image->colorspace == CMYKColorspace)
      {
        device="DeviceCMYK";
        channels=4;
      }
    else
      if ((compression == FaxCompression) ||
          (compression == Group4Compression) ||
          ((image_info->type != TrueColorType) &&
           (SetImageGray(image,exception) != MagickFalse)))
        {
          device="DeviceGray";
          channels=1;
        }
      else
        if ((image->storage_class == DirectClass) ||
            (image->colors > 256) || (compression == JPEGCompression) ||
            (compression == JPEG2000Compression))
          {
            device="DeviceRGB";
            channels=3;
          }
    profile=GetImageProfile(image,"icc");
    if ((profile == (StringInfo *) NULL) || (channels == 0))
      {
        if (channels != 0)
          (void) FormatLocaleString(buffer,MagickPathExtent,"/%s\n",device);
        else
          (void) FormatLocaleString(buffer,MagickPathExtent,
            "[ /Indexed /%s %.20g %.20g 0 R ]\n",device,(double) image->colors-
            1,(double) object+3);
        (void) WriteBlobString(image,buffer);
      }
    else
      {
        const unsigned char
          *p;

        /*
          Write ICC profile. 
        */
        (void) FormatLocaleString(buffer,MagickPathExtent,
          "[/ICCBased %.20g 0 R]\n",(double) object+1);
        (void) WriteBlobString(image,buffer);
        (void) WriteBlobString(image,"endobj\n");
        xref[object++]=TellBlob(image);
        (void) FormatLocaleString(buffer,MagickPathExtent,"%.20g 0 obj\n",
          (double) object);
        (void) WriteBlobString(image,buffer);
        (void) FormatLocaleString(buffer,MagickPathExtent,"<<\n/N %.20g\n"
          "/Filter /ASCII85Decode\n/Length %.20g 0 R\n/Alternate /%s\n>>\n"
          "stream\n",(double) channels,(double) object+1,device);
        (void) WriteBlobString(image,buffer);
        offset=TellBlob(image);
        Ascii85Initialize(image);
        p=GetStringInfoDatum(profile);
        for (i=0; i < (ssize_t) GetStringInfoLength(profile); i++)
          Ascii85Encode(image,(unsigned char) *p++);
        Ascii85Flush(image);
        offset=TellBlob(image)-offset;
        (void) WriteBlobString(image,"endstream\n");
        (void) WriteBlobString(image,"endobj\n");
        /*
          Write Length object.
        */
        xref[object++]=TellBlob(image);
        (void) FormatLocaleString(buffer,MagickPathExtent,"%.20g 0 obj\n",
          (double) object);
        (void) WriteBlobString(image,buffer);
        (void) FormatLocaleString(buffer,MagickPathExtent,"%.20g\n",(double)
          offset);
        (void) WriteBlobString(image,buffer);
      }
    (void) WriteBlobString(image,"endobj\n");
    /*
      Write Thumb object.
    */
    SetGeometry(image,&geometry);
    (void) ParseMetaGeometry("106x106+0+0>",&geometry.x,&geometry.y,
      &geometry.width,&geometry.height);
    tile_image=ThumbnailImage(image,geometry.width,geometry.height,exception);
    if (tile_image == (Image *) NULL)
      return(MagickFalse);
    xref[object++]=TellBlob(image);
    (void) FormatLocaleString(buffer,MagickPathExtent,"%.20g 0 obj\n",(double)
      object);
    (void) WriteBlobString(image,buffer);
    (void) WriteBlobString(image,"<<\n");
    switch (compression)
    {
      case NoCompression:
      {
        (void) FormatLocaleString(buffer,MagickPathExtent,CFormat,
          "ASCII85Decode");
        break;
      }
      case JPEGCompression:
      {
        (void) FormatLocaleString(buffer,MagickPathExtent,CFormat,"DCTDecode");
        if (image->colorspace != CMYKColorspace)
          break;
        (void) WriteBlobString(image,buffer);
        (void) CopyMagickString(buffer,"/Decode [1 0 1 0 1 0 1 0]\n",
          MagickPathExtent);
        break;
      }
      case JPEG2000Compression:
      {
        (void) FormatLocaleString(buffer,MagickPathExtent,CFormat,"JPXDecode");
        if (image->colorspace != CMYKColorspace)
          break;
        (void) WriteBlobString(image,buffer);
        (void) CopyMagickString(buffer,"/Decode [1 0 1 0 1 0 1 0]\n",
          MagickPathExtent);
        break;
      }
      case LZWCompression:
      {
        (void) FormatLocaleString(buffer,MagickPathExtent,CFormat,"LZWDecode");
        break;
      }
      case ZipCompression:
      {
        (void) FormatLocaleString(buffer,MagickPathExtent,CFormat,
          "FlateDecode");
        break;
      }
      case FaxCompression:
      case Group4Compression:
      {
        (void) CopyMagickString(buffer,"/Filter [ /CCITTFaxDecode ]\n",
          MagickPathExtent);
        (void) WriteBlobString(image,buffer);
        (void) FormatLocaleString(buffer,MagickPathExtent,"/DecodeParms [ << "
          "/K %s /BlackIs1 false /Columns %.20g /Rows %.20g >> ]\n",CCITTParam,
          (double) tile_image->columns,(double) tile_image->rows);
        break;
      }
      default:
      {
        (void) FormatLocaleString(buffer,MagickPathExtent,CFormat,
          "RunLengthDecode");
        break;
      }
    }
    (void) WriteBlobString(image,buffer);
    (void) FormatLocaleString(buffer,MagickPathExtent,"/Width %.20g\n",(double)
      tile_image->columns);
    (void) WriteBlobString(image,buffer);
    (void) FormatLocaleString(buffer,MagickPathExtent,"/Height %.20g\n",(double)
      tile_image->rows);
    (void) WriteBlobString(image,buffer);
    (void) FormatLocaleString(buffer,MagickPathExtent,"/ColorSpace %.20g 0 R\n",
      (double) object-(has_icc_profile != MagickFalse ? 3 : 1));
    (void) WriteBlobString(image,buffer);
    (void) FormatLocaleString(buffer,MagickPathExtent,"/BitsPerComponent %d\n",
      (compression == FaxCompression) || (compression == Group4Compression) ?
      1 : 8);
    (void) WriteBlobString(image,buffer);
    (void) FormatLocaleString(buffer,MagickPathExtent,"/Length %.20g 0 R\n",
      (double) object+1);
    (void) WriteBlobString(image,buffer);
    (void) WriteBlobString(image,">>\n");
    (void) WriteBlobString(image,"stream\n");
    offset=TellBlob(image);
    number_pixels=(MagickSizeType) tile_image->columns*tile_image->rows;
    if ((compression == FaxCompression) ||
        (compression == Group4Compression) ||
        ((image_info->type != TrueColorType) &&
         (SetImageGray(tile_image,exception) != MagickFalse)))
      {
        switch (compression)
        {
          case FaxCompression:
          case Group4Compression:
          {
            if (LocaleCompare(CCITTParam,"0") == 0)
              {
                (void) HuffmanEncodeImage(image_info,image,tile_image,
                  exception);
                break;
              }
            (void) Huffman2DEncodeImage(image_info,image,tile_image,exception);
            break;
          }
          case JPEGCompression:
          {
            status=InjectImageBlob(image_info,image,tile_image,"jpeg",
              exception);
            if (status == MagickFalse)
              {
                xref=(MagickOffsetType *) RelinquishMagickMemory(xref);
                (void) CloseBlob(image);
                return(MagickFalse);
              }
            break;
          }
          case JPEG2000Compression:
          {
            status=InjectImageBlob(image_info,image,tile_image,"jp2",exception);
            if (status == MagickFalse)
              {
                xref=(MagickOffsetType *) RelinquishMagickMemory(xref);
                (void) CloseBlob(image);
                return(MagickFalse);
              }
            break;
          }
          case RLECompression:
          default:
          {
            MemoryInfo
              *pixel_info;

            /*
              Allocate pixel array.
            */
            length=(size_t) number_pixels;
            pixel_info=AcquireVirtualMemory(length,sizeof(*pixels));
            if (pixel_info == (MemoryInfo *) NULL)
              {
                tile_image=DestroyImage(tile_image);
                ThrowPDFException(ResourceLimitError,"MemoryAllocationFailed");
              }
            pixels=(unsigned char *) GetVirtualMemoryBlob(pixel_info);
            /*
              Dump runlength encoded pixels.
            */
            q=pixels;
            for (y=0; y < (ssize_t) tile_image->rows; y++)
            {
              p=GetVirtualPixels(tile_image,0,y,tile_image->columns,1,
                exception);
              if (p == (const Quantum *) NULL)
                break;
              for (x=0; x < (ssize_t) tile_image->columns; x++)
              {
                *q++=ScaleQuantumToChar(ClampToQuantum(GetPixelLuma(
                  tile_image,p)));
                p+=GetPixelChannels(tile_image);
              }
            }
#if defined(MAGICKCORE_ZLIB_DELEGATE)
            if (compression == ZipCompression)
              status=ZLIBEncodeImage(image,length,pixels,exception);
            else
#endif
              if (compression == LZWCompression)
                status=LZWEncodeImage(image,length,pixels,exception);
              else
                status=PackbitsEncodeImage(image,length,pixels,exception);
            pixel_info=RelinquishVirtualMemory(pixel_info);
            if (status == MagickFalse)
              {
                xref=(MagickOffsetType *) RelinquishMagickMemory(xref);
                (void) CloseBlob(image);
                return(MagickFalse);
              }
            break;
          }
          case NoCompression:
          {
            /*
              Dump uncompressed PseudoColor packets.
            */
            Ascii85Initialize(image);
            for (y=0; y < (ssize_t) tile_image->rows; y++)
            {
              p=GetVirtualPixels(tile_image,0,y,tile_image->columns,1,
                exception);
              if (p == (const Quantum *) NULL)
                break;
              for (x=0; x < (ssize_t) tile_image->columns; x++)
              {
                Ascii85Encode(image,ScaleQuantumToChar(ClampToQuantum(
                  GetPixelLuma(tile_image,p))));
                p+=GetPixelChannels(tile_image);
              }
            }
            Ascii85Flush(image);
            break;
          }
        }
      }
    else
      if ((tile_image->storage_class == DirectClass) ||
          (tile_image->colors > 256) || (compression == JPEGCompression) ||
          (compression == JPEG2000Compression))
        switch (compression)
        {
          case JPEGCompression:
          {
            status=InjectImageBlob(image_info,image,tile_image,"jpeg",
              exception);
            if (status == MagickFalse)
              {
                xref=(MagickOffsetType *) RelinquishMagickMemory(xref);
                (void) CloseBlob(image);
                return(MagickFalse);
              }
            break;
          }
          case JPEG2000Compression:
          {
            status=InjectImageBlob(image_info,image,tile_image,"jp2",exception);
            if (status == MagickFalse)
              {
                xref=(MagickOffsetType *) RelinquishMagickMemory(xref);
                (void) CloseBlob(image);
                return(MagickFalse);
              }
            break;
          }
          case RLECompression:
          default:
          {
            MemoryInfo
              *pixel_info;

            /*
              Allocate pixel array.
            */
            length=(size_t) number_pixels;
            length*=tile_image->colorspace == CMYKColorspace ? 4UL : 3UL;
            pixel_info=AcquireVirtualMemory(length,4*sizeof(*pixels));
            if (pixel_info == (MemoryInfo *) NULL)
              {
                tile_image=DestroyImage(tile_image);
                ThrowPDFException(ResourceLimitError,"MemoryAllocationFailed");
              }
            pixels=(unsigned char *) GetVirtualMemoryBlob(pixel_info);
            /*
              Dump runlength encoded pixels.
            */
            q=pixels;
            for (y=0; y < (ssize_t) tile_image->rows; y++)
            {
              p=GetVirtualPixels(tile_image,0,y,tile_image->columns,1,
                exception);
              if (p == (const Quantum *) NULL)
                break;
              for (x=0; x < (ssize_t) tile_image->columns; x++)
              {
                *q++=ScaleQuantumToChar(GetPixelRed(tile_image,p));
                *q++=ScaleQuantumToChar(GetPixelGreen(tile_image,p));
                *q++=ScaleQuantumToChar(GetPixelBlue(tile_image,p));
                if (tile_image->colorspace == CMYKColorspace)
                  *q++=ScaleQuantumToChar(GetPixelBlack(tile_image,p));
                p+=GetPixelChannels(tile_image);
              }
            }
#if defined(MAGICKCORE_ZLIB_DELEGATE)
            if (compression == ZipCompression)
              status=ZLIBEncodeImage(image,length,pixels,exception);
            else
#endif
              if (compression == LZWCompression)
                status=LZWEncodeImage(image,length,pixels,exception);
              else
                status=PackbitsEncodeImage(image,length,pixels,exception);
            pixel_info=RelinquishVirtualMemory(pixel_info);
            if (status == MagickFalse)
              {
                xref=(MagickOffsetType *) RelinquishMagickMemory(xref);
                (void) CloseBlob(image);
                return(MagickFalse);
              }
            break;
          }
          case NoCompression:
          {
            /*
              Dump uncompressed DirectColor packets.
            */
            Ascii85Initialize(image);
            for (y=0; y < (ssize_t) tile_image->rows; y++)
            {
              p=GetVirtualPixels(tile_image,0,y,tile_image->columns,1,
                exception);
              if (p == (const Quantum *) NULL)
                break;
              for (x=0; x < (ssize_t) tile_image->columns; x++)
              {
                Ascii85Encode(image,ScaleQuantumToChar(
                  GetPixelRed(tile_image,p)));
                Ascii85Encode(image,ScaleQuantumToChar(
                  GetPixelGreen(tile_image,p)));
                Ascii85Encode(image,ScaleQuantumToChar(
                  GetPixelBlue(tile_image,p)));
                if (image->colorspace == CMYKColorspace)
                  Ascii85Encode(image,ScaleQuantumToChar(
                    GetPixelBlack(tile_image,p)));
                p+=GetPixelChannels(tile_image);
              }
            }
            Ascii85Flush(image);
            break;
          }
        }
      else
        {
          /*
            Dump number of colors and colormap.
          */
          switch (compression)
          {
            case RLECompression:
            default:
            {
              MemoryInfo
                *pixel_info;

              /*
                Allocate pixel array.
              */
              length=(size_t) number_pixels;
              pixel_info=AcquireVirtualMemory(length,sizeof(*pixels));
              if (pixel_info == (MemoryInfo *) NULL)
                {
                  tile_image=DestroyImage(tile_image);
                  ThrowPDFException(ResourceLimitError,
                    "MemoryAllocationFailed");
                }
              pixels=(unsigned char *) GetVirtualMemoryBlob(pixel_info);
              /*
                Dump runlength encoded pixels.
              */
              q=pixels;
              for (y=0; y < (ssize_t) tile_image->rows; y++)
              {
                p=GetVirtualPixels(tile_image,0,y,tile_image->columns,1,
                  exception);
                if (p == (const Quantum *) NULL)
                  break;
                for (x=0; x < (ssize_t) tile_image->columns; x++)
                {
                  *q++=(unsigned char) GetPixelIndex(tile_image,p);
                  p+=GetPixelChannels(tile_image);
                }
              }
#if defined(MAGICKCORE_ZLIB_DELEGATE)
              if (compression == ZipCompression)
                status=ZLIBEncodeImage(image,length,pixels,exception);
              else
#endif
                if (compression == LZWCompression)
                  status=LZWEncodeImage(image,length,pixels,exception);
                else
                  status=PackbitsEncodeImage(image,length,pixels,exception);
              pixel_info=RelinquishVirtualMemory(pixel_info);
              if (status == MagickFalse)
                {
                  xref=(MagickOffsetType *) RelinquishMagickMemory(xref);
                  (void) CloseBlob(image);
                  return(MagickFalse);
                }
              break;
            }
            case NoCompression:
            {
              /*
                Dump uncompressed PseudoColor packets.
              */
              Ascii85Initialize(image);
              for (y=0; y < (ssize_t) tile_image->rows; y++)
              {
                p=GetVirtualPixels(tile_image,0,y,tile_image->columns,1,
                  exception);
                if (p == (const Quantum *) NULL)
                  break;
                for (x=0; x < (ssize_t) tile_image->columns; x++)
                {
                  Ascii85Encode(image,(unsigned char)
                    GetPixelIndex(tile_image,p));
                  p+=GetPixelChannels(image);
                }
              }
              Ascii85Flush(image);
              break;
            }
          }
        }
    tile_image=DestroyImage(tile_image);
    offset=TellBlob(image)-offset;
    (void) WriteBlobString(image,"\nendstream\n");
    (void) WriteBlobString(image,"endobj\n");
    /*
      Write Length object.
    */
    xref[object++]=TellBlob(image);
    (void) FormatLocaleString(buffer,MagickPathExtent,"%.20g 0 obj\n",(double)
      object);
    (void) WriteBlobString(image,buffer);
    (void) FormatLocaleString(buffer,MagickPathExtent,"%.20g\n",(double)
      offset);
    (void) WriteBlobString(image,buffer);
    (void) WriteBlobString(image,"endobj\n");
    xref[object++]=TellBlob(image);
    (void) FormatLocaleString(buffer,MagickPathExtent,"%.20g 0 obj\n",(double)
      object);
    (void) WriteBlobString(image,buffer);
    (void) WriteBlobString(image,"<<\n");
    if ((image->storage_class == DirectClass) || (image->colors > 256) ||
        (compression == FaxCompression) || (compression == Group4Compression))
      (void) WriteBlobString(image,">>\n");
    else
      {
        /*
          Write Colormap object.
        */
        if (compression == NoCompression)
          (void) WriteBlobString(image,"/Filter [ /ASCII85Decode ]\n");
        (void) FormatLocaleString(buffer,MagickPathExtent,"/Length %.20g 0 R\n",
          (double) object+1);
        (void) WriteBlobString(image,buffer);
        (void) WriteBlobString(image,">>\n");
        (void) WriteBlobString(image,"stream\n");
        offset=TellBlob(image);
        if (compression == NoCompression)
          Ascii85Initialize(image);
        for (i=0; i < (ssize_t) image->colors; i++)
        {
          if (compression == NoCompression)
            {
              Ascii85Encode(image,ScaleQuantumToChar(ClampToQuantum(
                image->colormap[i].red)));
              Ascii85Encode(image,ScaleQuantumToChar(ClampToQuantum(
                image->colormap[i].green)));
              Ascii85Encode(image,ScaleQuantumToChar(ClampToQuantum(
                image->colormap[i].blue)));
              continue;
            }
          (void) WriteBlobByte(image,ScaleQuantumToChar(
             ClampToQuantum(image->colormap[i].red)));
          (void) WriteBlobByte(image,ScaleQuantumToChar(
             ClampToQuantum(image->colormap[i].green)));
          (void) WriteBlobByte(image,ScaleQuantumToChar(
             ClampToQuantum(image->colormap[i].blue)));
        }
        if (compression == NoCompression)
          Ascii85Flush(image);
       offset=TellBlob(image)-offset;
       (void) WriteBlobString(image,"\nendstream\n");
      }
    (void) WriteBlobString(image,"endobj\n");
    /*
      Write Length object.
    */
    xref[object++]=TellBlob(image);
    (void) FormatLocaleString(buffer,MagickPathExtent,"%.20g 0 obj\n",(double)
      object);
    (void) WriteBlobString(image,buffer);
    (void) FormatLocaleString(buffer,MagickPathExtent,"%.20g\n",(double)
      offset);
    (void) WriteBlobString(image,buffer);
    (void) WriteBlobString(image,"endobj\n");
    /*
      Write softmask object.
    */
    xref[object++]=TellBlob(image);
    (void) FormatLocaleString(buffer,MagickPathExtent,"%.20g 0 obj\n",(double)
      object);
    (void) WriteBlobString(image,buffer);
    (void) WriteBlobString(image,"<<\n");
    if (image->alpha_trait == UndefinedPixelTrait)
      (void) WriteBlobString(image,">>\n");
    else
      {
        (void) WriteBlobString(image,"/Type /XObject\n");
        (void) WriteBlobString(image,"/Subtype /Image\n");
        (void) FormatLocaleString(buffer,MagickPathExtent,"/Name /Ma%.20g\n",
          (double) image->scene);
        (void) WriteBlobString(image,buffer);
        switch (compression)
        {
          case NoCompression:
          {
            (void) FormatLocaleString(buffer,MagickPathExtent,CFormat,
              "ASCII85Decode");
            break;
          }
          case LZWCompression:
          {
            (void) FormatLocaleString(buffer,MagickPathExtent,CFormat,
              "LZWDecode");
            break;
          }
          case ZipCompression:
          {
            (void) FormatLocaleString(buffer,MagickPathExtent,CFormat,
              "FlateDecode");
            break;
          }
          default:
          {
            (void) FormatLocaleString(buffer,MagickPathExtent,CFormat,
              "RunLengthDecode");
            break;
          }
        }
        (void) WriteBlobString(image,buffer);
        (void) FormatLocaleString(buffer,MagickPathExtent,"/Width %.20g\n",
          (double) image->columns);
        (void) WriteBlobString(image,buffer);
        (void) FormatLocaleString(buffer,MagickPathExtent,"/Height %.20g\n",
          (double) image->rows);
        (void) WriteBlobString(image,buffer);
        (void) WriteBlobString(image,"/ColorSpace /DeviceGray\n");
        (void) FormatLocaleString(buffer,MagickPathExtent,
          "/BitsPerComponent %d\n",(compression == FaxCompression) ||
          (compression == Group4Compression) ? 1 : 8);
        (void) WriteBlobString(image,buffer);
        (void) FormatLocaleString(buffer,MagickPathExtent,"/Length %.20g 0 R\n",
          (double) object+1);
        (void) WriteBlobString(image,buffer);
        (void) WriteBlobString(image,">>\n");
        (void) WriteBlobString(image,"stream\n");
        offset=TellBlob(image);
        number_pixels=(MagickSizeType) image->columns*image->rows;
        switch (compression)
        {
          case RLECompression:
          default:
          {
            MemoryInfo
              *pixel_info;

            /*
              Allocate pixel array.
            */
            length=(size_t) number_pixels;
            pixel_info=AcquireVirtualMemory(length,4*sizeof(*pixels));
            if (pixel_info == (MemoryInfo *) NULL)
              {
                image=DestroyImage(image);
                ThrowPDFException(ResourceLimitError,"MemoryAllocationFailed");
              }
            pixels=(unsigned char *) GetVirtualMemoryBlob(pixel_info);
            /*
              Dump Runlength encoded pixels.
            */
            q=pixels;
            for (y=0; y < (ssize_t) image->rows; y++)
            {
              p=GetVirtualPixels(image,0,y,image->columns,1,exception);
              if (p == (const Quantum *) NULL)
                break;
              for (x=0; x < (ssize_t) image->columns; x++)
              {
                *q++=ScaleQuantumToChar(GetPixelAlpha(image,p));
                p+=GetPixelChannels(image);
              }
            }
#if defined(MAGICKCORE_ZLIB_DELEGATE)
            if (compression == ZipCompression)
              status=ZLIBEncodeImage(image,length,pixels,exception);
            else
#endif
              if (compression == LZWCompression)
                status=LZWEncodeImage(image,length,pixels,exception);
              else
                status=PackbitsEncodeImage(image,length,pixels,exception);
            pixel_info=RelinquishVirtualMemory(pixel_info);
            if (status == MagickFalse)
              {
                xref=(MagickOffsetType *) RelinquishMagickMemory(xref);
                (void) CloseBlob(image);
                return(MagickFalse);
              }
            break;
          }
          case NoCompression:
          {
            /*
              Dump uncompressed PseudoColor packets.
            */
            Ascii85Initialize(image);
            for (y=0; y < (ssize_t) image->rows; y++)
            {
              p=GetVirtualPixels(image,0,y,image->columns,1,exception);
              if (p == (const Quantum *) NULL)
                break;
              for (x=0; x < (ssize_t) image->columns; x++)
              {
                Ascii85Encode(image,ScaleQuantumToChar(GetPixelAlpha(image,p)));
                p+=GetPixelChannels(image);
              }
            }
            Ascii85Flush(image);
            break;
          }
        }
        offset=TellBlob(image)-offset;
        (void) WriteBlobString(image,"\nendstream\n");
      }
    (void) WriteBlobString(image,"endobj\n");
    /*
      Write Length object.
    */
    xref[object++]=TellBlob(image);
    (void) FormatLocaleString(buffer,MagickPathExtent,"%.20g 0 obj\n",(double)
      object);
    (void) WriteBlobString(image,buffer);
    (void) FormatLocaleString(buffer,MagickPathExtent,"%.20g\n",(double)
      offset);
    (void) WriteBlobString(image,buffer);
    (void) WriteBlobString(image,"endobj\n");
    if (GetNextImageInList(image) == (Image *) NULL)
      break;
    image=SyncNextImageInList(image);
    status=SetImageProgress(image,SaveImagesTag,scene++,imageListLength);
    if (status == MagickFalse)
      break;
  } while (image_info->adjoin != MagickFalse);
  /*
    Write Metadata object.
  */
  xref[object++]=TellBlob(image);
  info_id=object;
  (void) FormatLocaleString(buffer,MagickPathExtent,"%.20g 0 obj\n",(double)
    object);
  (void) WriteBlobString(image,buffer);
  (void) WriteBlobString(image,"<<\n");
  if (LocaleCompare(image_info->magick,"PDFA") == 0)
    (void) FormatLocaleString(buffer,MagickPathExtent,"/Title (%s)\n",
      EscapeParenthesis(basename));
  else
    {
      wchar_t
        *utf16;

      utf16=ConvertUTF8ToUTF16((unsigned char *) basename,&length);
      if (utf16 != (wchar_t *) NULL)
        {
          unsigned char
            hex_digits[16];

          hex_digits[0]='0';
          hex_digits[1]='1';
          hex_digits[2]='2';
          hex_digits[3]='3';
          hex_digits[4]='4';
          hex_digits[5]='5';
          hex_digits[6]='6';
          hex_digits[7]='7';
          hex_digits[8]='8';
          hex_digits[9]='9';
          hex_digits[10]='A';
          hex_digits[11]='B';
          hex_digits[12]='C';
          hex_digits[13]='D';
          hex_digits[14]='E';
          hex_digits[15]='F';
          (void) FormatLocaleString(buffer,MagickPathExtent,"/Title <FEFF");
          (void) WriteBlobString(image,buffer);
          for (i=0; i < (ssize_t) length; i++)
          {
            (void) WriteBlobByte(image,'0');
            (void) WriteBlobByte(image,'0');
            (void) WriteBlobByte(image,hex_digits[(utf16[i] >> 4) & 0x0f]);
            (void) WriteBlobByte(image,hex_digits[utf16[i] & 0x0f]);
          }
          (void) FormatLocaleString(buffer,MagickPathExtent,">\n");
          utf16=(wchar_t *) RelinquishMagickMemory(utf16);
        }
    }
  (void) WriteBlobString(image,buffer);
  seconds=GetMagickTime();
  GetMagickUTCtime(&seconds,&utc_time);
  (void) FormatLocaleString(date,MagickPathExtent,"D:%04d%02d%02d%02d%02d%02d",
    utc_time.tm_year+1900,utc_time.tm_mon+1,utc_time.tm_mday,
    utc_time.tm_hour,utc_time.tm_min,utc_time.tm_sec);
  (void) FormatLocaleString(buffer,MagickPathExtent,"/CreationDate (%s)\n",
    date);
  (void) WriteBlobString(image,buffer);
  (void) FormatLocaleString(buffer,MagickPathExtent,"/ModDate (%s)\n",date);
  (void) WriteBlobString(image,buffer);
  url=(char *) MagickAuthoritativeURL;
  escape=EscapeParenthesis(url);
  (void) FormatLocaleString(buffer,MagickPathExtent,"/Producer (%s)\n",escape);
  escape=DestroyString(escape);
  (void) WriteBlobString(image,buffer);
  (void) WriteBlobString(image,">>\n");
  (void) WriteBlobString(image,"endobj\n");
  /*
    Write Xref object.
  */
  offset=TellBlob(image)-xref[0]+
   (LocaleCompare(image_info->magick,"PDFA") == 0 ? 6 : 0)+10;
  (void) WriteBlobString(image,"xref\n");
  (void) FormatLocaleString(buffer,MagickPathExtent,"0 %.20g\n",(double)
    object+1);
  (void) WriteBlobString(image,buffer);
  (void) WriteBlobString(image,"0000000000 65535 f \n");
  for (i=0; i < (ssize_t) object; i++)
  {
    (void) FormatLocaleString(buffer,MagickPathExtent,"%010lu 00000 n \n",
      (unsigned long) xref[i]);
    (void) WriteBlobString(image,buffer);
  }
  (void) WriteBlobString(image,"trailer\n");
  (void) WriteBlobString(image,"<<\n");
  (void) FormatLocaleString(buffer,MagickPathExtent,"/Size %.20g\n",(double)
    object+1);
  (void) WriteBlobString(image,buffer);
  (void) FormatLocaleString(buffer,MagickPathExtent,"/Info %.20g 0 R\n",(double)
    info_id);
  (void) WriteBlobString(image,buffer);
  (void) FormatLocaleString(buffer,MagickPathExtent,"/Root %.20g 0 R\n",(double)
    root_id);
  (void) WriteBlobString(image,buffer);
  (void) SignatureImage(image,exception);
  (void) FormatLocaleString(buffer,MagickPathExtent,"/ID [<%s> <%s>]\n",
    GetImageProperty(image,"signature",exception),
    GetImageProperty(image,"signature",exception));
  (void) WriteBlobString(image,buffer);
  (void) WriteBlobString(image,">>\n");
  (void) WriteBlobString(image,"startxref\n");
  (void) FormatLocaleString(buffer,MagickPathExtent,"%.20g\n",(double) offset);
  (void) WriteBlobString(image,buffer);
  (void) WriteBlobString(image,"%%EOF\n");
  xref=(MagickOffsetType *) RelinquishMagickMemory(xref);
  (void) CloseBlob(image);
  return(MagickTrue);
}