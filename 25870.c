static void SwpWrd(char *wrd, int siz)
{
   char  swp;
   int   i;

   for(i=0;i<siz/2;i++)
   {
      swp = wrd[ siz-i-1 ];
      wrd[ siz-i-1 ] = wrd[i];
      wrd[i] = swp;
   }
}