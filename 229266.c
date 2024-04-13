void read_user_name(char *name)
{
  char *str=getenv("USERNAME");		/* ODBC will send user variable */
  ma_strmake(name,str ? str : "ODBC", USERNAME_LENGTH);
}