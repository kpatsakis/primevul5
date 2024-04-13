void set_position(JOIN *join,uint idx,JOIN_TAB *table,KEYUSE *key)
{
  join->positions[idx].table= table;
  join->positions[idx].key=key;
  join->positions[idx].records_read=1.0;	/* This is a const table */
  join->positions[idx].cond_selectivity= 1.0;
  join->positions[idx].ref_depend_map= 0;

//  join->positions[idx].loosescan_key= MAX_KEY; /* Not a LooseScan */
  join->positions[idx].sj_strategy= SJ_OPT_NONE;
  join->positions[idx].use_join_buffer= FALSE;

  /* Move the const table as down as possible in best_ref */
  JOIN_TAB **pos=join->best_ref+idx+1;
  JOIN_TAB *next=join->best_ref[idx];
  for (;next != table ; pos++)
  {
    JOIN_TAB *tmp=pos[0];
    pos[0]=next;
    next=tmp;
  }
  join->best_ref[idx]=table;
}