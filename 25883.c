int APIF77(gmfsetkwd)(  int64_t *MshIdx, int *KwdIdx, int *NmbLin,
                        int *NmbTyp, int *TypTab, int *deg, int *NmbNod)
{
   if(!strcmp(GmfKwdFmt[ *KwdIdx ][2], "hr"))
      return(GmfSetKwd(*MshIdx, *KwdIdx, *NmbLin, *NmbTyp, TypTab, *deg, *NmbNod));
   else if(!strcmp(GmfKwdFmt[ *KwdIdx ][2], "sr"))
      return(GmfSetKwd(*MshIdx, *KwdIdx, *NmbLin, *NmbTyp, TypTab));
   else
      return(GmfSetKwd(*MshIdx, *KwdIdx, *NmbLin));
}