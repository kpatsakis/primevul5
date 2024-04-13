static int64_t GetFilSiz(GmfMshSct *msh)
{
   int64_t CurPos, EndPos = 0;

   if(msh->typ & Bin)
   {
#ifdef WITH_GMF_AIO
      CurPos = lseek(msh->FilDes, 0, 1);
      EndPos = lseek(msh->FilDes, 0, 2);
      lseek(msh->FilDes, (off_t)CurPos, 0);
#else
      CurPos = MYFTELL(msh->hdl);

      if(MYFSEEK(msh->hdl, 0, SEEK_END) != 0)
         longjmp(msh->err, -32);

      EndPos = MYFTELL(msh->hdl);

      if(MYFSEEK(msh->hdl, (off_t)CurPos, SEEK_SET) != 0)
         longjmp(msh->err, -33);
#endif
   }
   else
   {
      CurPos = MYFTELL(msh->hdl);

      if(MYFSEEK(msh->hdl, 0, SEEK_END) != 0)
         longjmp(msh->err, -34);

      EndPos = MYFTELL(msh->hdl);

      if(MYFSEEK(msh->hdl, (off_t)CurPos, SEEK_SET) != 0)
         longjmp(msh->err, -35);
   }

   return(EndPos);
}