int GmfGetFloatPrecision(int64_t MshIdx)
{
   int FltSiz;
   GmfMshSct *msh = (GmfMshSct *)MshIdx;

   if(GmfStatKwd(MshIdx, GmfFloatingPointPrecision))
   {
      GmfGotoKwd(MshIdx, GmfFloatingPointPrecision);
      GmfGetLin(MshIdx, GmfFloatingPointPrecision, &FltSiz);

      if(FltSiz == 32 || FltSiz == 64)
         msh->FltSiz = FltSiz;
   }

   return(msh->FltSiz);
}