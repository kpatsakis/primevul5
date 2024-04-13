void crypto_cert_subject_alt_name_free(int count, int* lengths, char** alt_names)
{
	crypto_cert_dns_names_free(count, lengths, alt_names);
}