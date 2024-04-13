static SQLITE_NOINLINE void resizeResolveLabel(Parse *p, Vdbe *v, int j){
  int nNewSize = 10 - p->nLabel;
  p->aLabel = sqlite3DbReallocOrFree(p->db, p->aLabel,
                     nNewSize*sizeof(p->aLabel[0]));
  if( p->aLabel==0 ){
    p->nLabelAlloc = 0;
  }else{
#ifdef SQLITE_DEBUG
    int i;
    for(i=p->nLabelAlloc; i<nNewSize; i++) p->aLabel[i] = -1;
#endif
    p->nLabelAlloc = nNewSize;
    p->aLabel[j] = v->nOp;
  }
}