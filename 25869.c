void GmfSetFloatPrecision(int64_t MshIdx , int FltSiz)
{
   GmfMshSct *msh = (GmfMshSct *)MshIdx;

   if(FltSiz != 32 && FltSiz != 64)
      return;

   msh->FltSiz = FltSiz;
   GmfSetKwd(MshIdx, GmfFloatingPointPrecision, 1);
   GmfSetLin(MshIdx, GmfFloatingPointPrecision, FltSiz);
}