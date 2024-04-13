static int init_case_fold_table(void)
{
  const CaseFold_11_Type   *p;
  const CaseUnfold_11_Type *p1;
  const CaseUnfold_12_Type *p2;
  const CaseUnfold_13_Type *p3;
  int i;

  THREAD_ATOMIC_START;

  FoldTable = st_init_numtable_with_size(1200);
  if (ONIG_IS_NULL(FoldTable)) return ONIGERR_MEMORY;
  for (i = 0; i < numberof(CaseFold); i++) {
    p = &CaseFold[i];
    st_add_direct(FoldTable, (st_data_t )p->from, (st_data_t )&(p->to));
  }
  for (i = 0; i < numberof(CaseFold_Locale); i++) {
    p = &CaseFold_Locale[i];
    st_add_direct(FoldTable, (st_data_t )p->from, (st_data_t )&(p->to));
  }

  Unfold1Table = st_init_numtable_with_size(1000);
  if (ONIG_IS_NULL(Unfold1Table)) return ONIGERR_MEMORY;

  for (i = 0; i < numberof(CaseUnfold_11); i++) {
    p1 = &CaseUnfold_11[i];
    st_add_direct(Unfold1Table, (st_data_t )p1->from, (st_data_t )&(p1->to));
  }
  for (i = 0; i < numberof(CaseUnfold_11_Locale); i++) {
    p1 = &CaseUnfold_11_Locale[i];
    st_add_direct(Unfold1Table, (st_data_t )p1->from, (st_data_t )&(p1->to));
  }

  Unfold2Table = st_init_table_with_size(&type_code2_hash, 200);
  if (ONIG_IS_NULL(Unfold2Table)) return ONIGERR_MEMORY;

  for (i = 0; i < numberof(CaseUnfold_12); i++) {
    p2 = &CaseUnfold_12[i];
    st_add_direct(Unfold2Table, (st_data_t )p2->from, (st_data_t )(&p2->to));
  }
  for (i = 0; i < numberof(CaseUnfold_12_Locale); i++) {
    p2 = &CaseUnfold_12_Locale[i];
    st_add_direct(Unfold2Table, (st_data_t )p2->from, (st_data_t )(&p2->to));
  }

  Unfold3Table = st_init_table_with_size(&type_code3_hash, 30);
  if (ONIG_IS_NULL(Unfold3Table)) return ONIGERR_MEMORY;

  for (i = 0; i < numberof(CaseUnfold_13); i++) {
    p3 = &CaseUnfold_13[i];
    st_add_direct(Unfold3Table, (st_data_t )p3->from, (st_data_t )(&p3->to));
  }

  CaseFoldInited = 1;
  THREAD_ATOMIC_END;
  return 0;
}