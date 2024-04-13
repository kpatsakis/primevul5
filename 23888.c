kssl_map_enc(krb5_enctype enctype)
        {
	switch (enctype)
		{
	case ENCTYPE_DES_HMAC_SHA1:		/*    EVP_des_cbc();       */
	case ENCTYPE_DES_CBC_CRC:
	case ENCTYPE_DES_CBC_MD4:
	case ENCTYPE_DES_CBC_MD5:
	case ENCTYPE_DES_CBC_RAW:
				return EVP_des_cbc();
				break;
	case ENCTYPE_DES3_CBC_SHA1:		/*    EVP_des_ede3_cbc();  */
	case ENCTYPE_DES3_CBC_SHA:
	case ENCTYPE_DES3_CBC_RAW:
				return EVP_des_ede3_cbc();
				break;
	default:                return NULL;
				break;
		}
	}