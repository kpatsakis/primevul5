static void ScaWrd(GmfMshSct *msh, void *ptr)
{
#ifdef WITH_GMF_AIO
   if(read(msh->FilDes, ptr, WrdSiz) != WrdSiz)
#else
   if(fread(ptr, WrdSiz, 1, msh->hdl) != 1)
#endif
      longjmp(msh->err, -26);

   if(msh->cod != 1)
      SwpWrd((char *)ptr, WrdSiz);
}