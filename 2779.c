defaultNoticeReceiver(void *arg, const PGresult *res)
{
	(void) arg;					/* not used */
	if (res->noticeHooks.noticeProc != NULL)
		res->noticeHooks.noticeProc(res->noticeHooks.noticeProcArg,
									PQresultErrorMessage(res));
}