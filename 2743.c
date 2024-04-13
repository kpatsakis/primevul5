PQpipelineStatus(const PGconn *conn)
{
	if (!conn)
		return PQ_PIPELINE_OFF;

	return conn->pipelineStatus;
}