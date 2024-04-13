AlterObjectOwner_internal(Relation rel, Oid objectId, Oid new_ownerId)
{
	Oid			classId = RelationGetRelid(rel);
	AttrNumber	Anum_oid = get_object_attnum_oid(classId);
	AttrNumber	Anum_owner = get_object_attnum_owner(classId);
	AttrNumber	Anum_namespace = get_object_attnum_namespace(classId);
	AttrNumber	Anum_acl = get_object_attnum_acl(classId);
	AttrNumber	Anum_name = get_object_attnum_name(classId);
	HeapTuple	oldtup;
	Datum		datum;
	bool		isnull;
	Oid			old_ownerId;
	Oid			namespaceId = InvalidOid;

	oldtup = get_catalog_object_by_oid(rel, Anum_oid, objectId);
	if (oldtup == NULL)
		elog(ERROR, "cache lookup failed for object %u of catalog \"%s\"",
			 objectId, RelationGetRelationName(rel));

	datum = heap_getattr(oldtup, Anum_owner,
						 RelationGetDescr(rel), &isnull);
	Assert(!isnull);
	old_ownerId = DatumGetObjectId(datum);

	if (Anum_namespace != InvalidAttrNumber)
	{
		datum = heap_getattr(oldtup, Anum_namespace,
							 RelationGetDescr(rel), &isnull);
		Assert(!isnull);
		namespaceId = DatumGetObjectId(datum);
	}

	if (old_ownerId != new_ownerId)
	{
		AttrNumber	nattrs;
		HeapTuple	newtup;
		Datum	   *values;
		bool	   *nulls;
		bool	   *replaces;

		/* Superusers can bypass permission checks */
		if (!superuser())
		{
			/* must be owner */
			if (!has_privs_of_role(GetUserId(), old_ownerId))
			{
				char	   *objname;
				char		namebuf[NAMEDATALEN];

				if (Anum_name != InvalidAttrNumber)
				{
					datum = heap_getattr(oldtup, Anum_name,
										 RelationGetDescr(rel), &isnull);
					Assert(!isnull);
					objname = NameStr(*DatumGetName(datum));
				}
				else
				{
					snprintf(namebuf, sizeof(namebuf), "%u", objectId);
					objname = namebuf;
				}
				aclcheck_error(ACLCHECK_NOT_OWNER, get_object_type(classId, objectId),
							   objname);
			}
			/* Must be able to become new owner */
			check_is_member_of_role(GetUserId(), new_ownerId);

			/* New owner must have CREATE privilege on namespace */
			if (OidIsValid(namespaceId))
			{
				AclResult	aclresult;

				aclresult = pg_namespace_aclcheck(namespaceId, new_ownerId,
												  ACL_CREATE);
				if (aclresult != ACLCHECK_OK)
					aclcheck_error(aclresult, OBJECT_SCHEMA,
								   get_namespace_name(namespaceId));
			}
		}

		/* Build a modified tuple */
		nattrs = RelationGetNumberOfAttributes(rel);
		values = palloc0(nattrs * sizeof(Datum));
		nulls = palloc0(nattrs * sizeof(bool));
		replaces = palloc0(nattrs * sizeof(bool));
		values[Anum_owner - 1] = ObjectIdGetDatum(new_ownerId);
		replaces[Anum_owner - 1] = true;

		/*
		 * Determine the modified ACL for the new owner.  This is only
		 * necessary when the ACL is non-null.
		 */
		if (Anum_acl != InvalidAttrNumber)
		{
			datum = heap_getattr(oldtup,
								 Anum_acl, RelationGetDescr(rel), &isnull);
			if (!isnull)
			{
				Acl		   *newAcl;

				newAcl = aclnewowner(DatumGetAclP(datum),
									 old_ownerId, new_ownerId);
				values[Anum_acl - 1] = PointerGetDatum(newAcl);
				replaces[Anum_acl - 1] = true;
			}
		}

		newtup = heap_modify_tuple(oldtup, RelationGetDescr(rel),
								   values, nulls, replaces);

		/* Perform actual update */
		CatalogTupleUpdate(rel, &newtup->t_self, newtup);

		/* Update owner dependency reference */
		if (classId == LargeObjectMetadataRelationId)
			classId = LargeObjectRelationId;
		changeDependencyOnOwner(classId, objectId, new_ownerId);

		/* Release memory */
		pfree(values);
		pfree(nulls);
		pfree(replaces);
	}

	InvokeObjectPostAlterHook(classId, objectId, 0);
}