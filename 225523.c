make_NegHints(OM_uint32 *minor_status,
	      spnego_gss_cred_id_t spcred, gss_buffer_t *outbuf)
{
	gss_buffer_desc hintNameBuf;
	gss_name_t hintName = GSS_C_NO_NAME;
	gss_name_t hintKerberosName;
	gss_OID hintNameType;
	OM_uint32 major_status;
	OM_uint32 minor;
	unsigned int tlen = 0;
	unsigned int hintNameSize = 0;
	unsigned char *ptr;
	unsigned char *t;

	*outbuf = GSS_C_NO_BUFFER;

	if (spcred != NULL) {
		major_status = gss_inquire_cred(minor_status,
						spcred->mcred,
						&hintName,
						NULL,
						NULL,
						NULL);
		if (major_status != GSS_S_COMPLETE)
			return (major_status);
	}

	if (hintName == GSS_C_NO_NAME) {
		krb5_error_code code;
		krb5int_access kaccess;
		char hostname[HOST_PREFIX_LEN + MAXHOSTNAMELEN + 1] = HOST_PREFIX;

		code = krb5int_accessor(&kaccess, KRB5INT_ACCESS_VERSION);
		if (code != 0) {
			*minor_status = code;
			return (GSS_S_FAILURE);
		}

		/* this breaks mutual authentication but Samba relies on it */
		code = (*kaccess.clean_hostname)(NULL, NULL,
						 &hostname[HOST_PREFIX_LEN],
						 MAXHOSTNAMELEN);
		if (code != 0) {
			*minor_status = code;
			return (GSS_S_FAILURE);
		}

		hintNameBuf.value = hostname;
		hintNameBuf.length = strlen(hostname);

		major_status = gss_import_name(minor_status,
					       &hintNameBuf,
					       GSS_C_NT_HOSTBASED_SERVICE,
					       &hintName);
		if (major_status != GSS_S_COMPLETE) {
			return (major_status);
		}
	}

	hintNameBuf.value = NULL;
	hintNameBuf.length = 0;

	major_status = gss_canonicalize_name(minor_status,
					     hintName,
					     (gss_OID)&gss_mech_krb5_oid,
					     &hintKerberosName);
	if (major_status != GSS_S_COMPLETE) {
		gss_release_name(&minor, &hintName);
		return (major_status);
	}
	gss_release_name(&minor, &hintName);

	major_status = gss_display_name(minor_status,
					hintKerberosName,
					&hintNameBuf,
					&hintNameType);
	if (major_status != GSS_S_COMPLETE) {
		gss_release_name(&minor, &hintName);
		return (major_status);
	}
	gss_release_name(&minor, &hintKerberosName);

	/*
	 * Now encode the name hint into a NegHints ASN.1 type
	 */
	major_status = GSS_S_FAILURE;

	/* Length of DER encoded GeneralString */
	tlen = 1 + gssint_der_length_size(hintNameBuf.length) +
		hintNameBuf.length;
	hintNameSize = tlen;

	/* Length of DER encoded hintName */
	tlen += 1 + gssint_der_length_size(hintNameSize);

	t = gssalloc_malloc(tlen);
	if (t == NULL) {
		*minor_status = ENOMEM;
		goto errout;
	}

	ptr = t;

	*ptr++ = CONTEXT | 0x00; /* hintName identifier */
	if (gssint_put_der_length(hintNameSize,
				  &ptr, tlen - (int)(ptr-t)))
		goto errout;

	*ptr++ = GENERAL_STRING;
	if (gssint_put_der_length(hintNameBuf.length,
				  &ptr, tlen - (int)(ptr-t)))
		goto errout;

	memcpy(ptr, hintNameBuf.value, hintNameBuf.length);
	ptr += hintNameBuf.length;

	*outbuf = (gss_buffer_t)malloc(sizeof(gss_buffer_desc));
	if (*outbuf == NULL) {
		*minor_status = ENOMEM;
		goto errout;
	}
	(*outbuf)->value = (void *)t;
	(*outbuf)->length = ptr - t;

	t = NULL; /* don't free */

	*minor_status = 0;
	major_status = GSS_S_COMPLETE;

errout:
	if (t != NULL) {
		free(t);
	}

	gss_release_buffer(&minor, &hintNameBuf);

	return (major_status);
}