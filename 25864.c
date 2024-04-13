static void RecWrd(GmfMshSct *msh, const void *wrd)
{
   // [Bruno] added error control
#ifdef WITH_GMF_AIO
   if(write(msh->FilDes, wrd, WrdSiz) != WrdSiz)
#else
   if(fwrite(wrd, WrdSiz, 1, msh->hdl) != 1)
#endif
      longjmp(msh->err,-28);
}