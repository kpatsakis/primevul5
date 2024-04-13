static void ScaKwdHdr(GmfMshSct *msh, int KwdCod)
{
   int      i;
   KwdSct   *kwd = &msh->KwdTab[ KwdCod ];

   if(!strcmp("i", GmfKwdFmt[ KwdCod ][1]))
      if(msh->typ & Asc)
         safe_fscanf(msh->hdl, INT64_T_FMT, &kwd->NmbLin, msh->err);
      else
         if(msh->ver <= 3)
         {
            ScaWrd(msh, (unsigned char *)&i);
            kwd->NmbLin = i;
         }
         else
            ScaDblWrd(msh, (unsigned char *)&kwd->NmbLin);
   else
      kwd->NmbLin = 1;

   if(!strcmp("sr", GmfKwdFmt[ KwdCod ][2])
   || !strcmp("hr", GmfKwdFmt[ KwdCod ][2]) )
   {
      if(msh->typ & Asc)
      {
         safe_fscanf(msh->hdl, "%d", &kwd->NmbTyp, msh->err);

         for(i=0;i<kwd->NmbTyp;i++)
            safe_fscanf(msh->hdl, "%d", &kwd->TypTab[i], msh->err);

         // Scan two extra fields for HO solutions: deg and nmb Nodes
         if(!strcmp("hr", GmfKwdFmt[ KwdCod ][2]))
         {
            safe_fscanf(msh->hdl, "%d", &kwd->deg, msh->err);
            safe_fscanf(msh->hdl, "%d", &kwd->NmbNod, msh->err);
         }
         else
         {
            kwd->deg = 0;
            kwd->NmbNod = 1;
         }

      }
      else
      {
         ScaWrd(msh, (unsigned char *)&kwd->NmbTyp);

         for(i=0;i<kwd->NmbTyp;i++)
            ScaWrd(msh, (unsigned char *)&kwd->TypTab[i]);

         // Scan two extra fields for HO solutions: deg and nmb Nodes
         if(!strcmp("hr", GmfKwdFmt[ KwdCod ][2]))
         {
            ScaWrd(msh, (unsigned char *)&kwd->deg);
            ScaWrd(msh, (unsigned char *)&kwd->NmbNod);
         }
         else
         {
            kwd->deg = 0;
            kwd->NmbNod = 1;
         }
      }
   }

   ExpFmt(msh, KwdCod);
   kwd->pos = GetFilPos(msh);
}