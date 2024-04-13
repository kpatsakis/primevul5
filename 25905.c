static int SetFilPos(GmfMshSct *msh, int64_t pos)
{
#ifdef WITH_GMF_AIO
   if(msh->typ & Bin)
      return((lseek(msh->FilDes, (off_t)pos, 0) != -1));
   else
      return((MYFSEEK(msh->hdl, (off_t)pos, SEEK_SET) == 0));
#else
   return((MYFSEEK(msh->hdl, (off_t)pos, SEEK_SET) == 0));
#endif
}