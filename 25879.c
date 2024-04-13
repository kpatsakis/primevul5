static void SetPos(GmfMshSct *msh, int64_t pos)
{
   int IntVal;

   if(msh->ver >= 3)
      RecDblWrd(msh, (unsigned char*)&pos);
   else
   {
      IntVal = (int)pos;
      RecWrd(msh, (unsigned char*)&IntVal);
   }
}