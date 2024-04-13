static BOOL license_get_server_rsa_public_key(rdpLicense* license)
{
	BYTE* Exponent;
	BYTE* Modulus;
	int ModulusLength;
	rdpSettings* settings = license->rdp->settings;

	if (license->ServerCertificate->length < 1)
	{
		if (!certificate_read_server_certificate(license->certificate, settings->ServerCertificate,
		                                         settings->ServerCertificateLength))
			return FALSE;
	}

	Exponent = license->certificate->cert_info.exponent;
	Modulus = license->certificate->cert_info.Modulus;
	ModulusLength = license->certificate->cert_info.ModulusLength;
	CopyMemory(license->Exponent, Exponent, 4);
	license->ModulusLength = ModulusLength;
	license->Modulus = (BYTE*)malloc(ModulusLength);
	if (!license->Modulus)
		return FALSE;
	CopyMemory(license->Modulus, Modulus, ModulusLength);
	return TRUE;
}