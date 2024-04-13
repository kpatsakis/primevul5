int SELECT_LEX::vers_setup_conds(THD *thd, TABLE_LIST *tables)
{
  DBUG_ENTER("SELECT_LEX::vers_setup_cond");
#define newx new (thd->mem_root)

  const bool update_conds= !skip_setup_conds(thd);
  TABLE_LIST *table;

  if (!versioned_tables)
  {
    for (table= tables; table; table= table->next_local)
    {
      if (table->table && table->table->versioned())
        versioned_tables++;
      else if (table->vers_conditions.is_set() &&
              (table->is_non_derived() || !table->vers_conditions.used))
      {
        my_error(ER_VERS_NOT_VERSIONED, MYF(0), table->alias.str);
        DBUG_RETURN(-1);
      }
    }
  }

  if (versioned_tables == 0)
    DBUG_RETURN(0);

  /* For prepared statements we create items on statement arena,
     because they must outlive execution phase for multiple executions. */
  Query_arena_stmt on_stmt_arena(thd);

  // find outer system_time
  SELECT_LEX *outer_slex= outer_select();
  TABLE_LIST* outer_table= NULL;

  if (outer_slex)
  {
    TABLE_LIST* derived= master_unit()->derived;
    // inner SELECT may not be a derived table (derived == NULL)
    while (derived && outer_slex && !derived->vers_conditions.is_set())
    {
      derived= outer_slex->master_unit()->derived;
      outer_slex= outer_slex->outer_select();
    }
    if (derived && outer_slex)
    {
      DBUG_ASSERT(derived->vers_conditions.is_set());
      outer_table= derived;
    }
  }

  bool is_select= false;
  bool use_sysvar= false;
  switch (thd->lex->sql_command)
  {
  case SQLCOM_SELECT:
    use_sysvar= true;
    /* fall through */
  case SQLCOM_CREATE_TABLE:
  case SQLCOM_INSERT_SELECT:
  case SQLCOM_REPLACE_SELECT:
  case SQLCOM_DELETE_MULTI:
  case SQLCOM_UPDATE_MULTI:
    is_select= true;
  default:
    break;
  }

  for (table= tables; table; table= table->next_local)
  {
    if (!table->table || table->is_view() || !table->table->versioned())
      continue;

    vers_select_conds_t &vers_conditions= table->vers_conditions;

#ifdef WITH_PARTITION_STORAGE_ENGINE
      /*
        if the history is stored in partitions, then partitions
        themselves are not versioned
      */
      if (table->partition_names && table->table->part_info->vers_info)
      {
        /* If the history is stored in partitions, then partitions
            themselves are not versioned. */
        if (vers_conditions.was_set())
        {
          my_error(ER_VERS_QUERY_IN_PARTITION, MYF(0), table->alias.str);
          DBUG_RETURN(-1);
        }
        else if (!vers_conditions.is_set())
          vers_conditions.type= SYSTEM_TIME_ALL;
      }
#endif

    if (outer_table && !vers_conditions.is_set())
    {
      // propagate system_time from nearest outer SELECT_LEX
      vers_conditions= outer_table->vers_conditions;
      outer_table->vers_conditions.used= true;
    }

    // propagate system_time from sysvar
    if (!vers_conditions.is_set() && use_sysvar)
    {
      if (vers_conditions.init_from_sysvar(thd))
        DBUG_RETURN(-1);
    }

    if (vers_conditions.is_set())
    {
      if (vers_conditions.was_set() &&
          table->lock_type > TL_READ_NO_INSERT &&
          !vers_conditions.delete_history)
      {
        my_error(ER_TABLE_NOT_LOCKED_FOR_WRITE, MYF(0), table->alias.str);
        DBUG_RETURN(-1);
      }

      if (vers_conditions.type == SYSTEM_TIME_ALL)
        continue;
    }

    const LEX_CSTRING *fstart=
        thd->make_clex_string(table->table->vers_start_field()->field_name);
    const LEX_CSTRING *fend=
        thd->make_clex_string(table->table->vers_end_field()->field_name);

    Item *row_start=
        newx Item_field(thd, &this->context, table->db.str, table->alias.str, fstart);
    Item *row_end=
        newx Item_field(thd, &this->context, table->db.str, table->alias.str, fend);

    bool timestamps_only= table->table->versioned(VERS_TIMESTAMP);

    if (vers_conditions.is_set() && vers_conditions.type != SYSTEM_TIME_HISTORY)
    {
      thd->where= "FOR SYSTEM_TIME";
      /* TODO: do resolve fix_length_and_dec(), fix_fields(). This requires
        storing vers_conditions as Item and make some magic related to
        vers_system_time_t/VERS_TRX_ID at stage of fix_fields()
        (this is large refactoring). */
      if (vers_conditions.resolve_units(thd))
        DBUG_RETURN(-1);
      if (timestamps_only && (vers_conditions.start.unit == VERS_TRX_ID ||
        vers_conditions.end.unit == VERS_TRX_ID))
      {
        my_error(ER_VERS_ENGINE_UNSUPPORTED, MYF(0), table->table_name.str);
        DBUG_RETURN(-1);
      }
    }

    if (!update_conds)
      continue;

    Item *cond1= NULL, *cond2= NULL, *cond3= NULL, *curr= NULL;
    Item *point_in_time1= vers_conditions.start.item;
    Item *point_in_time2= vers_conditions.end.item;
    TABLE *t= table->table;
    if (t->versioned(VERS_TIMESTAMP))
    {
      MYSQL_TIME max_time;
      switch (vers_conditions.type)
      {
      case SYSTEM_TIME_UNSPECIFIED:
      case SYSTEM_TIME_HISTORY:
        thd->variables.time_zone->gmt_sec_to_TIME(&max_time, TIMESTAMP_MAX_VALUE);
        max_time.second_part= TIME_MAX_SECOND_PART;
        curr= newx Item_datetime_literal(thd, &max_time, TIME_SECOND_PART_DIGITS);
        if (vers_conditions.type == SYSTEM_TIME_UNSPECIFIED)
          cond1= newx Item_func_eq(thd, row_end, curr);
        else
          cond1= newx Item_func_lt(thd, row_end, curr);
        break;
      case SYSTEM_TIME_AS_OF:
        cond1= newx Item_func_le(thd, row_start, point_in_time1);
        cond2= newx Item_func_gt(thd, row_end, point_in_time1);
        break;
      case SYSTEM_TIME_FROM_TO:
        cond1= newx Item_func_lt(thd, row_start, point_in_time2);
        cond2= newx Item_func_gt(thd, row_end, point_in_time1);
        cond3= newx Item_func_lt(thd, point_in_time1, point_in_time2);
        break;
      case SYSTEM_TIME_BETWEEN:
        cond1= newx Item_func_le(thd, row_start, point_in_time2);
        cond2= newx Item_func_gt(thd, row_end, point_in_time1);
        cond3= newx Item_func_le(thd, point_in_time1, point_in_time2);
        break;
      case SYSTEM_TIME_BEFORE:
        cond1= newx Item_func_history(thd, row_end);
        cond2= newx Item_func_lt(thd, row_end, point_in_time1);
        break;
      default:
        DBUG_ASSERT(0);
      }
    }
    else
    {
      DBUG_ASSERT(table->table->s && table->table->s->db_plugin);

      Item *trx_id0, *trx_id1;

      switch (vers_conditions.type)
      {
      case SYSTEM_TIME_UNSPECIFIED:
      case SYSTEM_TIME_HISTORY:
        curr= newx Item_int(thd, ULONGLONG_MAX);
        if (vers_conditions.type == SYSTEM_TIME_UNSPECIFIED)
          cond1= newx Item_func_eq(thd, row_end, curr);
        else
          cond1= newx Item_func_lt(thd, row_end, curr);
        break;
      case SYSTEM_TIME_AS_OF:
        trx_id0= vers_conditions.start.unit == VERS_TIMESTAMP
          ? newx Item_func_trt_id(thd, point_in_time1, TR_table::FLD_TRX_ID)
          : point_in_time1;
        cond1= newx Item_func_trt_trx_sees_eq(thd, trx_id0, row_start);
        cond2= newx Item_func_trt_trx_sees(thd, row_end, trx_id0);
        break;
      case SYSTEM_TIME_FROM_TO:
	cond3= newx Item_func_lt(thd, point_in_time1, point_in_time2);
        /* fall through */
      case SYSTEM_TIME_BETWEEN:
        trx_id0= vers_conditions.start.unit == VERS_TIMESTAMP
          ? newx Item_func_trt_id(thd, point_in_time1, TR_table::FLD_TRX_ID, true)
          : point_in_time1;
        trx_id1= vers_conditions.end.unit == VERS_TIMESTAMP
          ? newx Item_func_trt_id(thd, point_in_time2, TR_table::FLD_TRX_ID, false)
          : point_in_time2;
        cond1= vers_conditions.type == SYSTEM_TIME_FROM_TO
          ? newx Item_func_trt_trx_sees(thd, trx_id1, row_start)
          : newx Item_func_trt_trx_sees_eq(thd, trx_id1, row_start);
        cond2= newx Item_func_trt_trx_sees_eq(thd, row_end, trx_id0);
	if (!cond3)
	  cond3= newx Item_func_le(thd, point_in_time1, point_in_time2);
        break;
      case SYSTEM_TIME_BEFORE:
        trx_id0= vers_conditions.start.unit == VERS_TIMESTAMP
          ? newx Item_func_trt_id(thd, point_in_time1, TR_table::FLD_TRX_ID, true)
          : point_in_time1;
        cond1= newx Item_func_history(thd, row_end);
        cond2= newx Item_func_trt_trx_sees(thd, trx_id0, row_end);
        break;
      default:
        DBUG_ASSERT(0);
      }
    }

    if (cond1)
    {
      cond1= and_items(thd, cond2, cond1);
      cond1= and_items(thd, cond3, cond1);
      if (is_select)
        table->on_expr= and_items(thd, table->on_expr, cond1);
      else
      {
        if (join)
        {
          where= and_items(thd, join->conds, cond1);
          join->conds= where;
        }
        else
          where= and_items(thd, where, cond1);
        table->where= and_items(thd, table->where, cond1);
      }
    }

    table->vers_conditions.type= SYSTEM_TIME_ALL;
  } // for (table= tables; ...)

  DBUG_RETURN(0);
#undef newx
}