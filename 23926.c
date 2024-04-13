kssl_krb5_free_data_contents(krb5_context CO, krb5_data  * data)
	{
	if (!krb5_loaded)
		load_krb5_dll();

	if ( p_krb5_free_data_contents )
		p_krb5_free_data_contents(CO,data);
	}