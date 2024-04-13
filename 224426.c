void sqlite3VdbePrintOp(FILE *pOut, int pc, VdbeOp *pOp){
  char *zP4;
  char zPtr[50];
  char zCom[100];
  static const char *zFormat1 = "%4d %-13s %4d %4d %4d %-13s %.2X %s\n";
  if( pOut==0 ) pOut = stdout;
  zP4 = displayP4(pOp, zPtr, sizeof(zPtr));
#ifdef SQLITE_ENABLE_EXPLAIN_COMMENTS
  displayComment(pOp, zP4, zCom, sizeof(zCom));
#else
  zCom[0] = 0;
#endif
  /* NB:  The sqlite3OpcodeName() function is implemented by code created
  ** by the mkopcodeh.awk and mkopcodec.awk scripts which extract the
  ** information from the vdbe.c source text */
  fprintf(pOut, zFormat1, pc, 
      sqlite3OpcodeName(pOp->opcode), pOp->p1, pOp->p2, pOp->p3, zP4, pOp->p5,
      zCom
  );
  fflush(pOut);
}