int APIF77(gmfstatkwd)( int64_t *MshIdx, int *KwdIdx, int *NmbTyp,
                        int *SolSiz, int *TypTab,  int *deg, int *NmbNod)
{
   if(!strcmp(GmfKwdFmt[ *KwdIdx ][2], "hr"))
      return(GmfStatKwd(*MshIdx, *KwdIdx, NmbTyp, SolSiz, TypTab, deg, NmbNod));
   else if(!strcmp(GmfKwdFmt[ *KwdIdx ][2], "sr"))
      return(GmfStatKwd(*MshIdx, *KwdIdx, NmbTyp, SolSiz, TypTab));
   else
      return(GmfStatKwd(*MshIdx, *KwdIdx));
}