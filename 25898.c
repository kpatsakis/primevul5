int GmfCpyLin(int64_t InpIdx, int64_t OutIdx, int KwdCod)
{
   char        s[ WrdSiz * FilStrSiz ];
   double      d;
   float       f;
   int         i, a, err;
   int64_t     l;
   GmfMshSct   *InpMsh = (GmfMshSct *)InpIdx, *OutMsh = (GmfMshSct *)OutIdx;
   KwdSct      *kwd = &InpMsh->KwdTab[ KwdCod ];

   // Save the current stack environment for longjmp
   if( (err = setjmp(InpMsh->err)) != 0)
   {
#ifdef GMFDEBUG
      printf("libMeshb : mesh %p : error %d\n", InpMsh, err);
#endif
      return(0);
   }

   for(i=0;i<kwd->SolSiz;i++)
   {
      if(kwd->fmt[i] == 'r')
      {
         if(InpMsh->FltSiz == 32)
         {
            if(InpMsh->typ & Asc)
               safe_fscanf(InpMsh->hdl, "%f", &f, InpMsh->err);
            else
               ScaWrd(InpMsh, (unsigned char *)&f);

            d = (double)f;
         }
         else
         {
            if(InpMsh->typ & Asc)
               safe_fscanf(InpMsh->hdl, "%lf", &d, InpMsh->err);
            else
               ScaDblWrd(InpMsh, (unsigned char *)&d);

            f = (float)d;
         }

         if(OutMsh->FltSiz == 32)
            if(OutMsh->typ & Asc)
               fprintf(OutMsh->hdl, "%.9g ", (double)f);
            else
               RecWrd(OutMsh, (unsigned char *)&f);
         else
            if(OutMsh->typ & Asc)
               fprintf(OutMsh->hdl, "%.17g ", d);
            else
               RecDblWrd(OutMsh, (unsigned char *)&d);
      }
      else if(kwd->fmt[i] == 'i')
      {
         if(InpMsh->ver <= 3)
         {
            if(InpMsh->typ & Asc)
               safe_fscanf(InpMsh->hdl, "%d", &a, InpMsh->err);
            else
               ScaWrd(InpMsh, (unsigned char *)&a);

            l = (int64_t)a;
         }
         else
         {
            if(InpMsh->typ & Asc)
               safe_fscanf(InpMsh->hdl, INT64_T_FMT, &l, InpMsh->err);
            else
               ScaDblWrd(InpMsh, (unsigned char *)&l);

            a = (int)l;
         }

         if( (i == kwd->SolSiz-1) && (a > GmfMaxRefTab[ KwdCod ]) )
            GmfMaxRefTab[ KwdCod ] = a;

         if(OutMsh->ver <= 3)
         {
            if(OutMsh->typ & Asc)
               fprintf(OutMsh->hdl, "%d ", a);
            else
               RecWrd(OutMsh, (unsigned char *)&a);
         }
         else
         {
            if(OutMsh->typ & Asc)
               fprintf(OutMsh->hdl, INT64_T_FMT" ", l);
            else
               RecDblWrd(OutMsh, (unsigned char *)&l);
         }
      }
      else if(kwd->fmt[i] == 'c')
      {
         memset(s, 0, FilStrSiz * WrdSiz);

         if(InpMsh->typ & Asc)
            safe_fgets(s, WrdSiz * FilStrSiz, InpMsh->hdl, InpMsh->err);
         else
#ifdef WITH_GMF_AIO
            read(InpMsh->FilDes, s, WrdSiz * FilStrSiz);
#else
            safe_fread(s, WrdSiz, FilStrSiz, InpMsh->hdl, InpMsh->err);
#endif
         if(OutMsh->typ & Asc)
            fprintf(OutMsh->hdl, "%s ", s);
         else
#ifdef WITH_GMF_AIO
            write(OutMsh->FilDes, s, WrdSiz * FilStrSiz);
#else
            fwrite(s, WrdSiz, FilStrSiz, OutMsh->hdl);
#endif
      }
   }

   if(OutMsh->typ & Asc)
      fprintf(OutMsh->hdl, "\n");

   return(1);
}