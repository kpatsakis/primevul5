void ma_invalidate_stmts(MYSQL *mysql, const char *function_name)
{
  if (mysql->stmts)
  {
    LIST *li_stmt= mysql->stmts;

    for (; li_stmt; li_stmt= li_stmt->next)
    {
      MYSQL_STMT *stmt= (MYSQL_STMT *)li_stmt->data;
      stmt->mysql= NULL;
      SET_CLIENT_STMT_ERROR(stmt, CR_STMT_CLOSED, SQLSTATE_UNKNOWN, function_name);
    }
    mysql->stmts= NULL;
  }
}