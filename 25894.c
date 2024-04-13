int NAMF77(GmfGetBlock, gmfgetblock)(  TYPF77(int64_t) MshIdx,
                                       TYPF77(int)     KwdCod,
                                       TYPF77(int64_t) BegIdx,
                                       TYPF77(int64_t) EndIdx,
                                       TYPF77(int)     MapTyp,
                                       void           *MapTab,
                                       void           *prc, ... )
{
   char        *UsrDat[ GmfMaxTyp ], *UsrBas[ GmfMaxTyp ], *FilPos, *EndUsrDat;
   char        *FilBuf = NULL, *FrtBuf = NULL, *BckBuf = NULL, *BegUsrDat;
   char        *StrTab[5] = { "", "%f", "%lf", "%d", INT64_T_FMT };
   char        **BegTab, **EndTab;
   int         i, j, k, *FilPtrI32, *UsrPtrI32, FilTyp[ GmfMaxTyp ];
   int         UsrTyp[ GmfMaxTyp ], TypSiz[5] = {0,4,8,4,8};
   int         *IntMapTab = NULL, err, TotSiz = 0, IniFlg = 1, mod = GmfArgLst;
   int         *TypTab, *SizTab, typ, VecCnt, ArgCnt = 0;
   float       *FilPtrR32, *UsrPtrR32;
   double      *FilPtrR64, *UsrPtrR64;
   int64_t     BlkNmbLin, *FilPtrI64, *UsrPtrI64, BlkBegIdx, BlkEndIdx = 0;
   int64_t     *LngMapTab = NULL, OldIdx = 0, UsrNmbLin, VecLen;
   size_t      FilBegIdx = VALF77(BegIdx), FilEndIdx = VALF77(EndIdx);
   void        (*UsrPrc)(int64_t, int64_t, void *) = NULL;
   size_t      UsrLen[ GmfMaxTyp ], ret, LinSiz, b, NmbBlk;
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
      if(BckBuf)
         free(BckBuf);

      if(FrtBuf)
         free(FrtBuf);

      return(0);
   }

   // Check mesh and keyword
   if( (VALF77(KwdCod) < 1) || (VALF77(KwdCod) > GmfMaxKwd) || !kwd->NmbLin )
      return(0);

   // Make sure it's not a simple information keyword
   if( (kwd->typ != RegKwd) && (kwd->typ != SolKwd) )
      return(0);

   // Check user's bounds
   if( (FilBegIdx < 1) || (FilBegIdx > FilEndIdx) || (FilEndIdx > (size_t)kwd->NmbLin) )
      return(0);

   // Compute the number of lines to be read
   UsrNmbLin = FilEndIdx - FilBegIdx + 1;

   // Get the renumbering map if any
   if(VALF77(MapTyp) == GmfInt)
      IntMapTab = (int *)MapTab;
   else if(VALF77(MapTyp) == GmfLong)
      LngMapTab = (int64_t *)MapTab;

   // Start decoding the arguments
   va_start(VarArg, prc);
   LinSiz = 0;

   // Get the user's preprocessing procedure and argument adresses, if any
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

   // Move file pointer to the keyword data
   SetFilPos(msh, kwd->pos);

   // Read the whole kwd data
   if(msh->typ & Asc)
   {
      OldIdx = 1;

      for(i=1;i<=FilEndIdx;i++)
      {
         for(j=0;j<kwd->SolSiz;j++)
         {
            // Reorder HO nodes on the fly
            if(kwd->OrdTab && (j != kwd->SolSiz-1))
               k = kwd->OrdTab[j];
            else
               k = j;

            // Move to the next user's data line only when the desired
            // begining position in the ascii file has been reached since
            // we cannot move directly to an arbitrary position
            if(IntMapTab)
               UsrDat[j] = UsrBas[k] + (IntMapTab[ OldIdx ] - 1) * UsrLen[k];
            else if(LngMapTab)
               UsrDat[j] = UsrBas[k] + (LngMapTab[ OldIdx ] - 1) * UsrLen[k];
            else
               UsrDat[j] = UsrBas[k] + (OldIdx - 1) * UsrLen[k];

            safe_fscanf(msh->hdl, StrTab[ UsrTyp[j] ], UsrDat[j], msh->err);
         }

         if(i >= FilBegIdx)
            OldIdx++;

         // Call the user's preprocessing procedure
         if(UsrPrc)
#ifdef F77API
            CalF77Prc(1, kwd->NmbLin, UsrPrc, NmbArg, ArgTab);
#else
            UsrPrc(1, kwd->NmbLin, UsrArg);
#endif
      }
   }
   else
   {
      // Allocate both front and back buffers
      if(!(BckBuf = malloc(BufSiz * LinSiz)))
         return(0);

      if(!(FrtBuf = malloc(BufSiz * LinSiz)))
         return(0);

      // Setup the ansynchonous parameters
      memset(&aio, 0, sizeof(struct aiocb));
      FilBuf = BckBuf;
      aio.aio_buf = BckBuf;
#ifdef WITH_GMF_AIO
      aio.aio_fildes = msh->FilDes;
#else
      aio.aio_fildes = msh->hdl;
#endif
      aio.aio_offset = (off_t)(GetFilPos(msh) + (FilBegIdx-1) * LinSiz);

      NmbBlk = UsrNmbLin / BufSiz;

      // Loop over N+1 blocks
      for(b=0;b<=NmbBlk+1;b++)
      {
         // Wait for the previous block read to complete except
         // for the first loop interation
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

            // Increment the reading position
            aio.aio_offset += (off_t)aio.aio_nbytes;

            // and swap the buffers
            if(aio.aio_buf == BckBuf)
            {
               aio.aio_buf = FrtBuf;
               FilBuf = BckBuf;
            }
            else
            {
               aio.aio_buf = BckBuf;
               FilBuf = FrtBuf;
            }
         }
 
         // Read a chunk of data except for the last loop interarion
         if(b <= NmbBlk)
         {
            // The last block is shorter than the others
            if(b == NmbBlk)
               BlkNmbLin = UsrNmbLin - b * BufSiz;
            else
               BlkNmbLin = BufSiz;

            aio.aio_nbytes = BlkNmbLin * LinSiz;

            if(my_aio_read(&aio) == -1)
            {
               printf("block      = %zd / %zd\n", b+1, NmbBlk+1);
               printf("size       = "INT64_T_FMT" lines\n", BlkNmbLin);
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

         // Then decode the block and store it in the user's data structure
         // except for the first loop interation
         if(b)
         {
            // The last block is shorter than the others
            if(b-1 == NmbBlk)
               BlkNmbLin = UsrNmbLin - (b-1) * BufSiz;
            else
               BlkNmbLin = BufSiz;

            BlkBegIdx = BlkEndIdx+1;
            BlkEndIdx += BlkNmbLin;
            FilPos = FilBuf;

            for(i=0;i<BlkNmbLin;i++)
            {
               OldIdx++;

               for(j=0;j<kwd->SolSiz;j++)
               {
                  if(msh->cod != 1)
                     SwpWrd(FilPos, TypSiz[ FilTyp[j] ]);

                  // Reorder HO nodes on the fly
                  if(kwd->OrdTab && (j != kwd->SolSiz-1))
                     k = kwd->OrdTab[j];
                  else
                     k = j;

                  if(IntMapTab)
                     UsrDat[j] = UsrBas[k] + (IntMapTab[ OldIdx ] - 1) * UsrLen[k];
                  else if(LngMapTab)
                     UsrDat[j] = UsrBas[k] + (LngMapTab[ OldIdx ] - 1) * UsrLen[k];
                  else
                     UsrDat[j] = UsrBas[k] + (OldIdx - 1) * UsrLen[k];

                  if(FilTyp[j] == GmfInt)
                  {
                     FilPtrI32 = (int *)FilPos;

                     if(UsrTyp[j] == GmfInt)
                     {
                        UsrPtrI32 = (int *)UsrDat[j];
                        *UsrPtrI32 = *FilPtrI32;
                     }
                     else
                     {
                        UsrPtrI64 = (int64_t *)UsrDat[j];
                        *UsrPtrI64 = (int64_t)*FilPtrI32;
                     }
                  }
                  else if(FilTyp[j] == GmfLong)
                  {
                     FilPtrI64 = (int64_t *)FilPos;

                     if(UsrTyp[j] == GmfLong)
                     {
                        UsrPtrI64 = (int64_t *)UsrDat[j];
                        *UsrPtrI64 = *FilPtrI64;
                     }
                     else
                     {
                        UsrPtrI32 = (int *)UsrDat[j];
                        *UsrPtrI32 = (int)*FilPtrI64;
                     }
                  }
                  else if(FilTyp[j] == GmfFloat)
                  {
                     FilPtrR32 = (float *)FilPos;

                     if(UsrTyp[j] == GmfFloat)
                     {
                        UsrPtrR32 = (float *)UsrDat[j];
                        *UsrPtrR32 = *FilPtrR32;
                     }
                     else
                     {
                        UsrPtrR64 = (double *)UsrDat[j];
                        *UsrPtrR64 = (double)*FilPtrR32;
                     }
                  }
                  else if(FilTyp[j] == GmfDouble)
                  {
                     FilPtrR64 = (double *)FilPos;

                     if(UsrTyp[j] == GmfDouble)
                     {
                        UsrPtrR64 = (double *)UsrDat[j];
                        *UsrPtrR64 = *FilPtrR64;
                     }
                     else
                     {
                        UsrPtrR32 = (float *)UsrDat[j];
                        *UsrPtrR32 = (float)*FilPtrR64;
                     }
                  }

                  FilPos += TypSiz[ FilTyp[j] ];
               }
            }

            // Call the user's preprocessing procedure
            if(UsrPrc)
#ifdef F77API
               CalF77Prc(BlkBegIdx, BlkEndIdx, UsrPrc, NmbArg, ArgTab);
#else
               UsrPrc(BlkBegIdx, BlkEndIdx, UsrArg);
#endif
         }
      }

      free(BckBuf);
      free(FrtBuf);
   }

   return(1);
}