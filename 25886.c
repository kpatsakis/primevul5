int GmfCloseMesh(int64_t MshIdx)
{
   int i, res = 1;
   GmfMshSct *msh = (GmfMshSct *)MshIdx;

   RecBlk(msh, msh->buf, 0);

   // In write down the "End" kw in write mode
   if(msh->mod == GmfWrite)
   {
      if(msh->typ & Asc)
         fprintf(msh->hdl, "\n%s\n", GmfKwdFmt[ GmfEnd ][0]);
      else
         GmfSetKwd(MshIdx, GmfEnd, 0);
   }

   // Close the file and free the mesh structure
   if(msh->typ & Bin)
#ifdef WITH_GMF_AIO
      close(msh->FilDes);
#else
      fclose(msh->hdl);
#endif
   else if(fclose(msh->hdl))
      res = 0;

   // Free optional H.O. renumbering tables
   for(i=0;i<GmfLastKeyword;i++)
      if(msh->KwdTab[i].OrdTab)
         free(msh->KwdTab[i].OrdTab);

   free(msh);

   return(res);
}