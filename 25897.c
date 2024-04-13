int NAMF77(GmfSetLin, gmfsetlin)(TYPF77(int64_t) MshIdx, TYPF77(int) KwdCod, ...)
{
   int         i, pos, *IntBuf, err;
   int64_t     *LngBuf;
   float       *FltSolTab, *FltBuf;
   double      *DblSolTab, *DblBuf;
   va_list     VarArg;
   GmfMshSct   *msh = (GmfMshSct *) VALF77(MshIdx);
   KwdSct      *kwd = &msh->KwdTab[ VALF77(KwdCod) ];

   if( ( VALF77(KwdCod) < 1) || ( VALF77(KwdCod) > GmfMaxKwd) )
      return(0);

   // Save the current stack environment for longjmp
   // This is needed in RecBlk()
   if( (err = setjmp(msh->err)) != 0)
   {
#ifdef GMFDEBUG
      printf("libMeshb : mesh %p : error %d\n", msh, err);
#endif
      return(0);
   }

   // Start decoding the arguments
   va_start(VarArg, KwdCod);

   if(kwd->typ != SolKwd)
   {
      if(msh->typ & Asc)
      {
         for(i=0;i<kwd->SolSiz;i++)
         {
            if(kwd->fmt[i] == 'r')
            {
               if(msh->FltSiz == 32)
#ifdef F77API
                  fprintf(msh->hdl, "%.9g ", *(va_arg(VarArg, float *)));
#else
                  fprintf(msh->hdl, "%.9g ", va_arg(VarArg, double));
#endif
               else
                  fprintf(msh->hdl, "%.17g ", VALF77(va_arg(VarArg, TYPF77(double))));
            }
            else if(kwd->fmt[i] == 'i')
            {
               if(msh->ver <= 3)
                  fprintf(msh->hdl, "%d ", VALF77(va_arg(VarArg, TYPF77(int))));
               else
               {
                  // [Bruno] %ld -> INT64_T_FMT
                  fprintf( msh->hdl, INT64_T_FMT " ",
                           VALF77(va_arg(VarArg, TYPF77(int64_t))));
               }
            }
            else if(kwd->fmt[i] == 'c')
               fprintf(msh->hdl, "%s ", va_arg(VarArg, char *));
         }
      }
      else
      {
         pos = 0;

         for(i=0;i<kwd->SolSiz;i++)
         {
            if(kwd->fmt[i] == 'r')
            {
               if(msh->FltSiz == 32)
               {
                  FltBuf = (void *)&msh->buf[ pos ];
#ifdef F77API
                  *FltBuf = (float)*(va_arg(VarArg, float *));
#else
                  *FltBuf = (float)va_arg(VarArg, double);
#endif
                  pos += 4;
               }
               else
               {
                  DblBuf = (void *)&msh->buf[ pos ];
                  *DblBuf = VALF77(va_arg(VarArg, TYPF77(double)));
                  pos += 8;
               }
            }
            else if(kwd->fmt[i] == 'i')
            {
               if(msh->ver <= 3)
               {
                  IntBuf = (void *)&msh->buf[ pos ];
                  *IntBuf = VALF77(va_arg(VarArg, TYPF77(int)));
                  pos += 4;
               }
               else
               {
                  LngBuf = (void *)&msh->buf[ pos ];
                  *LngBuf = VALF77(va_arg(VarArg, TYPF77(int64_t)));
                  pos += 8;
               }
            }
            else if(kwd->fmt[i] == 'c')
            {
               memset(&msh->buf[ pos ], 0, FilStrSiz * WrdSiz);
               strncpy(&msh->buf[ pos ], va_arg(VarArg, char *), FilStrSiz * WrdSiz);
               pos += FilStrSiz;
            }
         }

         RecBlk(msh, msh->buf, kwd->NmbWrd);
      }
   }
   else
   {
      if(msh->FltSiz == 32)
      {
         FltSolTab = va_arg(VarArg, float *);

         if(msh->typ & Asc)
            for(i=0; i<kwd->SolSiz; i++)
               fprintf(msh->hdl, "%.9g ", (double)FltSolTab[i]);
         else
            RecBlk(msh, (unsigned char *)FltSolTab, kwd->NmbWrd);
      }
      else
      {
         DblSolTab = va_arg(VarArg, double *);

         if(msh->typ & Asc)
            for(i=0; i<kwd->SolSiz; i++)
               fprintf(msh->hdl, "%.17g ", DblSolTab[i]);
         else
            RecBlk(msh, (unsigned char *)DblSolTab, kwd->NmbWrd);
      }
   }

   va_end(VarArg);

   if(msh->typ & Asc)
      fprintf(msh->hdl, "\n");

   return(1);
}