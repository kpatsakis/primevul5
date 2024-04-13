int ha_maria::extra(enum ha_extra_function operation)
{
  int tmp;
  TRN *old_trn= file->trn;
  if ((specialflag & SPECIAL_SAFE_MODE) && operation == HA_EXTRA_KEYREAD)
    return 0;
#ifdef NOT_USED
  if (operation == HA_EXTRA_MMAP && !opt_maria_use_mmap)
    return 0;
#endif
  if (operation == HA_EXTRA_WRITE_CACHE && has_long_unique())
    return 0;

  /*
    We have to set file->trn here because in some cases we call
    extern_lock(F_UNLOCK) (which resets file->trn) followed by maria_close()
    without calling commit/rollback in between.  If file->trn is not set
    we can't remove file->share from the transaction list in the extra() call.

    In current code we don't have to do this for HA_EXTRA_PREPARE_FOR_RENAME
    as this is only used the intermediate table used by ALTER TABLE which
    is not part of the transaction (it's not in the TRN list). Better to
    keep this for now, to not break anything in a stable release.
    When HA_EXTRA_PREPARE_FOR_RENAME is not handled below, we can change
    the warnings in _ma_remove_table_from_trnman() to asserts.

    table->in_use is not set in the case this is a done as part of closefrm()
    as part of drop table.
  */

  if (file->s->now_transactional && table->in_use &&
      (operation == HA_EXTRA_PREPARE_FOR_DROP ||
       operation == HA_EXTRA_PREPARE_FOR_RENAME ||
       operation == HA_EXTRA_PREPARE_FOR_FORCED_CLOSE))
  {
    THD *thd= table->in_use;
    file->trn= THD_TRN;
  }
  DBUG_ASSERT(file->s->base.born_transactional || file->trn == 0 ||
              file->trn == &dummy_transaction_object);

  tmp= maria_extra(file, operation, 0);
  /*
    Restore trn if it was changed above.
    Note that table could be removed from trn->used_tables and
    trn->used_instances if trn was set and some of the above operations
    was used. This is ok as the table should not be part of any transaction
    after this and thus doesn't need to be part of any of the above lists.
  */
  file->trn= old_trn;
  return tmp;
}