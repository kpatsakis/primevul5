static int64_t GetPos(GmfMshSct *msh)
{
   int      IntVal;
   int64_t  pos;

   if(msh->ver >= 3)
      ScaDblWrd(msh, (unsigned char*)&pos);
   else
   {
      ScaWrd(msh, (unsigned char*)&IntVal);
      pos = (int64_t)IntVal;
   }

   return(pos);
}