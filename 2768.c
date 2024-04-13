PQconnectionNeedsPassword(const PGconn *conn)
{
	char	   *password;

	if (!conn)
		return false;
	password = PQpass(conn);
	if (conn->password_needed &&
		(password == NULL || password[0] == '\0'))
		return true;
	else
		return false;
}