PQcancel(PGcancel *cancel, char *errbuf, int errbufsize)
{
	if (!cancel)
	{
		strlcpy(errbuf, "PQcancel() -- no cancel object supplied", errbufsize);
		return false;
	}

	return internal_cancel(&cancel->raddr, cancel->be_pid, cancel->be_key,
						   errbuf, errbufsize);
}