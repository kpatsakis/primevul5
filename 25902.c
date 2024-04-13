static void ScaDblWrd(GmfMshSct *msh, void *ptr)
{
#ifdef WITH_GMF_AIO
   if(read(msh->FilDes, ptr, WrdSiz * 2) != WrdSiz * 2)
#else
   if( fread(ptr, WrdSiz, 2, msh->hdl) != 2 )
#endif
      longjmp(msh->err, -27);

   if(msh->cod != 1)
      SwpWrd((char *)ptr, 2 * WrdSiz);
}