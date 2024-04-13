wStream* license_send_stream_init(rdpLicense* license)
{
	wStream* s;
	BOOL do_crypt = license->rdp->do_crypt;

	license->rdp->sec_flags = SEC_LICENSE_PKT;

	/**
	 * Encryption of licensing packets is optional even if the rdp security
	 * layer is used. If the peer has not indicated that it is capable of
	 * processing encrypted licensing packets (rdp->do_crypt_license) we turn
	 * off encryption (via rdp->do_crypt) before initializing the rdp stream
	 * and reenable it afterwards.
	 */

	if (do_crypt)
	{
		license->rdp->sec_flags |= SEC_LICENSE_ENCRYPT_CS;
		license->rdp->do_crypt = license->rdp->do_crypt_license;
	}

	s = rdp_send_stream_init(license->rdp);
	if (!s)
		return NULL;

	license->rdp->do_crypt = do_crypt;
	license->PacketHeaderLength = Stream_GetPosition(s);
	if (!Stream_SafeSeek(s, LICENSE_PREAMBLE_LENGTH))
		goto fail;
	return s;

fail:
	Stream_Release(s);
	return NULL;
}