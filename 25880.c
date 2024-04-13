static int64_t GetFilPos(GmfMshSct *msh)
{
#ifdef WITH_GMF_AIO
   if(msh->typ & Bin)
      return(lseek(msh->FilDes, 0, 1));
   else
      return(MYFTELL(msh->hdl));
#else
   return(MYFTELL(msh->hdl));
#endif
}