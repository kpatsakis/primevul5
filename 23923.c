load_krb5_dll(void)
	{
	HANDLE hKRB5_32;
    
	krb5_loaded++;
	hKRB5_32 = LoadLibrary(TEXT("KRB5_32"));
	if (!hKRB5_32)
		return;

	(FARPROC) p_krb5_free_data_contents =
		GetProcAddress( hKRB5_32, "krb5_free_data_contents" );
	(FARPROC) p_krb5_free_context =
		GetProcAddress( hKRB5_32, "krb5_free_context" );
	(FARPROC) p_krb5_auth_con_free =
		GetProcAddress( hKRB5_32, "krb5_auth_con_free" );
	(FARPROC) p_krb5_free_principal =
		GetProcAddress( hKRB5_32, "krb5_free_principal" );
	(FARPROC) p_krb5_mk_req_extended =
		GetProcAddress( hKRB5_32, "krb5_mk_req_extended" );
	(FARPROC) p_krb5_get_credentials =
		GetProcAddress( hKRB5_32, "krb5_get_credentials" );
	(FARPROC) p_krb5_cc_get_principal =
		GetProcAddress( hKRB5_32, "krb5_cc_get_principal" );
	(FARPROC) p_krb5_cc_default =
		GetProcAddress( hKRB5_32, "krb5_cc_default" );
	(FARPROC) p_krb5_sname_to_principal =
		GetProcAddress( hKRB5_32, "krb5_sname_to_principal" );
	(FARPROC) p_krb5_init_context =
		GetProcAddress( hKRB5_32, "krb5_init_context" );
	(FARPROC) p_krb5_free_ticket =
		GetProcAddress( hKRB5_32, "krb5_free_ticket" );
	(FARPROC) p_krb5_rd_req =
		GetProcAddress( hKRB5_32, "krb5_rd_req" );
	(FARPROC) p_krb5_principal_compare =
		GetProcAddress( hKRB5_32, "krb5_principal_compare" );
	(FARPROC) p_krb5_decrypt_tkt_part =
		GetProcAddress( hKRB5_32, "krb5_decrypt_tkt_part" );
	(FARPROC) p_krb5_timeofday =
		GetProcAddress( hKRB5_32, "krb5_timeofday" );
	(FARPROC) p_krb5_rc_default =
		GetProcAddress( hKRB5_32, "krb5_rc_default" );
	(FARPROC) p_krb5_rc_initialize =
		GetProcAddress( hKRB5_32, "krb5_rc_initialize" );
	(FARPROC) p_krb5_rc_get_lifespan =
		GetProcAddress( hKRB5_32, "krb5_rc_get_lifespan" );
	(FARPROC) p_krb5_rc_destroy =
		GetProcAddress( hKRB5_32, "krb5_rc_destroy" );
	(FARPROC) p_krb5_kt_default =
		GetProcAddress( hKRB5_32, "krb5_kt_default" );
	(FARPROC) p_krb5_kt_resolve =
		GetProcAddress( hKRB5_32, "krb5_kt_resolve" );
	(FARPROC) p_krb5_auth_con_init =
		GetProcAddress( hKRB5_32, "krb5_auth_con_init" );
        (FARPROC) p_valid_cksumtype =
                GetProcAddress( hKRB5_32, "valid_cksumtype" );
        (FARPROC) p_krb5_checksum_size =
                GetProcAddress( hKRB5_32, "krb5_checksum_size" );
        (FARPROC) p_krb5_kt_free_entry =
                GetProcAddress( hKRB5_32, "krb5_kt_free_entry" );
        (FARPROC) p_krb5_auth_con_setrcache =
                GetProcAddress( hKRB5_32, "krb5_auth_con_setrcache" );
        (FARPROC) p_krb5_get_server_rcache =
                GetProcAddress( hKRB5_32, "krb5_get_server_rcache" );
        (FARPROC) p_krb5_auth_con_getrcache =
                GetProcAddress( hKRB5_32, "krb5_auth_con_getrcache" );
        (FARPROC) p_krb5_kt_close =
                GetProcAddress( hKRB5_32, "krb5_kt_close" );
        (FARPROC) p_krb5_kt_get_entry =
                GetProcAddress( hKRB5_32, "krb5_kt_get_entry" );
	}