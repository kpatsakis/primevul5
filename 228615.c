static Image *ReadPDFImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
#define BeginXMPPacket  "<?xpacket begin="
#define CMYKProcessColor  "CMYKProcessColor"
#define CropBox  "CropBox"
#define DefaultCMYK  "DefaultCMYK"
#define DeviceCMYK  "DeviceCMYK"
#define EndXMPPacket  "<?xpacket end="
#define MediaBox  "MediaBox"
#define RenderPostscriptText  "Rendering Postscript...  "
#define PDFRotate  "Rotate"
#define SpotColor  "Separation"
#define TrimBox  "TrimBox"
#define PDFVersion  "PDF-"

  char
    command[MagickPathExtent],
    *density,
    filename[MagickPathExtent],
    geometry[MagickPathExtent],
    input_filename[MagickPathExtent],
    message[MagickPathExtent],
    *options,
    postscript_filename[MagickPathExtent];

  const char
    *option;

  const DelegateInfo
    *delegate_info;

  double
    angle;

  GeometryInfo
    geometry_info;

  Image
    *image,
    *next,
    *pdf_image;

  ImageInfo
    *read_info;

  int
    c,
    file;

  MagickBooleanType
    cmyk,
    cropbox,
    fitPage,
    status,
    stop_on_error,
    trimbox;

  MagickStatusType
    flags;

  PointInfo
    delta;

  RectangleInfo
    bounding_box,
    page;

  register char
    *p;

  register ssize_t
    i;

  SegmentInfo
    bounds,
    hires_bounds;

  size_t
    scene,
    spotcolor;

  ssize_t
    count;

  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  if (image_info->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      image_info->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  /*
    Open image file.
  */
  image=AcquireImage(image_info,exception);
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == MagickFalse)
    {
      image=DestroyImageList(image);
      return((Image *) NULL);
    }
  status=AcquireUniqueSymbolicLink(image_info->filename,input_filename);
  if (status == MagickFalse)
    {
      ThrowFileException(exception,FileOpenError,"UnableToCreateTemporaryFile",
        image_info->filename);
      image=DestroyImageList(image);
      return((Image *) NULL);
    }
  /*
    Set the page density.
  */
  delta.x=DefaultResolution;
  delta.y=DefaultResolution;
  if ((image->resolution.x == 0.0) || (image->resolution.y == 0.0))
    {
      flags=ParseGeometry(PSDensityGeometry,&geometry_info);
      image->resolution.x=geometry_info.rho;
      image->resolution.y=geometry_info.sigma;
      if ((flags & SigmaValue) == 0)
        image->resolution.y=image->resolution.x;
    }
  if (image_info->density != (char *) NULL)
    {
      flags=ParseGeometry(image_info->density,&geometry_info);
      image->resolution.x=geometry_info.rho;
      image->resolution.y=geometry_info.sigma;
      if ((flags & SigmaValue) == 0)
        image->resolution.y=image->resolution.x;
    }
  (void) memset(&page,0,sizeof(page));
  (void) ParseAbsoluteGeometry(PSPageGeometry,&page);
  if (image_info->page != (char *) NULL)
    (void) ParseAbsoluteGeometry(image_info->page,&page);
  page.width=(size_t) ceil((double) (page.width*image->resolution.x/delta.x)-
    0.5);
  page.height=(size_t) ceil((double) (page.height*image->resolution.y/delta.y)-
    0.5);
  /*
    Determine page geometry from the PDF media box.
  */
  cmyk=image_info->colorspace == CMYKColorspace ? MagickTrue : MagickFalse;
  cropbox=IsStringTrue(GetImageOption(image_info,"pdf:use-cropbox"));
  stop_on_error=IsStringTrue(GetImageOption(image_info,"pdf:stop-on-error"));
  trimbox=IsStringTrue(GetImageOption(image_info,"pdf:use-trimbox"));
  count=0;
  spotcolor=0;
  (void) memset(&bounding_box,0,sizeof(bounding_box));
  (void) memset(&bounds,0,sizeof(bounds));
  (void) memset(&hires_bounds,0,sizeof(hires_bounds));
  (void) memset(command,0,sizeof(command));
  angle=0.0;
  p=command;
  for (c=ReadBlobByte(image); c != EOF; c=ReadBlobByte(image))
  {
    /*
      Note PDF elements.
    */
    if (c == '\n')
      c=' ';
    *p++=(char) c;
    if ((c != (int) '/') && (c != (int) '%') &&
        ((size_t) (p-command) < (MagickPathExtent-1)))
      continue;
    *(--p)='\0';
    p=command;
    if (LocaleNCompare(PDFRotate,command,strlen(PDFRotate)) == 0)
      count=(ssize_t) sscanf(command,"Rotate %lf",&angle);
    /*
      Is this a CMYK document?
    */
    if (LocaleNCompare(DefaultCMYK,command,strlen(DefaultCMYK)) == 0)
      cmyk=MagickTrue;
    if (LocaleNCompare(DeviceCMYK,command,strlen(DeviceCMYK)) == 0)
      cmyk=MagickTrue;
    if (LocaleNCompare(CMYKProcessColor,command,strlen(CMYKProcessColor)) == 0)
      cmyk=MagickTrue;
    if (LocaleNCompare(SpotColor,command,strlen(SpotColor)) == 0)
      {
        char
          name[MagickPathExtent],
          property[MagickPathExtent],
          *value;

        /*
          Note spot names.
        */
        (void) FormatLocaleString(property,MagickPathExtent,
          "pdf:SpotColor-%.20g",(double) spotcolor++);
        i=0;
        for (c=ReadBlobByte(image); c != EOF; c=ReadBlobByte(image))
        {
          if ((isspace(c) != 0) || (c == '/') || ((i+1) == MagickPathExtent))
            break;
          name[i++]=(char) c;
        }
        name[i]='\0';
        value=ConstantString(name);
        (void) SubstituteString(&value,"#20"," ");
        if (*value != '\0')
          (void) SetImageProperty(image,property,value,exception);
        value=DestroyString(value);
        continue;
      }
    if (LocaleNCompare(PDFVersion,command,strlen(PDFVersion)) == 0)
      (void) SetImageProperty(image,"pdf:Version",command,exception);
    if (image_info->page != (char *) NULL)
      continue;
    count=0;
    if (cropbox != MagickFalse)
      {
        if (LocaleNCompare(CropBox,command,strlen(CropBox)) == 0)
          {
            /*
              Note region defined by crop box.
            */
            count=(ssize_t) sscanf(command,"CropBox [%lf %lf %lf %lf",
              &bounds.x1,&bounds.y1,&bounds.x2,&bounds.y2);
            if (count != 4)
              count=(ssize_t) sscanf(command,"CropBox[%lf %lf %lf %lf",
                &bounds.x1,&bounds.y1,&bounds.x2,&bounds.y2);
          }
      }
    else
      if (trimbox != MagickFalse)
        {
          if (LocaleNCompare(TrimBox,command,strlen(TrimBox)) == 0)
            {
              /*
                Note region defined by trim box.
              */
              count=(ssize_t) sscanf(command,"TrimBox [%lf %lf %lf %lf",
                &bounds.x1,&bounds.y1,&bounds.x2,&bounds.y2);
              if (count != 4)
                count=(ssize_t) sscanf(command,"TrimBox[%lf %lf %lf %lf",
                  &bounds.x1,&bounds.y1,&bounds.x2,&bounds.y2);
            }
        }
      else
        if (LocaleNCompare(MediaBox,command,strlen(MediaBox)) == 0)
          {
            /*
              Note region defined by media box.
            */
            count=(ssize_t) sscanf(command,"MediaBox [%lf %lf %lf %lf",
              &bounds.x1,&bounds.y1,&bounds.x2,&bounds.y2);
            if (count != 4)
              count=(ssize_t) sscanf(command,"MediaBox[%lf %lf %lf %lf",
                &bounds.x1,&bounds.y1,&bounds.x2,&bounds.y2);
          }
    if (count != 4)
      continue;
    if ((fabs(bounds.x2-bounds.x1) <= fabs(hires_bounds.x2-hires_bounds.x1)) ||
        (fabs(bounds.y2-bounds.y1) <= fabs(hires_bounds.y2-hires_bounds.y1)))
      continue;
    hires_bounds=bounds;
  }
  if ((fabs(hires_bounds.x2-hires_bounds.x1) >= MagickEpsilon) &&
      (fabs(hires_bounds.y2-hires_bounds.y1) >= MagickEpsilon))
    {
      /*
        Set PDF render geometry.
      */
      (void) FormatLocaleString(geometry,MagickPathExtent,"%gx%g%+.15g%+.15g",
        hires_bounds.x2-bounds.x1,hires_bounds.y2-hires_bounds.y1,
        hires_bounds.x1,hires_bounds.y1);
      (void) SetImageProperty(image,"pdf:HiResBoundingBox",geometry,exception);
      page.width=(size_t) ceil((double) ((hires_bounds.x2-hires_bounds.x1)*
        image->resolution.x/delta.x)-0.5);
      page.height=(size_t) ceil((double) ((hires_bounds.y2-hires_bounds.y1)*
        image->resolution.y/delta.y)-0.5);
    }
  fitPage=MagickFalse;
  option=GetImageOption(image_info,"pdf:fit-page");
  if (option != (char *) NULL)
    {
      char
        *page_geometry;

      page_geometry=GetPageGeometry(option);
      flags=ParseMetaGeometry(page_geometry,&page.x,&page.y,&page.width,
        &page.height);
      page_geometry=DestroyString(page_geometry);
      if (flags == NoValue)
        {
          (void) ThrowMagickException(exception,GetMagickModule(),OptionError,
            "InvalidGeometry","`%s'",option);
          image=DestroyImage(image);
          return((Image *) NULL);
        }
      page.width=(size_t) ceil((double) (page.width*image->resolution.x/delta.x)
        -0.5);
      page.height=(size_t) ceil((double) (page.height*image->resolution.y/
        delta.y) -0.5);
      fitPage=MagickTrue;
    }
  if ((fabs(angle) == 90.0) || (fabs(angle) == 270.0))
    {
      size_t
        swap;

      swap=page.width;
      page.width=page.height;
      page.height=swap;
    }
  if (IssRGBCompatibleColorspace(image_info->colorspace) != MagickFalse)
    cmyk=MagickFalse;
  /*
    Create Ghostscript control file.
  */
  file=AcquireUniqueFileResource(postscript_filename);
  if (file == -1)
    {
      ThrowFileException(exception,FileOpenError,"UnableToCreateTemporaryFile",
        image_info->filename);
      image=DestroyImage(image);
      return((Image *) NULL);
    }
  count=write(file," ",1);
  file=close(file)-1;
  /*
    Render Postscript with the Ghostscript delegate.
  */
  if (image_info->monochrome != MagickFalse)
    delegate_info=GetDelegateInfo("ps:mono",(char *) NULL,exception);
  else
     if (cmyk != MagickFalse)
       delegate_info=GetDelegateInfo("ps:cmyk",(char *) NULL,exception);
     else
       delegate_info=GetDelegateInfo("ps:alpha",(char *) NULL,exception);
  if (delegate_info == (const DelegateInfo *) NULL)
    {
      (void) RelinquishUniqueFileResource(postscript_filename);
      image=DestroyImage(image);
      return((Image *) NULL);
    }
  density=AcquireString("");
  options=AcquireString("");
  (void) FormatLocaleString(density,MagickPathExtent,"%gx%g",
    image->resolution.x,image->resolution.y);
  if ((image_info->page != (char *) NULL) || (fitPage != MagickFalse))
    (void) FormatLocaleString(options,MagickPathExtent,"-g%.20gx%.20g ",(double)
      page.width,(double) page.height);
  if (fitPage != MagickFalse)
    (void) ConcatenateMagickString(options,"-dPSFitPage ",MagickPathExtent);
  if (cmyk != MagickFalse)
    (void) ConcatenateMagickString(options,"-dUseCIEColor ",MagickPathExtent);
  if (cropbox != MagickFalse)
    (void) ConcatenateMagickString(options,"-dUseCropBox ",MagickPathExtent);
  if (stop_on_error != MagickFalse)
    (void) ConcatenateMagickString(options,"-dPDFSTOPONERROR ",
      MagickPathExtent);
  if (trimbox != MagickFalse)
    (void) ConcatenateMagickString(options,"-dUseTrimBox ",MagickPathExtent);
  option=GetImageOption(image_info,"authenticate");
  if (option != (char *) NULL)
    {
      char
        passphrase[MagickPathExtent];

      (void) FormatLocaleString(passphrase,MagickPathExtent,
        "\"-sPDFPassword=%s\" ",option);
      (void) ConcatenateMagickString(options,passphrase,MagickPathExtent);
    }
  read_info=CloneImageInfo(image_info);
  *read_info->magick='\0';
  if (read_info->number_scenes != 0)
    {
      char
        pages[MagickPathExtent];

      (void) FormatLocaleString(pages,MagickPathExtent,"-dFirstPage=%.20g "
        "-dLastPage=%.20g",(double) read_info->scene+1,(double)
        (read_info->scene+read_info->number_scenes));
      (void) ConcatenateMagickString(options,pages,MagickPathExtent);
      read_info->number_scenes=0;
      if (read_info->scenes != (char *) NULL)
        *read_info->scenes='\0';
    }
  (void) CopyMagickString(filename,read_info->filename,MagickPathExtent);
  (void) AcquireUniqueFilename(filename);
  (void) RelinquishUniqueFileResource(filename);
  (void) ConcatenateMagickString(filename,"%d",MagickPathExtent);
  (void) FormatLocaleString(command,MagickPathExtent,
    GetDelegateCommands(delegate_info),
    read_info->antialias != MagickFalse ? 4 : 1,
    read_info->antialias != MagickFalse ? 4 : 1,density,options,filename,
    postscript_filename,input_filename);
  options=DestroyString(options);
  density=DestroyString(density);
  *message='\0';
  status=InvokePDFDelegate(read_info->verbose,command,message,exception);
  (void) RelinquishUniqueFileResource(postscript_filename);
  (void) RelinquishUniqueFileResource(input_filename);
  pdf_image=(Image *) NULL;
  if (status == MagickFalse)
    for (i=1; ; i++)
    {
      (void) InterpretImageFilename(image_info,image,filename,(int) i,
        read_info->filename,exception);
      if (IsPDFRendered(read_info->filename) == MagickFalse)
        break;
      (void) RelinquishUniqueFileResource(read_info->filename);
    }
  else
    for (i=1; ; i++)
    {
      (void) InterpretImageFilename(image_info,image,filename,(int) i,
        read_info->filename,exception);
      if (IsPDFRendered(read_info->filename) == MagickFalse)
        break;
      read_info->blob=NULL;
      read_info->length=0;
      next=ReadImage(read_info,exception);
      (void) RelinquishUniqueFileResource(read_info->filename);
      if (next == (Image *) NULL)
        break;
      AppendImageToList(&pdf_image,next);
    }
  read_info=DestroyImageInfo(read_info);
  if (pdf_image == (Image *) NULL)
    {
      if (*message != '\0')
        (void) ThrowMagickException(exception,GetMagickModule(),DelegateError,
          "PDFDelegateFailed","`%s'",message);
      image=DestroyImage(image);
      return((Image *) NULL);
    }
  if (LocaleCompare(pdf_image->magick,"BMP") == 0)
    {
      Image
        *cmyk_image;

      cmyk_image=ConsolidateCMYKImages(pdf_image,exception);
      if (cmyk_image != (Image *) NULL)
        {
          pdf_image=DestroyImageList(pdf_image);
          pdf_image=cmyk_image;
        }
    }
  (void) SeekBlob(image,0,SEEK_SET);
  for (c=ReadBlobByte(image); c != EOF; c=ReadBlobByte(image))
  {
    /*
      Note document structuring comments.
    */
    *p++=(char) c;
    if ((strchr("\n\r%",c) == (char *) NULL) &&
        ((size_t) (p-command) < (MagickPathExtent-1)))
      continue;
    *p='\0';
    p=command;
    if (LocaleNCompare(BeginXMPPacket,command,strlen(BeginXMPPacket)) == 0)
      {
        StringInfo
          *profile;

        /*
          Read XMP profile.
        */
        p=command;
        profile=StringToStringInfo(command);
        for (i=(ssize_t) GetStringInfoLength(profile)-1; c != EOF; i++)
        {
          SetStringInfoLength(profile,(size_t) (i+1));
          c=ReadBlobByte(image);
          GetStringInfoDatum(profile)[i]=(unsigned char) c;
          *p++=(char) c;
          if ((strchr("\n\r%",c) == (char *) NULL) &&
              ((size_t) (p-command) < (MagickPathExtent-1)))
            continue;
          *p='\0';
          p=command;
          if (LocaleNCompare(EndXMPPacket,command,strlen(EndXMPPacket)) == 0)
            break;
        }
        SetStringInfoLength(profile,(size_t) i);
        (void) SetImageProfile(image,"xmp",profile,exception);
        profile=DestroyStringInfo(profile);
        continue;
      }
  }
  (void) CloseBlob(image);
  if (image_info->number_scenes != 0)
    {
      Image
        *clone_image;

      /*
        Add place holder images to meet the subimage specification requirement.
      */
      for (i=0; i < (ssize_t) image_info->scene; i++)
      {
        clone_image=CloneImage(pdf_image,1,1,MagickTrue,exception);
        if (clone_image != (Image *) NULL)
          PrependImageToList(&pdf_image,clone_image);
      }
    }
  do
  {
    (void) CopyMagickString(pdf_image->filename,filename,MagickPathExtent);
    (void) CopyMagickString(pdf_image->magick,image->magick,MagickPathExtent);
    pdf_image->page=page;
    (void) CloneImageProfiles(pdf_image,image);
    (void) CloneImageProperties(pdf_image,image);
    next=SyncNextImageInList(pdf_image);
    if (next != (Image *) NULL)
      pdf_image=next;
  } while (next != (Image *) NULL);
  image=DestroyImage(image);
  scene=0;
  for (next=GetFirstImageInList(pdf_image); next != (Image *) NULL; )
  {
    next->scene=scene++;
    next=GetNextImageInList(next);
  }
  return(GetFirstImageInList(pdf_image));
}