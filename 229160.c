AlterObjectNamespace_internal(Relation rel, Oid objid, Oid nspOid)
{
	Oid			classId = RelationGetRelid(rel);
	int			oidCacheId = get_object_catcache_oid(classId);
	int			nameCacheId = get_object_catcache_name(classId);
	AttrNumber	Anum_name = get_object_attnum_name(classId);
	AttrNumber	Anum_namespace = get_object_attnum_namespace(classId);
	AttrNumber	Anum_owner = get_object_attnum_owner(classId);
	Oid			oldNspOid;
	Datum		name,
				namespace;
	bool		isnull;
	HeapTuple	tup,
				newtup;
	Datum	   *values;
	bool	   *nulls;
	bool	   *replaces;

	tup = SearchSysCacheCopy1(oidCacheId, ObjectIdGetDatum(objid));
	if (!HeapTupleIsValid(tup)) /* should not happen */
		elog(ERROR, "cache lookup failed for object %u of catalog \"%s\"",
			 objid, RelationGetRelationName(rel));

	name = heap_getattr(tup, Anum_name, RelationGetDescr(rel), &isnull);
	Assert(!isnull);
	namespace = heap_getattr(tup, Anum_namespace, RelationGetDescr(rel),
							 &isnull);
	Assert(!isnull);
	oldNspOid = DatumGetObjectId(namespace);

	/*
	 * If the object is already in the correct namespace, we don't need to do
	 * anything except fire the object access hook.
	 */
	if (oldNspOid == nspOid)
	{
		InvokeObjectPostAlterHook(classId, objid, 0);
		return oldNspOid;
	}

	/* Check basic namespace related issues */
	CheckSetNamespace(oldNspOid, nspOid);

	/* Permission checks ... superusers can always do it */
	if (!superuser())
	{
		Datum		owner;
		Oid			ownerId;
		AclResult	aclresult;

		/* Fail if object does not have an explicit owner */
		if (Anum_owner <= 0)
			ereport(ERROR,
					(errcode(ERRCODE_INSUFFICIENT_PRIVILEGE),
					 errmsg("must be superuser to set schema of %s",
							getObjectDescriptionOids(classId, objid))));

		/* Otherwise, must be owner of the existing object */
		owner = heap_getattr(tup, Anum_owner, RelationGetDescr(rel), &isnull);
		Assert(!isnull);
		ownerId = DatumGetObjectId(owner);

		if (!has_privs_of_role(GetUserId(), ownerId))
			aclcheck_error(ACLCHECK_NOT_OWNER, get_object_type(classId, objid),
						   NameStr(*(DatumGetName(name))));

		/* User must have CREATE privilege on new namespace */
		aclresult = pg_namespace_aclcheck(nspOid, GetUserId(), ACL_CREATE);
		if (aclresult != ACLCHECK_OK)
			aclcheck_error(aclresult, OBJECT_SCHEMA,
						   get_namespace_name(nspOid));
	}

	/*
	 * Check for duplicate name (more friendly than unique-index failure).
	 * Since this is just a friendliness check, we can just skip it in cases
	 * where there isn't suitable support.
	 */
	if (classId == ProcedureRelationId)
	{
		Form_pg_proc proc = (Form_pg_proc) GETSTRUCT(tup);

		IsThereFunctionInNamespace(NameStr(proc->proname), proc->pronargs,
								   &proc->proargtypes, nspOid);
	}
	else if (classId == CollationRelationId)
	{
		Form_pg_collation coll = (Form_pg_collation) GETSTRUCT(tup);

		IsThereCollationInNamespace(NameStr(coll->collname), nspOid);
	}
	else if (classId == OperatorClassRelationId)
	{
		Form_pg_opclass opc = (Form_pg_opclass) GETSTRUCT(tup);

		IsThereOpClassInNamespace(NameStr(opc->opcname),
								  opc->opcmethod, nspOid);
	}
	else if (classId == OperatorFamilyRelationId)
	{
		Form_pg_opfamily opf = (Form_pg_opfamily) GETSTRUCT(tup);

		IsThereOpFamilyInNamespace(NameStr(opf->opfname),
								   opf->opfmethod, nspOid);
	}
	else if (nameCacheId >= 0 &&
			 SearchSysCacheExists2(nameCacheId, name,
								   ObjectIdGetDatum(nspOid)))
		report_namespace_conflict(classId,
								  NameStr(*(DatumGetName(name))),
								  nspOid);

	/* Build modified tuple */
	values = palloc0(RelationGetNumberOfAttributes(rel) * sizeof(Datum));
	nulls = palloc0(RelationGetNumberOfAttributes(rel) * sizeof(bool));
	replaces = palloc0(RelationGetNumberOfAttributes(rel) * sizeof(bool));
	values[Anum_namespace - 1] = ObjectIdGetDatum(nspOid);
	replaces[Anum_namespace - 1] = true;
	newtup = heap_modify_tuple(tup, RelationGetDescr(rel),
							   values, nulls, replaces);

	/* Perform actual update */
	CatalogTupleUpdate(rel, &tup->t_self, newtup);

	/* Release memory */
	pfree(values);
	pfree(nulls);
	pfree(replaces);

	/* update dependencies to point to the new schema */
	changeDependencyFor(classId, objid,
						NamespaceRelationId, oldNspOid, nspOid);

	InvokeObjectPostAlterHook(classId, objid, 0);

	return oldNspOid;
}