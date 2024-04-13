int NAMF77(GmfSetBlock, gmfsetblock)(  TYPF77(int64_t) MshIdx,
                                       TYPF77(int)     KwdCod,
                                       TYPF77(int64_t) BegIdx,
                                       TYPF77(int64_t) EndIdx,
                                       TYPF77(int)     MapTyp,
                                       void           *MapTab,
                                       void           *prc, ... )
{
   char        *UsrDat[ GmfMaxTyp ], *UsrBas[ GmfMaxTyp ];
   char        *StrTab[5] = { "", "%.9g", "%.17g", "%d", "%lld" }, *FilPos;
   char        *FilBuf = NULL, *FrtBuf = NULL, *BckBuf = NULL;
   char        **BegTab, **EndTab, *BegUsrDat, *EndUsrDat;
   int         i, j, *FilPtrI32, *UsrPtrI32, FilTyp[ GmfMaxTyp ];
   int         UsrTyp[ GmfMaxTyp ], TypSiz[5] = {0,4,8,4,8};
   int         err, *IntMapTab = NULL, typ, mod = GmfArgLst;
   int         *TypTab, *SizTab, IniFlg = 1, TotSiz = 0, VecCnt, ArgCnt = 0;
   float       *FilPtrR32, *UsrPtrR32;
   double      *FilPtrR64, *UsrPtrR64;
   int64_t     UsrNmbLin, BlkNmbLin = 0, BlkBegIdx, BlkEndIdx = 0;
   int64_t     *FilPtrI64, *UsrPtrI64, *LngMapTab = NULL, OldIdx = 0;
   size_t      FilBegIdx = VALF77(BegIdx), FilEndIdx = VALF77(EndIdx);
   void        (*UsrPrc)(int64_t, int64_t, void *) = NULL;
   size_t      UsrLen[ GmfMaxTyp ], ret, LinSiz, VecLen, s, b, NmbBlk;
   va_list     VarArg;
   GmfMshSct   *msh = (GmfMshSct *) VALF77(MshIdx);
   KwdSct      *kwd = &msh->KwdTab[ VALF77(KwdCod) ];
   struct      aiocb aio;
#ifdef F77API
   int         NmbArg = 0;
   void        *ArgTab[ MaxArg ];
#else
   char        *UsrArg = NULL;
#endif

   // Save the current stack environment for longjmp
   if( (err = setjmp(msh->err)) != 0)
   {
#ifdef GMFDEBUG
      printf("libMeshb : mesh %p : error %d\n", msh, err);
#endif
      if(FilBuf)
         free(FilBuf);

      return(0);
   }

   // Check mesh and keyword
   if( (VALF77(KwdCod) < 1) || (VALF77(KwdCod) > GmfMaxKwd) || !kwd->NmbLin )
      return(0);

   // Make sure it's not a simple information keyword
   if( (kwd->typ != RegKwd) && (kwd->typ != SolKwd) )
      return(0);

   // Temporarily overwright the given begin and end values
   // as arbitrary position block write is not yet implemented
   FilBegIdx = 1;
   FilEndIdx = kwd->NmbLin;

   // Check user's bounds
   if( (FilBegIdx < 1) || (FilBegIdx > FilEndIdx) || (FilEndIdx > (size_t)kwd->NmbLin) )
      return(0);

   // Compute the number of lines to be written
   UsrNmbLin = FilEndIdx - FilBegIdx + 1;

   // Get the renumbering map if any
   if(VALF77(MapTyp) == GmfInt)
      IntMapTab = (int *)MapTab;
   else if(VALF77(MapTyp) == GmfLong)
      LngMapTab = (int64_t *)MapTab;

   // Start decoding the arguments
   va_start(VarArg, prc);
   LinSiz = 0;

   // Get the user's postprocessing procedure and argument adresses, if any
#ifdef F77API
   if(prc)
   {
      UsrPrc = (void (*)(int64_t, int64_t, void *))prc;
      NmbArg = *(va_arg(VarArg, int *));

      for(i=0;i<NmbArg;i++)
         ArgTab[i] = va_arg(VarArg, void *);
   }
#else
   if(prc)
   {
      UsrPrc = (void (*)(int64_t, int64_t, void *))prc;
      UsrArg = va_arg(VarArg, void *);
   }
#endif

   if( (kwd->typ != RegKwd) && (kwd->typ != SolKwd) )
      return(0);

   // Read the first data type to select between list and table mode
   typ = VALF77(va_arg(VarArg, TYPF77(int)));

   // If the table mode is selected, read the four additional tables
   // containing the arguments: type, vector size, begin and end pointers
   if(typ == GmfArgTab)
   {
      mod = GmfArgTab;
      TypTab = va_arg(VarArg, int *);
      SizTab = va_arg(VarArg, int *);
      BegTab = va_arg(VarArg, char **);
      EndTab = va_arg(VarArg, char **);
   }

   // Read the arguments until to total size reaches the keyword's size
   while(TotSiz < kwd->SolSiz)
   {
      // In list mode all arguments are read from the variable argument buffer
      if(mod == GmfArgLst)
      {
         // Do not read the type argument for the first iteration because
         // it was read befeore the loop begins to get the argument mode
         if(IniFlg)
            IniFlg = 0;
         else
            typ = VALF77(va_arg(VarArg, TYPF77(int)));

         // In case the type is a vector. get its size and change the type
         // for the corresponding scalar type
         if(typ >= GmfFloatVec && typ <= GmfLongVec)
         {
            typ -= 4;
            VecCnt = VALF77(va_arg(VarArg, TYPF77(int)));
         }
         else
            VecCnt = 1;

         BegUsrDat = va_arg(VarArg, char *);
         EndUsrDat = va_arg(VarArg, char *);
      }
      else
      {
         // Do exactly the same as above but the arguments are read from
         // the tables instead of VarArgs
         typ = TypTab[ ArgCnt ];

         if(typ >= GmfFloatVec && typ <= GmfLongVec)
         {
            typ -= 4;
            VecCnt = SizTab[ ArgCnt ];
         }
         else
            VecCnt = 1;

         BegUsrDat = (char *)BegTab[ ArgCnt ];
         EndUsrDat = (char *)EndTab[ ArgCnt ];
         ArgCnt++;
      }

      if(UsrNmbLin > 1)
         VecLen = (size_t)(EndUsrDat - BegUsrDat) / (UsrNmbLin - 1);
      else
         VecLen = 0;

      // Compute the consecutive begin / end adresses for vector data types
      for(i=0;i<VecCnt;i++)
      {
         UsrTyp[ TotSiz ]  = typ;
         UsrBas[ TotSiz ]  = BegUsrDat + i * TypSiz[ typ ];
         UsrDat[ TotSiz ]  = UsrBas[ TotSiz ];
         UsrLen[ TotSiz ]  = VecLen;
         TotSiz++;
      }
   }

   // Get the file's data type
   for(i=0;i<kwd->SolSiz;i++)
   {
      if(kwd->fmt[i] == 'r')
         if(msh->FltSiz == 32)
            FilTyp[i] = GmfFloat;
         else
            FilTyp[i] = GmfDouble;
      else
         if(msh->ver <= 3)
            FilTyp[i] = GmfInt;
         else
            FilTyp[i] = GmfLong;

      // Compute the file stride
      LinSiz += TypSiz[ FilTyp[i] ];
   }

   va_end(VarArg);

   // Write the whole kwd data
   if(msh->typ & Asc)
   {
      if(UsrPrc)
#ifdef F77API
         CalF77Prc(1, kwd->NmbLin, UsrPrc, NmbArg, ArgTab);
#else
         UsrPrc(1, kwd->NmbLin, UsrArg);
#endif

      for(s=FilBegIdx; s<=FilEndIdx; s++)
         for(j=0;j<kwd->SolSiz;j++)
         {
            if(UsrTyp[j] == GmfFloat)
            {
               UsrPtrR32 = (float *)UsrDat[j];
               fprintf(msh->hdl, StrTab[ UsrTyp[j] ], (double)*UsrPtrR32);
            }
            else if(UsrTyp[j] == GmfDouble)
            {
               UsrPtrR64 = (double *)UsrDat[j];
               fprintf(msh->hdl, StrTab[ UsrTyp[j] ], *UsrPtrR64);
            }
            else if(UsrTyp[j] == GmfInt)
            {
               UsrPtrI32 = (int *)UsrDat[j];
               fprintf(msh->hdl, StrTab[ UsrTyp[j] ], *UsrPtrI32);
            }
            else if(UsrTyp[j] == GmfLong)
            {
               UsrPtrI64 = (int64_t *)UsrDat[j];
               fprintf(msh->hdl, StrTab[ UsrTyp[j] ], *UsrPtrI64);
            }

            if(j < kwd->SolSiz -1)
               fprintf(msh->hdl, " ");
            else
               fprintf(msh->hdl, "\n");

            //UsrDat[j] += UsrLen[j];
            if(IntMapTab)
               UsrDat[j] = UsrBas[j] + IntMapTab[s] * UsrLen[j];
            else if(LngMapTab)
               UsrDat[j] = UsrBas[j] + LngMapTab[s] * UsrLen[j];
            else
               UsrDat[j] = UsrBas[j] + s * UsrLen[j];
         }
   }
   else
   {
      // Allocate the front and back buffers
      if(!(BckBuf = malloc(BufSiz * LinSiz)))
         return(0);

      if(!(FrtBuf = malloc(BufSiz * LinSiz)))
         return(0);

      // Setup the asynchronous parameters
      memset(&aio, 0, sizeof(struct aiocb));
      FilBuf = BckBuf;
#ifdef WITH_GMF_AIO
      aio.aio_fildes = msh->FilDes;
#else
      aio.aio_fildes = msh->hdl;
#endif
      aio.aio_offset = (off_t)GetFilPos(msh);

      NmbBlk = UsrNmbLin / BufSiz;

      // Loop over N+1 blocks
      for(b=0;b<=NmbBlk+1;b++)
      {
         // Launch an asynchronous block write
         // except for the first loop iteration
         if(b)
         {
            aio.aio_nbytes = BlkNmbLin * LinSiz;
            
            if(my_aio_write(&aio) == -1)
            {
#ifdef WITH_GMF_AIO
               printf("aio_fildes = %d\n",aio.aio_fildes);
#else
               printf("aio_fildes = %p\n",aio.aio_fildes);
#endif
               printf("aio_buf    = %p\n",aio.aio_buf);
               printf("aio_offset = " INT64_T_FMT "\n",(int64_t)aio.aio_offset);
               printf("aio_nbytes = " INT64_T_FMT "\n",(int64_t)aio.aio_nbytes);
               printf("errno      = %d\n",errno);
               exit(1);
            }
         }

         // Parse the block data except at the last loop iteration
         if(b<=NmbBlk)
         {
            // The last block is shorter
            if(b == NmbBlk)
               BlkNmbLin = UsrNmbLin - b * BufSiz;
            else
               BlkNmbLin = BufSiz;

            FilPos = FilBuf;
            BlkBegIdx = BlkEndIdx+1;
            BlkEndIdx += BlkNmbLin;

            // Call user's preprocessing first
            if(UsrPrc)
#ifdef F77API
               CalF77Prc(BlkBegIdx, BlkEndIdx, UsrPrc, NmbArg, ArgTab);
#else
               UsrPrc(BlkBegIdx, BlkEndIdx, UsrArg);
#endif

            // Then copy it's data to the file buffer
            for(i=0;i<BlkNmbLin;i++)
            {
               OldIdx++;

               for(j=0;j<kwd->SolSiz;j++)
               {
                  if(IntMapTab)
                     UsrDat[j] = UsrBas[j] + (IntMapTab[ OldIdx ] - 1) * UsrLen[j];
                  else if(LngMapTab)
                     UsrDat[j] = UsrBas[j] + (LngMapTab[ OldIdx ] - 1) * UsrLen[j];
                  else
                     UsrDat[j] = UsrBas[j] + (OldIdx - 1) * UsrLen[j];

                  if(FilTyp[j] == GmfInt)
                  {
                     FilPtrI32 = (int *)FilPos;

                     if(UsrTyp[j] == GmfInt)
                     {
                        UsrPtrI32 = (int *)UsrDat[j];
                        *FilPtrI32 = *UsrPtrI32;
                     }
                     else
                     {
                        UsrPtrI64 = (int64_t *)UsrDat[j];
                        *FilPtrI32 = (int)*UsrPtrI64;
                     }
                  }
                  else if(FilTyp[j] == GmfLong)
                  {
                     FilPtrI64 = (int64_t *)FilPos;

                     if(UsrTyp[j] == GmfLong)
                     {
                        UsrPtrI64 = (int64_t *)UsrDat[j];
                        *FilPtrI64 = *UsrPtrI64;
                     }
                     else
                     {
                        UsrPtrI32 = (int *)UsrDat[j];
                        *FilPtrI64 = (int64_t)*UsrPtrI32;
                     }
                  }
                  else if(FilTyp[j] == GmfFloat)
                  {
                     FilPtrR32 = (float *)FilPos;

                     if(UsrTyp[j] == GmfFloat)
                     {
                        UsrPtrR32 = (float *)UsrDat[j];
                        *FilPtrR32 = *UsrPtrR32;
                     }
                     else
                     {
                        UsrPtrR64 = (double *)UsrDat[j];
                        *FilPtrR32 = (float)*UsrPtrR64;
                     }
                  }
                  else if(FilTyp[j] == GmfDouble)
                  {
                     FilPtrR64 = (double *)FilPos;

                     if(UsrTyp[j] == GmfDouble)
                     {
                        UsrPtrR64 = (double *)UsrDat[j];
                        *FilPtrR64 = *UsrPtrR64;
                     }
                     else
                     {
                        UsrPtrR32 = (float *)UsrDat[j];
                        *FilPtrR64 = (double)*UsrPtrR32;
                     }
                  }

                  FilPos += TypSiz[ FilTyp[j] ];
               }
            }
         }

         // Wait for write completion execpt at the first loop iteration
         if(b)
         {
            while(my_aio_error(&aio) == EINPROGRESS);

            err = my_aio_error(&aio);
            ret = my_aio_return(&aio);

            if (err != 0) {
              printf (" Error at aio_error() : %s\n", strerror (err));
              exit(1);
            }

            if (ret != aio.aio_nbytes) {
              printf(" Error at aio_return()\n");
              exit(1);
            }

            // Move the write position
            aio.aio_offset += (off_t)aio.aio_nbytes;
         }

         // Swap the buffers
         if(FilBuf == BckBuf)
         {
            aio.aio_buf = BckBuf;
            FilBuf = FrtBuf;
         }
         else
         {
            aio.aio_buf = FrtBuf;
            FilBuf = BckBuf;
         }
      }

      SetFilPos(msh, aio.aio_offset);
      free(BckBuf);
      free(FrtBuf);
   }

   return(1);
}