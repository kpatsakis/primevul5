report_namespace_conflict(Oid classId, const char *name, Oid nspOid)
{
	char	   *msgfmt;

	Assert(OidIsValid(nspOid));

	switch (classId)
	{
		case ConversionRelationId:
			Assert(OidIsValid(nspOid));
			msgfmt = gettext_noop("conversion \"%s\" already exists in schema \"%s\"");
			break;
		case StatisticExtRelationId:
			Assert(OidIsValid(nspOid));
			msgfmt = gettext_noop("statistics object \"%s\" already exists in schema \"%s\"");
			break;
		case TSParserRelationId:
			Assert(OidIsValid(nspOid));
			msgfmt = gettext_noop("text search parser \"%s\" already exists in schema \"%s\"");
			break;
		case TSDictionaryRelationId:
			Assert(OidIsValid(nspOid));
			msgfmt = gettext_noop("text search dictionary \"%s\" already exists in schema \"%s\"");
			break;
		case TSTemplateRelationId:
			Assert(OidIsValid(nspOid));
			msgfmt = gettext_noop("text search template \"%s\" already exists in schema \"%s\"");
			break;
		case TSConfigRelationId:
			Assert(OidIsValid(nspOid));
			msgfmt = gettext_noop("text search configuration \"%s\" already exists in schema \"%s\"");
			break;
		default:
			elog(ERROR, "unsupported object class %u", classId);
			break;
	}

	ereport(ERROR,
			(errcode(ERRCODE_DUPLICATE_OBJECT),
			 errmsg(msgfmt, name, get_namespace_name(nspOid))));
}