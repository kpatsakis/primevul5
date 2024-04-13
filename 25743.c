void Multiupdate_prelocking_strategy::reset(THD *thd)
{
  done= false;
  has_prelocking_list= thd->lex->requires_prelocking();
}