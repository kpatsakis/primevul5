ExecAlterOwnerStmt(AlterOwnerStmt *stmt)
{
	Oid			newowner = get_rolespec_oid(stmt->newowner, false);

	switch (stmt->objectType)
	{
		case OBJECT_DATABASE:
			return AlterDatabaseOwner(strVal((Value *) stmt->object), newowner);

		case OBJECT_SCHEMA:
			return AlterSchemaOwner(strVal((Value *) stmt->object), newowner);

		case OBJECT_TYPE:
		case OBJECT_DOMAIN:		/* same as TYPE */
			return AlterTypeOwner(castNode(List, stmt->object), newowner, stmt->objectType);
			break;

		case OBJECT_FDW:
			return AlterForeignDataWrapperOwner(strVal((Value *) stmt->object),
												newowner);

		case OBJECT_FOREIGN_SERVER:
			return AlterForeignServerOwner(strVal((Value *) stmt->object),
										   newowner);

		case OBJECT_EVENT_TRIGGER:
			return AlterEventTriggerOwner(strVal((Value *) stmt->object),
										  newowner);

		case OBJECT_PUBLICATION:
			return AlterPublicationOwner(strVal((Value *) stmt->object),
										 newowner);

		case OBJECT_SUBSCRIPTION:
			return AlterSubscriptionOwner(strVal((Value *) stmt->object),
										  newowner);

			/* Generic cases */
		case OBJECT_AGGREGATE:
		case OBJECT_COLLATION:
		case OBJECT_CONVERSION:
		case OBJECT_FUNCTION:
		case OBJECT_LANGUAGE:
		case OBJECT_LARGEOBJECT:
		case OBJECT_OPERATOR:
		case OBJECT_OPCLASS:
		case OBJECT_OPFAMILY:
		case OBJECT_PROCEDURE:
		case OBJECT_ROUTINE:
		case OBJECT_STATISTIC_EXT:
		case OBJECT_TABLESPACE:
		case OBJECT_TSDICTIONARY:
		case OBJECT_TSCONFIGURATION:
			{
				Relation	catalog;
				Relation	relation;
				Oid			classId;
				ObjectAddress address;

				address = get_object_address(stmt->objectType,
											 stmt->object,
											 &relation,
											 AccessExclusiveLock,
											 false);
				Assert(relation == NULL);
				classId = address.classId;

				/*
				 * XXX - get_object_address returns Oid of pg_largeobject
				 * catalog for OBJECT_LARGEOBJECT because of historical
				 * reasons.  Fix up it here.
				 */
				if (classId == LargeObjectRelationId)
					classId = LargeObjectMetadataRelationId;

				catalog = table_open(classId, RowExclusiveLock);

				AlterObjectOwner_internal(catalog, address.objectId, newowner);
				table_close(catalog, RowExclusiveLock);

				return address;
			}
			break;

		default:
			elog(ERROR, "unrecognized AlterOwnerStmt type: %d",
				 (int) stmt->objectType);
			return InvalidObjectAddress;	/* keep compiler happy */
	}
}