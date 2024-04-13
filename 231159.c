ldns_rdf_type_maybe_quoted(ldns_rdf_type rdf_type)
{
	return  rdf_type == LDNS_RDF_TYPE_STR ||
		rdf_type == LDNS_RDF_TYPE_LONG_STR;
}