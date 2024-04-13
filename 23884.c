krb5_timeofday(krb5_context con, krb5_int32 *timeret)
	{
	if (!krb5_loaded)
		load_krb5_dll();

	if ( p_krb5_timeofday )
		return(p_krb5_timeofday(con,timeret));
	else
		return KRB5KRB_ERR_GENERIC;
	}