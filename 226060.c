static Image *ReadMATImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  Image *image, *image2=NULL,
   *rotated_image;
  register Quantum *q;

  unsigned int status;
  MATHeader MATLAB_HDR;
  size_t size;
  size_t CellType;
  QuantumInfo *quantum_info;
  ImageInfo *clone_info;
  int i;
  ssize_t ldblk;
  unsigned char *BImgBuff = NULL;
  double MinVal, MaxVal;
  unsigned z, z2;
  unsigned Frames;
  int logging;
  int sample_size;
  MagickOffsetType filepos=0x80;
  BlobInfo *blob;
  size_t one;

  unsigned int (*ReadBlobXXXLong)(Image *image);
  unsigned short (*ReadBlobXXXShort)(Image *image);
  void (*ReadBlobDoublesXXX)(Image * image, size_t len, double *data);
  void (*ReadBlobFloatsXXX)(Image * image, size_t len, float *data);


  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  logging = LogMagickEvent(CoderEvent,GetMagickModule(),"enter");

  /*
     Open image file.
   */
  image = AcquireImage(image_info,exception);

  status = OpenBlob(image_info, image, ReadBinaryBlobMode, exception);
  if (status == MagickFalse)
    {
      image=DestroyImageList(image);
      return((Image *) NULL);
    }
  /*
     Read MATLAB image.
   */
  quantum_info=(QuantumInfo *) NULL;
  clone_info=(ImageInfo *) NULL;
  if (ReadBlob(image,124,(unsigned char *) &MATLAB_HDR.identific) != 124)
    ThrowReaderException(CorruptImageError,"ImproperImageHeader");
  if (strncmp(MATLAB_HDR.identific,"MATLAB",6) != 0)
    {
      image2=ReadMATImageV4(image_info,image,exception);
      if (image2  == NULL)
        goto MATLAB_KO;
      image=image2;
      goto END_OF_READING;
    }
  MATLAB_HDR.Version = ReadBlobLSBShort(image);
  if(ReadBlob(image,2,(unsigned char *) &MATLAB_HDR.EndianIndicator) != 2)
    ThrowReaderException(CorruptImageError,"ImproperImageHeader");

  if (logging)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),"  Endian %c%c",
      MATLAB_HDR.EndianIndicator[0],MATLAB_HDR.EndianIndicator[1]);
  if (!strncmp(MATLAB_HDR.EndianIndicator, "IM", 2))
  {
    ReadBlobXXXLong = ReadBlobLSBLong;
    ReadBlobXXXShort = ReadBlobLSBShort;
    ReadBlobDoublesXXX = ReadBlobDoublesLSB;
    ReadBlobFloatsXXX = ReadBlobFloatsLSB;
    image->endian = LSBEndian;
  }
  else if (!strncmp(MATLAB_HDR.EndianIndicator, "MI", 2))
  {
    ReadBlobXXXLong = ReadBlobMSBLong;
    ReadBlobXXXShort = ReadBlobMSBShort;
    ReadBlobDoublesXXX = ReadBlobDoublesMSB;
    ReadBlobFloatsXXX = ReadBlobFloatsMSB;
    image->endian = MSBEndian;
  }
  else
    goto MATLAB_KO;    /* unsupported endian */

  if (strncmp(MATLAB_HDR.identific, "MATLAB", 6))
    {
MATLAB_KO:
      clone_info=DestroyImageInfo(clone_info);
      ThrowReaderException(CorruptImageError,"ImproperImageHeader");
    }

  filepos = TellBlob(image);
  while(!EOFBlob(image)) /* object parser loop */
  {
    Frames = 1;
    (void) SeekBlob(image,filepos,SEEK_SET);
    /* printf("pos=%X\n",TellBlob(image)); */

    MATLAB_HDR.DataType = ReadBlobXXXLong(image);
    if(EOFBlob(image)) break;
    MATLAB_HDR.ObjectSize = ReadBlobXXXLong(image);
    if(EOFBlob(image)) break;
    if((MagickSizeType) (MATLAB_HDR.ObjectSize+filepos) > GetBlobSize(image))
      goto MATLAB_KO;
    filepos += MATLAB_HDR.ObjectSize + 4 + 4;

    clone_info=CloneImageInfo(image_info);
    image2 = image;
#if defined(MAGICKCORE_ZLIB_DELEGATE)
    if(MATLAB_HDR.DataType == miCOMPRESSED)
    {
      image2 = decompress_block(image,&MATLAB_HDR.ObjectSize,clone_info,exception);
      if(image2==NULL) continue;
      MATLAB_HDR.DataType = ReadBlobXXXLong(image2); /* replace compressed object type. */
    }
#endif

    if(MATLAB_HDR.DataType!=miMATRIX) continue;  /* skip another objects. */

    MATLAB_HDR.unknown1 = ReadBlobXXXLong(image2);
    MATLAB_HDR.unknown2 = ReadBlobXXXLong(image2);

    MATLAB_HDR.unknown5 = ReadBlobXXXLong(image2);
    MATLAB_HDR.StructureClass = MATLAB_HDR.unknown5 & 0xFF;
    MATLAB_HDR.StructureFlag = (MATLAB_HDR.unknown5>>8) & 0xFF;

    MATLAB_HDR.unknown3 = ReadBlobXXXLong(image2);
    if(image!=image2)
      MATLAB_HDR.unknown4 = ReadBlobXXXLong(image2);  /* ??? don't understand why ?? */
    MATLAB_HDR.unknown4 = ReadBlobXXXLong(image2);
    MATLAB_HDR.DimFlag = ReadBlobXXXLong(image2);
    MATLAB_HDR.SizeX = ReadBlobXXXLong(image2);
    MATLAB_HDR.SizeY = ReadBlobXXXLong(image2);


    switch(MATLAB_HDR.DimFlag)
    {
      case  8: z2=z=1; break;      /* 2D matrix*/
      case 12: z2=z = ReadBlobXXXLong(image2);  /* 3D matrix RGB*/
           (void) ReadBlobXXXLong(image2);
         if(z!=3) ThrowReaderException(CoderError, "MultidimensionalMatricesAreNotSupported");
         break;
      case 16: z2=z = ReadBlobXXXLong(image2);  /* 4D matrix animation */
         if(z!=3 && z!=1)
            ThrowReaderException(CoderError, "MultidimensionalMatricesAreNotSupported");
         Frames = ReadBlobXXXLong(image2);
         if (Frames == 0)
           ThrowReaderException(CorruptImageError,"ImproperImageHeader");
         break;
      default: ThrowReaderException(CoderError, "MultidimensionalMatricesAreNotSupported");
    }

    MATLAB_HDR.Flag1 = ReadBlobXXXShort(image2);
    MATLAB_HDR.NameFlag = ReadBlobXXXShort(image2);

    if (logging) (void)LogMagickEvent(CoderEvent,GetMagickModule(),
          "MATLAB_HDR.StructureClass %d",MATLAB_HDR.StructureClass);
    if (MATLAB_HDR.StructureClass != mxCHAR_CLASS &&
        MATLAB_HDR.StructureClass != mxSINGLE_CLASS &&    /* float + complex float */
        MATLAB_HDR.StructureClass != mxDOUBLE_CLASS &&    /* double + complex double */
        MATLAB_HDR.StructureClass != mxINT8_CLASS &&
        MATLAB_HDR.StructureClass != mxUINT8_CLASS &&    /* uint8 + uint8 3D */
        MATLAB_HDR.StructureClass != mxINT16_CLASS &&
        MATLAB_HDR.StructureClass != mxUINT16_CLASS &&    /* uint16 + uint16 3D */
        MATLAB_HDR.StructureClass != mxINT32_CLASS &&
        MATLAB_HDR.StructureClass != mxUINT32_CLASS &&    /* uint32 + uint32 3D */
        MATLAB_HDR.StructureClass != mxINT64_CLASS &&
        MATLAB_HDR.StructureClass != mxUINT64_CLASS)    /* uint64 + uint64 3D */
      ThrowReaderException(CoderError,"UnsupportedCellTypeInTheMatrix");

    switch (MATLAB_HDR.NameFlag)
    {
      case 0:
        size = ReadBlobXXXLong(image2);  /* Object name string size */
        size = 4 * (ssize_t) ((size + 3 + 1) / 4);
        (void) SeekBlob(image2, size, SEEK_CUR);
        break;
      case 1:
      case 2:
      case 3:
      case 4:
        (void) ReadBlob(image2, 4, (unsigned char *) &size); /* Object name string */
        break;
      default:
        goto MATLAB_KO;
    }

    CellType = ReadBlobXXXLong(image2);    /* Additional object type */
    if (logging)
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "MATLAB_HDR.CellType: %.20g",(double) CellType);

    (void) ReadBlob(image2, 4, (unsigned char *) &size);     /* data size */

    NEXT_FRAME:
    switch (CellType)
    {
      case miINT8:
      case miUINT8:
        sample_size = 8;
        if(MATLAB_HDR.StructureFlag & FLAG_LOGICAL)
          image->depth = 1;
        else
          image->depth = 8;         /* Byte type cell */
        ldblk = (ssize_t) MATLAB_HDR.SizeX;
        break;
      case miINT16:
      case miUINT16:
        sample_size = 16;
        image->depth = 16;        /* Word type cell */
        ldblk = (ssize_t) (2 * MATLAB_HDR.SizeX);
        break;
      case miINT32:
      case miUINT32:
        sample_size = 32;
        image->depth = 32;        /* Dword type cell */
        ldblk = (ssize_t) (4 * MATLAB_HDR.SizeX);
        break;
      case miINT64:
      case miUINT64:
        sample_size = 64;
        image->depth = 64;        /* Qword type cell */
        ldblk = (ssize_t) (8 * MATLAB_HDR.SizeX);
        break;
      case miSINGLE:
        sample_size = 32;
        image->depth = 32;        /* double type cell */
        (void) SetImageOption(clone_info,"quantum:format","floating-point");
        if (MATLAB_HDR.StructureFlag & FLAG_COMPLEX)
  {              /* complex float type cell */
  }
        ldblk = (ssize_t) (4 * MATLAB_HDR.SizeX);
        break;
      case miDOUBLE:
        sample_size = 64;
        image->depth = 64;        /* double type cell */
        (void) SetImageOption(clone_info,"quantum:format","floating-point");
DisableMSCWarning(4127)
        if (sizeof(double) != 8)
RestoreMSCWarning
          ThrowReaderException(CoderError, "IncompatibleSizeOfDouble");
        if (MATLAB_HDR.StructureFlag & FLAG_COMPLEX)
  {                         /* complex double type cell */
  }
        ldblk = (ssize_t) (8 * MATLAB_HDR.SizeX);
        break;
      default:
        ThrowReaderException(CoderError, "UnsupportedCellTypeInTheMatrix");
    }
    (void) sample_size;
    image->columns = MATLAB_HDR.SizeX;
    image->rows = MATLAB_HDR.SizeY;
    one=1;
    image->colors = one << image->depth;
    if (image->columns == 0 || image->rows == 0)
      goto MATLAB_KO;
    if((unsigned long)ldblk*MATLAB_HDR.SizeY > MATLAB_HDR.ObjectSize)
      goto MATLAB_KO;
    /* Image is gray when no complex flag is set and 2D Matrix */
    if ((MATLAB_HDR.DimFlag == 8) &&
        ((MATLAB_HDR.StructureFlag & FLAG_COMPLEX) == 0))
      {
        image->type=GrayscaleType;
        SetImageColorspace(image,GRAYColorspace,exception);
      }


    /*
      If ping is true, then only set image size and colors without
      reading any image data.
    */
    if (image_info->ping)
    {
      size_t temp = image->columns;
      image->columns = image->rows;
      image->rows = temp;
      goto done_reading; /* !!!!!! BAD  !!!! */
    }
    status=SetImageExtent(image,image->columns,image->rows,exception);
    if (status == MagickFalse)
      return(DestroyImageList(image));
    quantum_info=AcquireQuantumInfo(clone_info,image);
    if (quantum_info == (QuantumInfo *) NULL)
      ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");

  /* ----- Load raster data ----- */
    BImgBuff = (unsigned char *) AcquireQuantumMemory((size_t) (ldblk),sizeof(double));    /* Ldblk was set in the check phase */
    if (BImgBuff == NULL)
      ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
    (void) ResetMagickMemory(BImgBuff,0,ldblk*sizeof(double));

    MinVal = 0;
    MaxVal = 0;
    if (CellType==miDOUBLE || CellType==miSINGLE)        /* Find Min and Max Values for floats */
    {
      CalcMinMax(image2, image_info->endian,  MATLAB_HDR.SizeX, MATLAB_HDR.SizeY, CellType, ldblk, BImgBuff, &quantum_info->minimum, &quantum_info->maximum);
    }

    /* Main loop for reading all scanlines */
    if(z==1) z=0; /* read grey scanlines */
    /* else read color scanlines */
    do
    {
      for (i = 0; i < (ssize_t) MATLAB_HDR.SizeY; i++)
      {
        q=GetAuthenticPixels(image,0,MATLAB_HDR.SizeY-i-1,image->columns,1,exception);
        if (q == (Quantum *) NULL)
  {
    if (logging) (void)LogMagickEvent(CoderEvent,GetMagickModule(),
              "  MAT set image pixels returns unexpected NULL on a row %u.", (unsigned)(MATLAB_HDR.SizeY-i-1));
    goto done_reading;    /* Skip image rotation, when cannot set image pixels    */
  }
        if(ReadBlob(image2,ldblk,(unsigned char *)BImgBuff) != (ssize_t) ldblk)
  {
    if (logging) (void)LogMagickEvent(CoderEvent,GetMagickModule(),
             "  MAT cannot read scanrow %u from a file.", (unsigned)(MATLAB_HDR.SizeY-i-1));
    goto ExitLoop;
  }
        if((CellType==miINT8 || CellType==miUINT8) && (MATLAB_HDR.StructureFlag & FLAG_LOGICAL))
        {
          FixLogical((unsigned char *)BImgBuff,ldblk);
          if(ImportQuantumPixels(image,(CacheView *) NULL,quantum_info,z2qtype[z],BImgBuff,exception) <= 0)
    {
ImportQuantumPixelsFailed:
      if (logging) (void)LogMagickEvent(CoderEvent,GetMagickModule(),
              "  MAT failed to ImportQuantumPixels for a row %u", (unsigned)(MATLAB_HDR.SizeY-i-1));
      break;
    }
        }
        else
        {
          if(ImportQuantumPixels(image,(CacheView *) NULL,quantum_info,z2qtype[z],BImgBuff,exception) <= 0)
      goto ImportQuantumPixelsFailed;


          if (z<=1 &&       /* fix only during a last pass z==0 || z==1 */
          (CellType==miINT8 || CellType==miINT16 || CellType==miINT32 || CellType==miINT64))
      FixSignedValues(image,q,MATLAB_HDR.SizeX);
        }

        if (!SyncAuthenticPixels(image,exception))
  {
    if (logging) (void)LogMagickEvent(CoderEvent,GetMagickModule(),
            "  MAT failed to sync image pixels for a row %u", (unsigned)(MATLAB_HDR.SizeY-i-1));
    goto ExitLoop;
  }
      }
    } while(z-- >= 2);
