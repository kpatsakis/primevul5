int64_t APIF77(gmfopenmesh)(  char *FilNam, int *mod,
                              int *ver, int *dim, int StrSiz )
{
   int   i = 0;
   char  TmpNam[ GmfStrSiz ];

   if(StrSiz <= 0)
      return(0);

   // Trim trailing spaces from the fortran string
   while(isspace(FilNam[ StrSiz-1 ]))
      StrSiz--;

   for(i=0;i<StrSiz;i++)
      TmpNam[i] = FilNam[i];

   TmpNam[ StrSiz ] = 0;

   if(*mod == GmfRead)
      return(GmfOpenMesh(TmpNam, *mod, ver, dim));
   else
      return(GmfOpenMesh(TmpNam, *mod, *ver, *dim));
}