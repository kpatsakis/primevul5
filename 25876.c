int GmfWriteByteFlow(int64_t MshIdx, char *BytTab, int NmbByt)
{
   int i, PadWrd = 0, *WrdTab = (int *)BytTab, NmbWrd = NmbByt / WrdSiz;

   // Add an extra padding word at the end if needed
   if(NmbByt > NmbWrd * 4)
      PadWrd = 1;

   // Create the keyword with the number of words, not bytes
   if(!GmfSetKwd(MshIdx, GmfByteFlow, NmbWrd + PadWrd))
      return(0);

   // Reacord the exact number of bytes
   GmfSetLin(MshIdx, GmfByteFlow, NmbByt);

   // Write the byteflow as 4-byte words, missing up to 3 endding bytes
   for(i=0;i<NmbWrd;i++)
      GmfSetLin(MshIdx, GmfByteFlow, WrdTab[i]);

   // Write the extra 1,2 or 3 ending bytes
   if(PadWrd)
   {
      PadWrd = 0;

      // Copy the last bytes in an integer
      for(i=0; i<NmbByt - NmbWrd * 4; i++)
         PadWrd |= BytTab[ NmbWrd * 4 + i ] << (i*8);

      // And write it as the last line
      GmfSetLin(MshIdx, GmfByteFlow, PadWrd);
   }

   return(1);
}