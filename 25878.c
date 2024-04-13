char *GmfReadByteFlow(int64_t MshIdx, int *NmbByt)
{
   int         cod, *WrdTab;
   size_t      i, NmbWrd;
   GmfMshSct   *msh = (GmfMshSct *)MshIdx;

   // Read and allocate the number of 4-byte words in the byteflow
   if(!(NmbWrd = GmfStatKwd(MshIdx, GmfByteFlow)))
      return(NULL);

   if(!(WrdTab = malloc(NmbWrd * WrdSiz)))
      return(NULL);

   // Disable the endianess conversion
   cod = msh->cod;
   msh->cod = 1;

   // Read the exact number of bytes in the byteflow
   GmfGotoKwd(MshIdx, GmfByteFlow);
   GmfGetLin(MshIdx, GmfByteFlow, NmbByt);

   // Read the byteflow as 4-byte blocks
   for(i=0;i<NmbWrd;i++)
      GmfGetLin(MshIdx, GmfByteFlow, &WrdTab[i]);

   // Enable endianess convertion
   msh->cod = cod;

   return((char *)WrdTab);
}