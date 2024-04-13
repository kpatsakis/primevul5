void sqlite3VdbeIncrWriteCounter(Vdbe *p, VdbeCursor *pC){
  if( pC==0
   || (pC->eCurType!=CURTYPE_SORTER
       && pC->eCurType!=CURTYPE_PSEUDO
       && !pC->isEphemeral)
  ){
    p->nWrite++;
  }
}