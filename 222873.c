static int sqliteProcessJoin(Parse *pParse, Select *p){
  SrcList *pSrc;                  /* All tables in the FROM clause */
  int i, j;                       /* Loop counters */
  struct SrcList_item *pLeft;     /* Left table being joined */
  struct SrcList_item *pRight;    /* Right table being joined */

  pSrc = p->pSrc;
  pLeft = &pSrc->a[0];
  pRight = &pLeft[1];
  for(i=0; i<pSrc->nSrc-1; i++, pRight++, pLeft++){
    Table *pRightTab = pRight->pTab;
    int isOuter;

    if( NEVER(pLeft->pTab==0 || pRightTab==0) ) continue;
    isOuter = (pRight->fg.jointype & JT_OUTER)!=0;

    /* When the NATURAL keyword is present, add WHERE clause terms for
    ** every column that the two tables have in common.
    */
    if( pRight->fg.jointype & JT_NATURAL ){
      if( pRight->pOn || pRight->pUsing ){
        sqlite3ErrorMsg(pParse, "a NATURAL join may not have "
           "an ON or USING clause", 0);
        return 1;
      }
      for(j=0; j<pRightTab->nCol; j++){
        char *zName;   /* Name of column in the right table */
        int iLeft;     /* Matching left table */
        int iLeftCol;  /* Matching column in the left table */

        zName = pRightTab->aCol[j].zName;
        if( tableAndColumnIndex(pSrc, i+1, zName, &iLeft, &iLeftCol) ){
          addWhereTerm(pParse, pSrc, iLeft, iLeftCol, i+1, j,
                       isOuter, &p->pWhere);
        }
      }
    }

    /* Disallow both ON and USING clauses in the same join
    */
    if( pRight->pOn && pRight->pUsing ){
      sqlite3ErrorMsg(pParse, "cannot have both ON and USING "
        "clauses in the same join");
      return 1;
    }

    /* Add the ON clause to the end of the WHERE clause, connected by
    ** an AND operator.
    */
    if( pRight->pOn ){
      if( isOuter ) sqlite3SetJoinExpr(pRight->pOn, pRight->iCursor);
      p->pWhere = sqlite3ExprAnd(pParse, p->pWhere, pRight->pOn);
      pRight->pOn = 0;
    }

    /* Create extra terms on the WHERE clause for each column named
    ** in the USING clause.  Example: If the two tables to be joined are 
    ** A and B and the USING clause names X, Y, and Z, then add this
    ** to the WHERE clause:    A.X=B.X AND A.Y=B.Y AND A.Z=B.Z
    ** Report an error if any column mentioned in the USING clause is
    ** not contained in both tables to be joined.
    */
    if( pRight->pUsing ){
      IdList *pList = pRight->pUsing;
      for(j=0; j<pList->nId; j++){
        char *zName;     /* Name of the term in the USING clause */
        int iLeft;       /* Table on the left with matching column name */
        int iLeftCol;    /* Column number of matching column on the left */
        int iRightCol;   /* Column number of matching column on the right */

        zName = pList->a[j].zName;
        iRightCol = columnIndex(pRightTab, zName);
        if( iRightCol<0
         || !tableAndColumnIndex(pSrc, i+1, zName, &iLeft, &iLeftCol)
        ){
          sqlite3ErrorMsg(pParse, "cannot join using column %s - column "
            "not present in both tables", zName);
          return 1;
        }
        addWhereTerm(pParse, pSrc, iLeft, iLeftCol, i+1, iRightCol,
                     isOuter, &p->pWhere);
      }
    }
  }
  return 0;
}