void kssl_krb5_free_data_contents(krb5_context context, krb5_data *data)
	{
#ifdef KRB5_HEIMDAL
	data->length = 0;
        if (data->data)
            free(data->data);
#elif defined(KRB5_MIT_OLD11)
	if (data->data)  {
		krb5_xfree(data->data);
		data->data = 0;
		}
#else
	krb5_free_data_contents(NULL, data);
#endif
	}