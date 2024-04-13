static void ExpFmt(GmfMshSct *msh, int KwdCod)
{
   int         i, j, TmpSiz=0, IntWrd, FltWrd;
   char        chr;
   const char  *InpFmt = GmfKwdFmt[ KwdCod ][2];
   KwdSct      *kwd = &msh->KwdTab[ KwdCod ];

   // Set the kwd's type
   if(!strlen(GmfKwdFmt[ KwdCod ][1]))
      kwd->typ = InfKwd;
   else if( !strcmp(InpFmt, "sr") || !strcmp(InpFmt, "hr") )
      kwd->typ = SolKwd;
   else
      kwd->typ = RegKwd;

   // Get the solution-field's size
   if(kwd->typ == SolKwd)
      for(i=0;i<kwd->NmbTyp;i++)
         switch(kwd->TypTab[i])
         {
            case GmfSca    : TmpSiz += 1; break;
            case GmfVec    : TmpSiz += msh->dim; break;
            case GmfSymMat : TmpSiz += (msh->dim * (msh->dim+1)) / 2; break;
            case GmfMat    : TmpSiz += msh->dim * msh->dim; break;
         }

   // Scan each character from the format string
   i = kwd->SolSiz = kwd->NmbWrd = 0;

   while(i < (int)strlen(InpFmt))
   {
      chr = InpFmt[ i++ ];

      if(chr == 'd')
      {
         chr = InpFmt[i++];

         for(j=0;j<msh->dim;j++)
            kwd->fmt[ kwd->SolSiz++ ] = chr;
      }
      else if((chr == 's')||(chr == 'h'))
      {
         chr = InpFmt[i++];

         for(j=0;j<TmpSiz;j++)
            kwd->fmt[ kwd->SolSiz++ ] = chr;
      }
      else
         kwd->fmt[ kwd->SolSiz++ ] = chr;
   }

   if(msh->FltSiz == 32)
      FltWrd = 1;
   else
      FltWrd = 2;

   if(msh->ver <= 3)
      IntWrd = 1;
   else
      IntWrd = 2;

   for(i=0;i<kwd->SolSiz;i++)
      switch(kwd->fmt[i])
      {
         case 'i' : kwd->NmbWrd += IntWrd; break;
         case 'c' : kwd->NmbWrd += FilStrSiz; break;
         case 'r' : kwd->NmbWrd += FltWrd;break;
      }

   // HO solution: duplicate the format as many times as the number of nodes
   if( !strcmp(InpFmt, "hr") && (kwd->NmbNod > 1) )
   {
      for(i=1;i<=kwd->NmbNod;i++)
         for(j=0;j<kwd->SolSiz;j++)
            kwd->fmt[ i * kwd->SolSiz + j ] = kwd->fmt[j];

      kwd->SolSiz *= kwd->NmbNod;
      kwd->NmbWrd *= kwd->NmbNod;
   }
}