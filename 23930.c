krb5_error_code  kssl_validate_times(	krb5_timestamp atime,
					krb5_ticket_times *ttimes)
	{
	krb5_deltat 	skew;
	krb5_timestamp	start, now;
	krb5_error_code	rc;
	krb5_context	context;

	if ((rc = krb5_init_context(&context)))	 return SSL_R_KRB5_S_BAD_TICKET;
	skew = get_rc_clockskew(context); 
	if ((rc = krb5_timeofday(context,&now))) return SSL_R_KRB5_S_BAD_TICKET;
	krb5_free_context(context);

	if (atime  &&  labs(atime - now) >= skew)  return SSL_R_KRB5_S_TKT_SKEW;

	if (! ttimes)  return 0;

	start = (ttimes->starttime != 0)? ttimes->starttime: ttimes->authtime;
	if (start - now > skew)  return SSL_R_KRB5_S_TKT_NYV;
	if ((now - ttimes->endtime) > skew)  return SSL_R_KRB5_S_TKT_EXPIRED;

#ifdef KSSL_DEBUG
	printf("kssl_validate_times: %d |<-  | %d - %d | < %d  ->| %d\n",
		start, atime, now, skew, ttimes->endtime);
#endif	/* KSSL_DEBUG */

	return 0;
	}