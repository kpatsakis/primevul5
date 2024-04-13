int NAMF77(GmfGetLin, gmfgetlin)(TYPF77(int64_t)MshIdx, TYPF77(int)KwdCod, ...)
{
   int         i, err;
   float       *FltSolTab, FltVal, *PtrFlt;
   double      *DblSolTab, *PtrDbl;
   va_list     VarArg;
   GmfMshSct   *msh = (GmfMshSct *) VALF77(MshIdx);
   KwdSct      *kwd = &msh->KwdTab[ VALF77(KwdCod) ];

   if( (VALF77(KwdCod) < 1) || (VALF77(KwdCod) > GmfMaxKwd) )
      return(0);

   // Save the current stack environment for longjmp
   if( (err = setjmp(msh->err)) != 0)
   {
#ifdef GMFDEBUG
      printf("libMeshb : mesh %p : error %d\n", msh, err);
#endif
      return(0);
   }

   // Start decoding the arguments
   va_start(VarArg, KwdCod);

   switch(kwd->typ)
   {
      case InfKwd : case RegKwd : case CmtKwd :
      {
         if(msh->typ & Asc)
         {
            for(i=0;i<kwd->SolSiz;i++)
            {
               if(kwd->fmt[i] == 'r')
               {
                  if(msh->FltSiz == 32)
                  {
                     safe_fscanf(msh->hdl, "%f", &FltVal, msh->err);
                     PtrDbl = va_arg(VarArg, double *);
                     PtrFlt = (float *)PtrDbl;
                     *PtrFlt = FltVal;
                  }                     
                  else
                  {
                     safe_fscanf(msh->hdl, "%lf",
                              va_arg(VarArg, double *), msh->err);
                  }
               }
               else if(kwd->fmt[i] == 'i')
               {
                  if(msh->ver <= 3)
                  {
                     safe_fscanf(msh->hdl, "%d",
                        va_arg(VarArg, int *), msh->err);
                  }
                  else
                  {
                     // [Bruno] %ld -> INT64_T_FMT
                     safe_fscanf(msh->hdl, INT64_T_FMT,
                              va_arg(VarArg, int64_t *), msh->err);
                  }
               }
               else if(kwd->fmt[i] == 'c')
               {
                  safe_fgets( va_arg(VarArg, char *),
                              WrdSiz * FilStrSiz, msh->hdl, msh->err);
               }
            }
         }
         else
         {
            for(i=0;i<kwd->SolSiz;i++)
               if(kwd->fmt[i] == 'r')
                  if(msh->FltSiz == 32)
                     ScaWrd(msh, (unsigned char *)va_arg(VarArg, float *));
                  else
                     ScaDblWrd(msh, (unsigned char *)va_arg(VarArg, double *));
               else if(kwd->fmt[i] == 'i')
                  if(msh->ver <= 3)
                     ScaWrd(msh, (unsigned char *)va_arg(VarArg, int *));
                  else
                     ScaDblWrd(msh, (unsigned char *)va_arg(VarArg, int64_t *));
               else if(kwd->fmt[i] == 'c')
                  // [Bruno] added error control
                  safe_fread(va_arg(VarArg, char *), WrdSiz, FilStrSiz, msh->hdl, msh->err);
         }
      }break;

      case SolKwd :
      {
         if(msh->FltSiz == 32)
         {
            FltSolTab = va_arg(VarArg, float *);

            if(msh->typ & Asc)
               for(i=0; i<kwd->SolSiz; i++)
                  safe_fscanf(msh->hdl, "%f", &FltSolTab[i], msh->err);
            else
               for(i=0; i<kwd->SolSiz; i++)
                  ScaWrd(msh, (unsigned char *)&FltSolTab[i]);
         }
         else
         {
            DblSolTab = va_arg(VarArg, double *);

            if(msh->typ & Asc)
               for(i=0; i<kwd->SolSiz; i++)
                  safe_fscanf(msh->hdl, "%lf", &DblSolTab[i], msh->err);
            else
               for(i=0; i<kwd->SolSiz; i++)
                  ScaDblWrd(msh, (unsigned char *)&DblSolTab[i]);
         }
      }break;
   }

   va_end(VarArg);

   return(1);
}