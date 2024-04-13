static krb5_deltat get_rc_clockskew(krb5_context context)
	{
	krb5_rcache 	rc;
	krb5_deltat 	clockskew;

	if (krb5_rc_default(context, &rc))  return KSSL_CLOCKSKEW;
	if (krb5_rc_initialize(context, rc, 0))  return KSSL_CLOCKSKEW;
	if (krb5_rc_get_lifespan(context, rc, &clockskew))  {
		clockskew = KSSL_CLOCKSKEW;
		}
	(void) krb5_rc_destroy(context, rc);
	return clockskew;
	}