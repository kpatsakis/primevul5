int GmfSetHONodesOrdering(int64_t MshIdx, int KwdCod, int *BasTab, int *OrdTab)
{
   int i, j, k, flg, NmbNod, NmbCrd;
   GmfMshSct   *msh = (GmfMshSct *)MshIdx;
   KwdSct      *kwd;
   
   // printf("\n\tGmfSetHONodesOrdering 0\n");

   if( (KwdCod < 1) || (KwdCod > GmfMaxKwd) )
      return(0);

   kwd = &msh->KwdTab[ KwdCod ];

   // Find the Bezier indices dimension according to the element's kind
   switch(KwdCod)
   {
      case GmfEdges   :          NmbNod =  2; NmbCrd = 1; break;
      case GmfEdgesP2 :          NmbNod =  3; NmbCrd = 1; break;
      case GmfEdgesP3 :          NmbNod =  4; NmbCrd = 1; break;
      case GmfEdgesP4 :          NmbNod =  5; NmbCrd = 1; break;
      case GmfTriangles   :      NmbNod =  3; NmbCrd = 3; break;
      case GmfTrianglesP2 :      NmbNod =  6; NmbCrd = 3; break;
      case GmfTrianglesP3 :      NmbNod = 10; NmbCrd = 3; break;
      case GmfTrianglesP4 :      NmbNod = 15; NmbCrd = 3; break;
      case GmfQuadrilaterals   : NmbNod =  4; NmbCrd = 2; break;
      case GmfQuadrilateralsQ2 : NmbNod =  9; NmbCrd = 2; break;
      case GmfQuadrilateralsQ3 : NmbNod = 16; NmbCrd = 2; break;
      case GmfQuadrilateralsQ4 : NmbNod = 25; NmbCrd = 2; break;
      case GmfTetrahedra   :     NmbNod =  4; NmbCrd = 4; break;
      case GmfTetrahedraP2 :     NmbNod = 10; NmbCrd = 4; break;
      case GmfTetrahedraP3 :     NmbNod = 20; NmbCrd = 4; break;
      case GmfTetrahedraP4 :     NmbNod = 35; NmbCrd = 4; break;
      case GmfPyramids   :       NmbNod =  5; NmbCrd = 3; break;
      case GmfPyramidsP2 :       NmbNod = 14; NmbCrd = 3; break;
      case GmfPyramidsP3 :       NmbNod = 30; NmbCrd = 3; break;
      case GmfPyramidsP4 :       NmbNod = 55; NmbCrd = 3; break;
      case GmfPrisms   :         NmbNod =  6; NmbCrd = 4; break;
      case GmfPrismsP2 :         NmbNod = 18; NmbCrd = 4; break;
      case GmfPrismsP3 :         NmbNod = 40; NmbCrd = 4; break;
      case GmfPrismsP4 :         NmbNod = 75; NmbCrd = 4; break;
      case GmfHexahedra   :      NmbNod =  8; NmbCrd = 3; break;
      case GmfHexahedraQ2 :      NmbNod = 27; NmbCrd = 3; break;
      case GmfHexahedraQ3 :      NmbNod = 64; NmbCrd = 3; break;
      case GmfHexahedraQ4 :      NmbNod =125; NmbCrd = 3; break;
      default : return(0);
   }

   // Free and rebuild the mapping table if there were already one
   if(kwd->OrdTab)
      free(kwd->OrdTab);

   if(!(kwd->OrdTab = malloc(NmbNod * sizeof(int))))
      return(0);

   // Find the corresponding Bezier coordinates from the source table
   for(i=0;i<NmbNod;i++)
   {
      for(j=0;j<NmbNod;j++)
      {
         flg = 1;

         for(k=0;k<NmbCrd;k++)
            if(BasTab[ i * NmbCrd + k ] != OrdTab[ j * NmbCrd + k ])
            {
               flg = 0;
               break;
            }

         if(flg)
            kwd->OrdTab[j] = i;
      }
   }

   // Check the ordering consistency
   for(i=0;i<NmbNod;i++)
   {
      flg = 0;

      for(j=0;j<NmbNod;j++)
         if(kwd->OrdTab[j] == i)
         {
            flg = 1;
            break;
         }

      if(!flg)
      {
         for(j=0;j<NmbNod;j++)
            kwd->OrdTab[j] = j;

         return(0);
      }
   }

   return(1);
}