ExitLoop:


    /* Read complex part of numbers here */
    if (MATLAB_HDR.StructureFlag & FLAG_COMPLEX)
    {        /* Find Min and Max Values for complex parts of floats */
      CellType = ReadBlobXXXLong(image2);    /* Additional object type */
      i = ReadBlobXXXLong(image2);           /* size of a complex part - toss away*/

      if (CellType==miDOUBLE || CellType==miSINGLE)
      {
        CalcMinMax(image2,  image_info->endian, MATLAB_HDR.SizeX, MATLAB_HDR.SizeY, CellType, ldblk, BImgBuff, &MinVal, &MaxVal);
      }

      if (CellType==miDOUBLE)
        for (i = 0; i < (ssize_t) MATLAB_HDR.SizeY; i++)
  {
          ReadBlobDoublesXXX(image2, ldblk, (double *)BImgBuff);
          InsertComplexDoubleRow(image, (double *)BImgBuff, i, MinVal, MaxVal,
            exception);
  }

      if (CellType==miSINGLE)
        for (i = 0; i < (ssize_t) MATLAB_HDR.SizeY; i++)
  {
          ReadBlobFloatsXXX(image2, ldblk, (float *)BImgBuff);
          InsertComplexFloatRow(image,(float *)BImgBuff,i,MinVal,MaxVal,
            exception);
  }
    }

      /* Image is gray when no complex flag is set and 2D Matrix AGAIN!!! */
    if ((MATLAB_HDR.DimFlag == 8) &&
        ((MATLAB_HDR.StructureFlag & FLAG_COMPLEX) == 0))
      image->type=GrayscaleType;
    if (image->depth == 1)
      image->type=BilevelType;

    if(image2==image)
        image2 = NULL;    /* Remove shadow copy to an image before rotation. */

      /*  Rotate image. */
    rotated_image = RotateImage(image, 90.0, exception);
    if (rotated_image != (Image *) NULL)
    {
        /* Remove page offsets added by RotateImage */
      rotated_image->page.x=0;
      rotated_image->page.y=0;

      blob = rotated_image->blob;
      rotated_image->blob = image->blob;
      rotated_image->colors = image->colors;
      image->blob = blob;
      AppendImageToList(&image,rotated_image);
      DeleteImageFromList(&image);
    }

