int64_t GmfStatKwd(int64_t MshIdx, int KwdCod, ...)
{
   int         i, *PtrNmbTyp, *PtrSolSiz, *TypTab, *PtrDeg, *PtrNmbNod;
   GmfMshSct   *msh = (GmfMshSct *)MshIdx;
   KwdSct      *kwd;
   va_list     VarArg;

   if( (KwdCod < 1) || (KwdCod > GmfMaxKwd) )
      return(0);

   kwd = &msh->KwdTab[ KwdCod ];

   if(!kwd->NmbLin)
      return(0);

   // Read further arguments if this kw is a sol
   if(kwd->typ == SolKwd)
   {
      va_start(VarArg, KwdCod);

      PtrNmbTyp = va_arg(VarArg, int *);
      *PtrNmbTyp = kwd->NmbTyp;

      PtrSolSiz = va_arg(VarArg, int *);
      *PtrSolSiz = kwd->SolSiz;

      TypTab = va_arg(VarArg, int *);

      for(i=0;i<kwd->NmbTyp;i++)
         TypTab[i] = kwd->TypTab[i];

      // Add two extra paramaters for HO elements: degree and nmb nodes
      if(!strcmp("hr", GmfKwdFmt[ KwdCod ][2]) )
      {
         PtrDeg = va_arg(VarArg, int *);
         *PtrDeg = kwd->deg;
         
         PtrNmbNod = va_arg(VarArg, int *);
         *PtrNmbNod = kwd->NmbNod;
      }

      va_end(VarArg);
   }

   return(kwd->NmbLin);
}