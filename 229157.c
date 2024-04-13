AlterObjectRename_internal(Relation rel, Oid objectId, const char *new_name)
{
	Oid			classId = RelationGetRelid(rel);
	int			oidCacheId = get_object_catcache_oid(classId);
	int			nameCacheId = get_object_catcache_name(classId);
	AttrNumber	Anum_name = get_object_attnum_name(classId);
	AttrNumber	Anum_namespace = get_object_attnum_namespace(classId);
	AttrNumber	Anum_owner = get_object_attnum_owner(classId);
	HeapTuple	oldtup;
	HeapTuple	newtup;
	Datum		datum;
	bool		isnull;
	Oid			namespaceId;
	Oid			ownerId;
	char	   *old_name;
	AclResult	aclresult;
	Datum	   *values;
	bool	   *nulls;
	bool	   *replaces;
	NameData	nameattrdata;

	oldtup = SearchSysCache1(oidCacheId, ObjectIdGetDatum(objectId));
	if (!HeapTupleIsValid(oldtup))
		elog(ERROR, "cache lookup failed for object %u of catalog \"%s\"",
			 objectId, RelationGetRelationName(rel));

	datum = heap_getattr(oldtup, Anum_name,
						 RelationGetDescr(rel), &isnull);
	Assert(!isnull);
	old_name = NameStr(*(DatumGetName(datum)));

	/* Get OID of namespace */
	if (Anum_namespace > 0)
	{
		datum = heap_getattr(oldtup, Anum_namespace,
							 RelationGetDescr(rel), &isnull);
		Assert(!isnull);
		namespaceId = DatumGetObjectId(datum);
	}
	else
		namespaceId = InvalidOid;

	/* Permission checks ... superusers can always do it */
	if (!superuser())
	{
		/* Fail if object does not have an explicit owner */
		if (Anum_owner <= 0)
			ereport(ERROR,
					(errcode(ERRCODE_INSUFFICIENT_PRIVILEGE),
					 errmsg("must be superuser to rename %s",
							getObjectDescriptionOids(classId, objectId))));

		/* Otherwise, must be owner of the existing object */
		datum = heap_getattr(oldtup, Anum_owner,
							 RelationGetDescr(rel), &isnull);
		Assert(!isnull);
		ownerId = DatumGetObjectId(datum);

		if (!has_privs_of_role(GetUserId(), DatumGetObjectId(ownerId)))
			aclcheck_error(ACLCHECK_NOT_OWNER, get_object_type(classId, objectId),
						   old_name);

		/* User must have CREATE privilege on the namespace */
		if (OidIsValid(namespaceId))
		{
			aclresult = pg_namespace_aclcheck(namespaceId, GetUserId(),
											  ACL_CREATE);
			if (aclresult != ACLCHECK_OK)
				aclcheck_error(aclresult, OBJECT_SCHEMA,
							   get_namespace_name(namespaceId));
		}
	}

	/*
	 * Check for duplicate name (more friendly than unique-index failure).
	 * Since this is just a friendliness check, we can just skip it in cases
	 * where there isn't suitable support.
	 */
	if (classId == ProcedureRelationId)
	{
		Form_pg_proc proc = (Form_pg_proc) GETSTRUCT(oldtup);

		IsThereFunctionInNamespace(new_name, proc->pronargs,
								   &proc->proargtypes, proc->pronamespace);
	}
	else if (classId == CollationRelationId)
	{
		Form_pg_collation coll = (Form_pg_collation) GETSTRUCT(oldtup);

		IsThereCollationInNamespace(new_name, coll->collnamespace);
	}
	else if (classId == OperatorClassRelationId)
	{
		Form_pg_opclass opc = (Form_pg_opclass) GETSTRUCT(oldtup);

		IsThereOpClassInNamespace(new_name, opc->opcmethod,
								  opc->opcnamespace);
	}
	else if (classId == OperatorFamilyRelationId)
	{
		Form_pg_opfamily opf = (Form_pg_opfamily) GETSTRUCT(oldtup);

		IsThereOpFamilyInNamespace(new_name, opf->opfmethod,
								   opf->opfnamespace);
	}
	else if (classId == SubscriptionRelationId)
	{
		if (SearchSysCacheExists2(SUBSCRIPTIONNAME, MyDatabaseId,
								  CStringGetDatum(new_name)))
			report_name_conflict(classId, new_name);

		/* Also enforce regression testing naming rules, if enabled */
#ifdef ENFORCE_REGRESSION_TEST_NAME_RESTRICTIONS
		if (strncmp(new_name, "regress_", 8) != 0)
			elog(WARNING, "subscriptions created by regression test cases should have names starting with \"regress_\"");
#endif
	}
	else if (nameCacheId >= 0)
	{
		if (OidIsValid(namespaceId))
		{
			if (SearchSysCacheExists2(nameCacheId,
									  CStringGetDatum(new_name),
									  ObjectIdGetDatum(namespaceId)))
				report_namespace_conflict(classId, new_name, namespaceId);
		}
		else
		{
			if (SearchSysCacheExists1(nameCacheId,
									  CStringGetDatum(new_name)))
				report_name_conflict(classId, new_name);
		}
	}

	/* Build modified tuple */
	values = palloc0(RelationGetNumberOfAttributes(rel) * sizeof(Datum));
	nulls = palloc0(RelationGetNumberOfAttributes(rel) * sizeof(bool));
	replaces = palloc0(RelationGetNumberOfAttributes(rel) * sizeof(bool));
	namestrcpy(&nameattrdata, new_name);
	values[Anum_name - 1] = NameGetDatum(&nameattrdata);
	replaces[Anum_name - 1] = true;
	newtup = heap_modify_tuple(oldtup, RelationGetDescr(rel),
							   values, nulls, replaces);

	/* Perform actual update */
	CatalogTupleUpdate(rel, &oldtup->t_self, newtup);

	InvokeObjectPostAlterHook(classId, objectId, 0);

	/* Release memory */
	pfree(values);
	pfree(nulls);
	pfree(replaces);
	heap_freetuple(newtup);

	ReleaseSysCache(oldtup);
}