done_reading:

    if(image2!=NULL)
      if(image2!=image)
      {
        DeleteImageFromList(&image2);
  if(clone_info)
  {
          if(clone_info->file)
    {
            fclose(clone_info->file);
            clone_info->file = NULL;
            (void) remove_utf8(clone_info->filename);
    }
        }
      }

      /* Allocate next image structure. */
    AcquireNextImage(image_info,image,exception);
    if (image->next == (Image *) NULL) break;
    image=SyncNextImageInList(image);
    image->columns=image->rows=0;
    image->colors=0;

      /* row scan buffer is no longer needed */
    RelinquishMagickMemory(BImgBuff);
    BImgBuff = NULL;

    if(--Frames>0)
    {
      z = z2;
      if(image2==NULL) image2 = image;
      goto NEXT_FRAME;
    }
    if ((image2!=NULL) && (image2!=image))   /* Does shadow temporary decompressed image exist? */
      {
/*  CloseBlob(image2); */
        DeleteImageFromList(&image2);
        if(clone_info)
        {
          if(clone_info->file)
          {
            fclose(clone_info->file);
            clone_info->file = NULL;
            (void) remove_utf8(clone_info->filename);
          }
        }
        }
  }

  RelinquishMagickMemory(BImgBuff);
  if (quantum_info != (QuantumInfo *) NULL)
    quantum_info=DestroyQuantumInfo(quantum_info);
