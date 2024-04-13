PQpingParams(const char *const *keywords,
			 const char *const *values,
			 int expand_dbname)
{
	PGconn	   *conn = PQconnectStartParams(keywords, values, expand_dbname);
	PGPing		ret;

	ret = internal_ping(conn);
	PQfinish(conn);

	return ret;
}