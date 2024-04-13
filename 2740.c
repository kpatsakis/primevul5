PQconnectionUsedPassword(const PGconn *conn)
{
	if (!conn)
		return false;
	if (conn->password_needed)
		return true;
	else
		return false;
}