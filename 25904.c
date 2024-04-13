int GmfGotoKwd(int64_t MshIdx, int KwdCod)
{
   GmfMshSct   *msh = (GmfMshSct *)MshIdx;
   KwdSct      *kwd = &msh->KwdTab[ KwdCod ];

   if( (KwdCod < 1) || (KwdCod > GmfMaxKwd) || !kwd->NmbLin )
      return(0);

   return(SetFilPos(msh, kwd->pos));
}