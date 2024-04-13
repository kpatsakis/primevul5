freePGconn(PGconn *conn)
{
	int			i;

	/* let any event procs clean up their state data */
	for (i = 0; i < conn->nEvents; i++)
	{
		PGEventConnDestroy evt;

		evt.conn = conn;
		(void) conn->events[i].proc(PGEVT_CONNDESTROY, &evt,
									conn->events[i].passThrough);
		free(conn->events[i].name);
	}

	/* clean up pg_conn_host structures */
	if (conn->connhost != NULL)
	{
		for (i = 0; i < conn->nconnhost; ++i)
		{
			if (conn->connhost[i].host != NULL)
				free(conn->connhost[i].host);
			if (conn->connhost[i].hostaddr != NULL)
				free(conn->connhost[i].hostaddr);
			if (conn->connhost[i].port != NULL)
				free(conn->connhost[i].port);
			if (conn->connhost[i].password != NULL)
			{
				explicit_bzero(conn->connhost[i].password, strlen(conn->connhost[i].password));
				free(conn->connhost[i].password);
			}
		}
		free(conn->connhost);
	}

	if (conn->client_encoding_initial)
		free(conn->client_encoding_initial);
	if (conn->events)
		free(conn->events);
	if (conn->pghost)
		free(conn->pghost);
	if (conn->pghostaddr)
		free(conn->pghostaddr);
	if (conn->pgport)
		free(conn->pgport);
	if (conn->connect_timeout)
		free(conn->connect_timeout);
	if (conn->pgtcp_user_timeout)
		free(conn->pgtcp_user_timeout);
	if (conn->pgoptions)
		free(conn->pgoptions);
	if (conn->appname)
		free(conn->appname);
	if (conn->fbappname)
		free(conn->fbappname);
	if (conn->dbName)
		free(conn->dbName);
	if (conn->replication)
		free(conn->replication);
	if (conn->pguser)
		free(conn->pguser);
	if (conn->pgpass)
	{
		explicit_bzero(conn->pgpass, strlen(conn->pgpass));
		free(conn->pgpass);
	}
	if (conn->pgpassfile)
		free(conn->pgpassfile);
	if (conn->channel_binding)
		free(conn->channel_binding);
	if (conn->keepalives)
		free(conn->keepalives);
	if (conn->keepalives_idle)
		free(conn->keepalives_idle);
	if (conn->keepalives_interval)
		free(conn->keepalives_interval);
	if (conn->keepalives_count)
		free(conn->keepalives_count);
	if (conn->sslmode)
		free(conn->sslmode);
	if (conn->sslcert)
		free(conn->sslcert);
	if (conn->sslkey)
		free(conn->sslkey);
	if (conn->sslpassword)
	{
		explicit_bzero(conn->sslpassword, strlen(conn->sslpassword));
		free(conn->sslpassword);
	}
	if (conn->sslrootcert)
		free(conn->sslrootcert);
	if (conn->sslcrl)
		free(conn->sslcrl);
	if (conn->sslcrldir)
		free(conn->sslcrldir);
	if (conn->sslcompression)
		free(conn->sslcompression);
	if (conn->sslsni)
		free(conn->sslsni);
	if (conn->requirepeer)
		free(conn->requirepeer);
	if (conn->ssl_min_protocol_version)
		free(conn->ssl_min_protocol_version);
	if (conn->ssl_max_protocol_version)
		free(conn->ssl_max_protocol_version);
	if (conn->gssencmode)
		free(conn->gssencmode);
	if (conn->krbsrvname)
		free(conn->krbsrvname);
	if (conn->gsslib)
		free(conn->gsslib);
	if (conn->connip)
		free(conn->connip);
	/* Note that conn->Pfdebug is not ours to close or free */
	if (conn->write_err_msg)
		free(conn->write_err_msg);
	if (conn->inBuffer)
		free(conn->inBuffer);
	if (conn->outBuffer)
		free(conn->outBuffer);
	if (conn->rowBuf)
		free(conn->rowBuf);
	if (conn->target_session_attrs)
		free(conn->target_session_attrs);
	termPQExpBuffer(&conn->errorMessage);
	termPQExpBuffer(&conn->workBuffer);

	free(conn);
}