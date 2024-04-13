ExecAlterObjectSchemaStmt(AlterObjectSchemaStmt *stmt,
						  ObjectAddress *oldSchemaAddr)
{
	ObjectAddress address;
	Oid			oldNspOid;

	switch (stmt->objectType)
	{
		case OBJECT_EXTENSION:
			address = AlterExtensionNamespace(strVal((Value *) stmt->object), stmt->newschema,
											  oldSchemaAddr ? &oldNspOid : NULL);
			break;

		case OBJECT_FOREIGN_TABLE:
		case OBJECT_SEQUENCE:
		case OBJECT_TABLE:
		case OBJECT_VIEW:
		case OBJECT_MATVIEW:
			address = AlterTableNamespace(stmt,
										  oldSchemaAddr ? &oldNspOid : NULL);
			break;

		case OBJECT_DOMAIN:
		case OBJECT_TYPE:
			address = AlterTypeNamespace(castNode(List, stmt->object), stmt->newschema,
										 stmt->objectType,
										 oldSchemaAddr ? &oldNspOid : NULL);
			break;

			/* generic code path */
		case OBJECT_AGGREGATE:
		case OBJECT_COLLATION:
		case OBJECT_CONVERSION:
		case OBJECT_FUNCTION:
		case OBJECT_OPERATOR:
		case OBJECT_OPCLASS:
		case OBJECT_OPFAMILY:
		case OBJECT_PROCEDURE:
		case OBJECT_ROUTINE:
		case OBJECT_STATISTIC_EXT:
		case OBJECT_TSCONFIGURATION:
		case OBJECT_TSDICTIONARY:
		case OBJECT_TSPARSER:
		case OBJECT_TSTEMPLATE:
			{
				Relation	catalog;
				Relation	relation;
				Oid			classId;
				Oid			nspOid;

				address = get_object_address(stmt->objectType,
											 stmt->object,
											 &relation,
											 AccessExclusiveLock,
											 false);
				Assert(relation == NULL);
				classId = address.classId;
				catalog = table_open(classId, RowExclusiveLock);
				nspOid = LookupCreationNamespace(stmt->newschema);

				oldNspOid = AlterObjectNamespace_internal(catalog, address.objectId,
														  nspOid);
				table_close(catalog, RowExclusiveLock);
			}
			break;

		default:
			elog(ERROR, "unrecognized AlterObjectSchemaStmt type: %d",
				 (int) stmt->objectType);
			return InvalidObjectAddress;	/* keep compiler happy */
	}

	if (oldSchemaAddr)
		ObjectAddressSet(*oldSchemaAddr, NamespaceRelationId, oldNspOid);

	return address;
}