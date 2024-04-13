void st_select_lex::alloc_index_hints (THD *thd)
{ 
  index_hints= new (thd->mem_root) List<Index_hint>(); 
}