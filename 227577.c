static Image *ReadSVGImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  char
    filename[MagickPathExtent];

  const char
    *option;

  FILE
    *file;

  Image
    *image,
    *next;

  int
    status,
    unique_file;

  ssize_t
    n;

  SVGInfo
    *svg_info;

  unsigned char
    message[MagickPathExtent];

  xmlSAXHandler
    sax_modules;

  xmlSAXHandlerPtr
    sax_handler;

  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  assert(exception != (ExceptionInfo *) NULL);
  if (image_info->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      image_info->filename);
  assert(exception->signature == MagickCoreSignature);
  image=AcquireImage(image_info,exception);
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == MagickFalse)
    {
      image=DestroyImageList(image);
      return((Image *) NULL);
    }
  if ((fabs(image->resolution.x) < MagickEpsilon) ||
      (fabs(image->resolution.y) < MagickEpsilon))
    {
      GeometryInfo
        geometry_info;

      int
        flags;

      flags=ParseGeometry(SVGDensityGeometry,&geometry_info);
      image->resolution.x=geometry_info.rho;
      image->resolution.y=geometry_info.sigma;
      if ((flags & SigmaValue) == 0)
        image->resolution.y=image->resolution.x;
    }
  if (LocaleCompare(image_info->magick,"MSVG") != 0)
    {
      Image
        *svg_image;

      svg_image=RenderSVGImage(image_info,image,exception);
      if (svg_image != (Image *) NULL)
        {
          image=DestroyImageList(image);
          return(svg_image);
        }
      {
#if defined(MAGICKCORE_RSVG_DELEGATE)
#if defined(MAGICKCORE_CAIRO_DELEGATE)
        cairo_surface_t
          *cairo_surface;

        cairo_t
          *cairo_image;

        MagickBooleanType
          apply_density;

        MemoryInfo
          *pixel_info;

        register unsigned char
          *p;

        RsvgDimensionData
          dimension_info;

        unsigned char
          *pixels;

#else
        GdkPixbuf
          *pixel_buffer;

        register const guchar
          *p;
#endif

        GError
          *error;

        PixelInfo
          fill_color;

        register ssize_t
          x;

        register Quantum
          *q;

        RsvgHandle
          *svg_handle;

        ssize_t
          y;

        unsigned char
          *buffer;

        buffer=(unsigned char *) AcquireQuantumMemory(MagickMaxBufferExtent,
          sizeof(*buffer));
        if (buffer == (unsigned char *) NULL)
          ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
#if LIBRSVG_CHECK_VERSION(2,40,3)
        option=GetImageOption(image_info,"svg:xml-parse-huge");
        if ((option != (char *) NULL) && (IsStringTrue(option) != MagickFalse))
          svg_handle=rsvg_handle_new_with_flags(RSVG_HANDLE_FLAG_UNLIMITED);
        else
#endif
          svg_handle=rsvg_handle_new();
        if (svg_handle == (RsvgHandle *) NULL)
          {
            buffer=(unsigned char *) RelinquishMagickMemory(buffer);
            ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
          }
        rsvg_handle_set_base_uri(svg_handle,image_info->filename);
        if ((fabs(image->resolution.x) > MagickEpsilon) &&
            (fabs(image->resolution.y) > MagickEpsilon))
          rsvg_handle_set_dpi_x_y(svg_handle,image->resolution.x,
            image->resolution.y);
        while ((n=ReadBlob(image,MagickMaxBufferExtent-1,buffer)) != 0)
        {
          buffer[n]='\0';
          error=(GError *) NULL;
          (void) rsvg_handle_write(svg_handle,buffer,n,&error);
          if (error != (GError *) NULL)
            g_error_free(error);
        }
        buffer=(unsigned char *) RelinquishMagickMemory(buffer);
        error=(GError *) NULL;
        rsvg_handle_close(svg_handle,&error);
        if (error != (GError *) NULL)
          g_error_free(error);
#if defined(MAGICKCORE_CAIRO_DELEGATE)
        apply_density=MagickTrue;
        rsvg_handle_get_dimensions(svg_handle,&dimension_info);
        if ((image->resolution.x > 0.0) && (image->resolution.y > 0.0))
          {
            RsvgDimensionData
              dpi_dimension_info;

            /*
              We should not apply the density when the internal 'factor' is 'i'.
              This can be checked by using the trick below.
            */
            rsvg_handle_set_dpi_x_y(svg_handle,image->resolution.x*256,
              image->resolution.y*256);
            rsvg_handle_get_dimensions(svg_handle,&dpi_dimension_info);
            if ((dpi_dimension_info.width != dimension_info.width) ||
                (dpi_dimension_info.height != dimension_info.height))
              apply_density=MagickFalse;
            rsvg_handle_set_dpi_x_y(svg_handle,image->resolution.x,
              image->resolution.y);
          }
        if (image_info->size != (char *) NULL)
          {
            (void) GetGeometry(image_info->size,(ssize_t *) NULL,
              (ssize_t *) NULL,&image->columns,&image->rows);
            if ((image->columns != 0) || (image->rows != 0))
              {
                image->resolution.x=DefaultSVGDensity*image->columns/
                  dimension_info.width;
                image->resolution.y=DefaultSVGDensity*image->rows/
                  dimension_info.height;
                if (fabs(image->resolution.x) < MagickEpsilon)
                  image->resolution.x=image->resolution.y;
                else
                  if (fabs(image->resolution.y) < MagickEpsilon)
                    image->resolution.y=image->resolution.x;
                  else
                    image->resolution.x=image->resolution.y=MagickMin(
                      image->resolution.x,image->resolution.y);
                apply_density=MagickTrue;
              }
          }
        if (apply_density != MagickFalse)
          {
            image->columns=image->resolution.x*dimension_info.width/
              DefaultSVGDensity;
            image->rows=image->resolution.y*dimension_info.height/
              DefaultSVGDensity;
          }
        else
          {
            image->columns=dimension_info.width;
            image->rows=dimension_info.height;
          }
        pixel_info=(MemoryInfo *) NULL;
#else
        pixel_buffer=rsvg_handle_get_pixbuf(svg_handle);
        rsvg_handle_free(svg_handle);
        image->columns=gdk_pixbuf_get_width(pixel_buffer);
        image->rows=gdk_pixbuf_get_height(pixel_buffer);
#endif
        image->alpha_trait=BlendPixelTrait;
        if (image_info->ping == MagickFalse)
          {
#if defined(MAGICKCORE_CAIRO_DELEGATE)
            size_t
              stride;
#endif

            status=SetImageExtent(image,image->columns,image->rows,exception);
            if (status == MagickFalse)
              {
#if !defined(MAGICKCORE_CAIRO_DELEGATE)
                g_object_unref(G_OBJECT(pixel_buffer));
#endif
                g_object_unref(svg_handle);
                ThrowReaderException(MissingDelegateError,
                  "NoDecodeDelegateForThisImageFormat");
              }
#if defined(MAGICKCORE_CAIRO_DELEGATE)
            stride=4*image->columns;
#if defined(MAGICKCORE_PANGOCAIRO_DELEGATE)
            stride=(size_t) cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32,
              (int) image->columns);
#endif
            pixel_info=AcquireVirtualMemory(stride,image->rows*sizeof(*pixels));
            if (pixel_info == (MemoryInfo *) NULL)
              {
                g_object_unref(svg_handle);
                ThrowReaderException(ResourceLimitError,
                  "MemoryAllocationFailed");
              }
            pixels=(unsigned char *) GetVirtualMemoryBlob(pixel_info);
#endif
            (void) SetImageBackgroundColor(image,exception);
#if defined(MAGICKCORE_CAIRO_DELEGATE)
            cairo_surface=cairo_image_surface_create_for_data(pixels,
              CAIRO_FORMAT_ARGB32,(int) image->columns,(int) image->rows,(int)
              stride);
            if ((cairo_surface == (cairo_surface_t *) NULL) ||
                (cairo_surface_status(cairo_surface) != CAIRO_STATUS_SUCCESS))
              {
                if (cairo_surface != (cairo_surface_t *) NULL)
                  cairo_surface_destroy(cairo_surface);
                pixel_info=RelinquishVirtualMemory(pixel_info);
                g_object_unref(svg_handle);
                ThrowReaderException(ResourceLimitError,
                  "MemoryAllocationFailed");
              }
            cairo_image=cairo_create(cairo_surface);
            cairo_set_operator(cairo_image,CAIRO_OPERATOR_CLEAR);
            cairo_paint(cairo_image);
            cairo_set_operator(cairo_image,CAIRO_OPERATOR_OVER);
            if (apply_density != MagickFalse)
              cairo_scale(cairo_image,image->resolution.x/DefaultSVGDensity,
                image->resolution.y/DefaultSVGDensity);
            rsvg_handle_render_cairo(svg_handle,cairo_image);
            cairo_destroy(cairo_image);
            cairo_surface_destroy(cairo_surface);
            g_object_unref(svg_handle);
            p=pixels;
#else
            p=gdk_pixbuf_get_pixels(pixel_buffer);
#endif
            GetPixelInfo(image,&fill_color);
            for (y=0; y < (ssize_t) image->rows; y++)
            {
              q=GetAuthenticPixels(image,0,y,image->columns,1,exception);
              if (q == (Quantum *) NULL)
                break;
              for (x=0; x < (ssize_t) image->columns; x++)
              {
#if defined(MAGICKCORE_CAIRO_DELEGATE)
                fill_color.blue=ScaleCharToQuantum(*p++);
                fill_color.green=ScaleCharToQuantum(*p++);
                fill_color.red=ScaleCharToQuantum(*p++);
#else
                fill_color.red=ScaleCharToQuantum(*p++);
                fill_color.green=ScaleCharToQuantum(*p++);
                fill_color.blue=ScaleCharToQuantum(*p++);
#endif
                fill_color.alpha=ScaleCharToQuantum(*p++);
#if defined(MAGICKCORE_CAIRO_DELEGATE)
                {
                  double
                    gamma;

                  gamma=QuantumScale*fill_color.alpha;
                  gamma=PerceptibleReciprocal(gamma);
                  fill_color.blue*=gamma;
                  fill_color.green*=gamma;
                  fill_color.red*=gamma;
                }
#endif
                CompositePixelOver(image,&fill_color,fill_color.alpha,q,(double)
                  GetPixelAlpha(image,q),q);
                q+=GetPixelChannels(image);
              }
              if (SyncAuthenticPixels(image,exception) == MagickFalse)
                break;
              if (image->previous == (Image *) NULL)
                {
                  status=SetImageProgress(image,LoadImageTag,(MagickOffsetType)
                    y,image->rows);
                  if (status == MagickFalse)
                    break;
                }
            }
          }
#if defined(MAGICKCORE_CAIRO_DELEGATE)
        if (pixel_info != (MemoryInfo *) NULL)
          pixel_info=RelinquishVirtualMemory(pixel_info);
#else
        g_object_unref(G_OBJECT(pixel_buffer));
#endif
        (void) CloseBlob(image);
        for (next=GetFirstImageInList(image); next != (Image *) NULL; )
        {
          (void) CopyMagickString(next->filename,image->filename,MaxTextExtent);
          (void) CopyMagickString(next->magick,image->magick,MaxTextExtent);
          next=GetNextImageInList(next);
        }
        return(GetFirstImageInList(image));
#endif
      }
    }
  /*
    Open draw file.
  */
  file=(FILE *) NULL;
  unique_file=AcquireUniqueFileResource(filename);
  if (unique_file != -1)
    file=fdopen(unique_file,"w");
  if ((unique_file == -1) || (file == (FILE *) NULL))
    {
      (void) CopyMagickString(image->filename,filename,MagickPathExtent);
      ThrowFileException(exception,FileOpenError,"UnableToCreateTemporaryFile",
        image->filename);
      image=DestroyImageList(image);
      return((Image *) NULL);
    }
  /*
    Parse SVG file.
  */
  svg_info=AcquireSVGInfo();
  if (svg_info == (SVGInfo *) NULL)
    {
      (void) fclose(file);
      ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
    }
  svg_info->file=file;
  svg_info->exception=exception;
  svg_info->image=image;
  svg_info->image_info=image_info;
  svg_info->bounds.width=image->columns;
  svg_info->bounds.height=image->rows;
  svg_info->svgDepth=0;
  if (image_info->size != (char *) NULL)
    (void) CloneString(&svg_info->size,image_info->size);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),"begin SAX");
  xmlInitParser();
  (void) xmlSubstituteEntitiesDefault(1);
  (void) memset(&sax_modules,0,sizeof(sax_modules));
  sax_modules.internalSubset=SVGInternalSubset;
  sax_modules.isStandalone=SVGIsStandalone;
  sax_modules.hasInternalSubset=SVGHasInternalSubset;
  sax_modules.hasExternalSubset=SVGHasExternalSubset;
  sax_modules.resolveEntity=SVGResolveEntity;
  sax_modules.getEntity=SVGGetEntity;
  sax_modules.entityDecl=SVGEntityDeclaration;
  sax_modules.notationDecl=SVGNotationDeclaration;
  sax_modules.attributeDecl=SVGAttributeDeclaration;
  sax_modules.elementDecl=SVGElementDeclaration;
  sax_modules.unparsedEntityDecl=SVGUnparsedEntityDeclaration;
  sax_modules.setDocumentLocator=SVGSetDocumentLocator;
  sax_modules.startDocument=SVGStartDocument;
  sax_modules.endDocument=SVGEndDocument;
  sax_modules.startElement=SVGStartElement;
  sax_modules.endElement=SVGEndElement;
  sax_modules.reference=SVGReference;
  sax_modules.characters=SVGCharacters;
  sax_modules.ignorableWhitespace=SVGIgnorableWhitespace;
  sax_modules.processingInstruction=SVGProcessingInstructions;
  sax_modules.comment=SVGComment;
  sax_modules.warning=SVGWarning;
  sax_modules.error=SVGError;
  sax_modules.fatalError=SVGError;
  sax_modules.getParameterEntity=SVGGetParameterEntity;
  sax_modules.cdataBlock=SVGCDataBlock;
  sax_modules.externalSubset=SVGExternalSubset;
  sax_handler=(&sax_modules);
  n=ReadBlob(image,MagickPathExtent-1,message);
  message[n]='\0';
  if (n > 0)
    {
      svg_info->parser=xmlCreatePushParserCtxt(sax_handler,svg_info,(char *)
        message,n,image->filename);
      if (svg_info->parser != (xmlParserCtxtPtr) NULL)
        {
          option=GetImageOption(image_info,"svg:xml-parse-huge");
          if ((option != (char *) NULL) && (IsStringTrue(option) != MagickFalse))
            (void) xmlCtxtUseOptions(svg_info->parser,XML_PARSE_HUGE);
          while ((n=ReadBlob(image,MagickPathExtent-1,message)) != 0)
          {
            message[n]='\0';
            status=xmlParseChunk(svg_info->parser,(char *) message,(int) n,0);
            if (status != 0)
              break;
          }
        }
    }
  if (svg_info->parser == (xmlParserCtxtPtr) NULL)
    {
      svg_info=DestroySVGInfo(svg_info);
      (void) RelinquishUniqueFileResource(filename);
      image=DestroyImage(image);
      return((Image *) NULL);
    }
  (void) xmlParseChunk(svg_info->parser,(char *) message,0,1);
  SVGEndDocument(svg_info);
  if (svg_info->parser->myDoc != (xmlDocPtr) NULL)
    xmlFreeDoc(svg_info->parser->myDoc);
  xmlFreeParserCtxt(svg_info->parser);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),"end SAX");
  (void) fclose(file);
  (void) CloseBlob(image);
  image->columns=svg_info->width;
  image->rows=svg_info->height;
  if (exception->severity >= ErrorException)
    {
      svg_info=DestroySVGInfo(svg_info);
      (void) RelinquishUniqueFileResource(filename);
      image=DestroyImage(image);
      return((Image *) NULL);
    }
  if (image_info->ping == MagickFalse)
    {
      ImageInfo
        *read_info;

      /*
        Draw image.
      */
      image=DestroyImage(image);
      image=(Image *) NULL;
      read_info=CloneImageInfo(image_info);
      SetImageInfoBlob(read_info,(void *) NULL,0);
      (void) FormatLocaleString(read_info->filename,MagickPathExtent,"mvg:%s",
        filename);
      image=ReadImage(read_info,exception);
      read_info=DestroyImageInfo(read_info);
      if (image != (Image *) NULL)
        (void) CopyMagickString(image->filename,image_info->filename,
          MagickPathExtent);
    }
  /*
    Relinquish resources.
  */
  if (image != (Image *) NULL)
    {
      if (svg_info->title != (char *) NULL)
        (void) SetImageProperty(image,"svg:title",svg_info->title,exception);
      if (svg_info->comment != (char *) NULL)
        (void) SetImageProperty(image,"svg:comment",svg_info->comment,
          exception);
    }
  for (next=GetFirstImageInList(image); next != (Image *) NULL; )
  {
    (void) CopyMagickString(next->filename,image->filename,MaxTextExtent);
    (void) CopyMagickString(next->magick,image->magick,MaxTextExtent);
    next=GetNextImageInList(next);
  }
  svg_info=DestroySVGInfo(svg_info);
  (void) RelinquishUniqueFileResource(filename);
  return(GetFirstImageInList(image));
}