END_OF_READING:
  if (clone_info)
    clone_info=DestroyImageInfo(clone_info);
  CloseBlob(image);


  {
    Image *p;
    ssize_t scene=0;

    /*
      Rewind list, removing any empty images while rewinding.
    */
    p=image;
    image=NULL;
    while (p != (Image *) NULL)
      {
        Image *tmp=p;
        if ((p->rows == 0) || (p->columns == 0)) {
          p=p->previous;
          DeleteImageFromList(&tmp);
        } else {
          image=p;
          p=p->previous;
        }
      }

    /*
      Fix scene numbers
    */
    for (p=image; p != (Image *) NULL; p=p->next)
      p->scene=scene++;
  }

  if(clone_info != NULL)  /* cleanup garbage file from compression */
  {
    if(clone_info->file)
    {
      fclose(clone_info->file);
      clone_info->file = NULL;
      (void) remove_utf8(clone_info->filename);
    }
    DestroyImageInfo(clone_info);
    clone_info = NULL;
  }
  if (logging) (void)LogMagickEvent(CoderEvent,GetMagickModule(),"return");
  if ((image != image2) && (image2 != (Image *) NULL))
    image2=DestroyImage(image2);
  if(image==NULL)
    ThrowReaderException(CorruptImageError,"ImproperImageHeader");
  return (image);
}