int GmfSetKwd(int64_t MshIdx, int KwdCod, int64_t NmbLin, ...)
{
   int         i, *TypTab;
   int64_t     CurPos;
   va_list     VarArg;
   GmfMshSct   *msh = (GmfMshSct *)MshIdx;
   KwdSct      *kwd;

   RecBlk(msh, msh->buf, 0);

   if( (KwdCod < 1) || (KwdCod > GmfMaxKwd) )
      return(0);

   kwd = &msh->KwdTab[ KwdCod ];

   // Read further arguments if this kw is a solution
   if(!strcmp(GmfKwdFmt[ KwdCod ][2], "sr")
   || !strcmp(GmfKwdFmt[ KwdCod ][2], "hr"))
   {
      va_start(VarArg, NmbLin);

      kwd->NmbTyp = va_arg(VarArg, int);
      TypTab = va_arg(VarArg, int *);

      for(i=0;i<kwd->NmbTyp;i++)
         kwd->TypTab[i] = TypTab[i];

      // Add two extra paramaters for HO elements: degree and nmb nodes
      if(!strcmp("hr", GmfKwdFmt[ KwdCod ][2]))
      {
         kwd->deg = va_arg(VarArg, int);
         kwd->NmbNod = va_arg(VarArg, int);
      }

      va_end(VarArg);
   }

   // Setup the kwd info
   ExpFmt(msh, KwdCod);

   if(!kwd->typ)
      return(0);
   else if(kwd->typ == InfKwd)
      kwd->NmbLin = 1;
   else
      kwd->NmbLin = NmbLin;

   // Store the next kwd position in binary file
   if( (msh->typ & Bin) && msh->NexKwdPos )
   {
      CurPos = GetFilPos(msh);

      if(!SetFilPos(msh, msh->NexKwdPos))
         return(0);

      SetPos(msh, CurPos);

      if(!SetFilPos(msh, CurPos))
         return(0);
   }

   // Write the header
   if(msh->typ & Asc)
   {
      fprintf(msh->hdl, "\n%s\n", GmfKwdFmt[ KwdCod ][0]);

      if(kwd->typ != InfKwd)
         fprintf(msh->hdl, INT64_T_FMT"\n", kwd->NmbLin);

      // In case of solution field, write the extended header
      if(kwd->typ == SolKwd)
      {
         fprintf(msh->hdl, "%d ", kwd->NmbTyp);

         for(i=0;i<kwd->NmbTyp;i++)
            fprintf(msh->hdl, "%d ", kwd->TypTab[i]);

         fprintf(msh->hdl, "\n");
      }

      if(!strcmp("hr", GmfKwdFmt[ KwdCod ][2]))
         fprintf(msh->hdl, "%d %d\n", kwd->deg, kwd->NmbNod);
   }
   else
   {
      RecWrd(msh, (unsigned char *)&KwdCod);
      msh->NexKwdPos = GetFilPos(msh);
      SetPos(msh, 0);

      if(kwd->typ != InfKwd)
      {
         if(msh->ver < 4)
         {
            i = (int)kwd->NmbLin;
            RecWrd(msh, (unsigned char *)&i);
         }
         else
            RecDblWrd(msh, (unsigned char *)&kwd->NmbLin);
      }

      // In case of solution field, write the extended header at once
      if(kwd->typ == SolKwd)
      {
         RecWrd(msh, (unsigned char *)&kwd->NmbTyp);

         for(i=0;i<kwd->NmbTyp;i++)
            RecWrd(msh, (unsigned char *)&kwd->TypTab[i]);

         if(!strcmp("hr", GmfKwdFmt[ KwdCod ][2]))
         {
            RecWrd(msh, (unsigned char *)&kwd->deg);
            RecWrd(msh, (unsigned char *)&kwd->NmbNod);
         }
      }
   }

   // Reset write buffer position
   msh->pos = 0;

   // Compute the total file size and check if it crosses the 2GB threshold
   msh->siz += kwd->NmbLin * kwd->NmbWrd * WrdSiz;

   return(1);
}