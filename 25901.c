static void RecDblWrd(GmfMshSct *msh, const void *wrd)
{
   // [Bruno] added error control
#ifdef WITH_GMF_AIO
   if(write(msh->FilDes, wrd, WrdSiz * 2) != WrdSiz*2)
#else
   if(fwrite(wrd, WrdSiz, 2, msh->hdl) != 2)
#endif
      longjmp(msh->err,-29);
}