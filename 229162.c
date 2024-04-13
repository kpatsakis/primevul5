ExecRenameStmt(RenameStmt *stmt)
{
	switch (stmt->renameType)
	{
		case OBJECT_TABCONSTRAINT:
		case OBJECT_DOMCONSTRAINT:
			return RenameConstraint(stmt);

		case OBJECT_DATABASE:
			return RenameDatabase(stmt->subname, stmt->newname);

		case OBJECT_ROLE:
			return RenameRole(stmt->subname, stmt->newname);

		case OBJECT_SCHEMA:
			return RenameSchema(stmt->subname, stmt->newname);

		case OBJECT_TABLESPACE:
			return RenameTableSpace(stmt->subname, stmt->newname);

		case OBJECT_TABLE:
		case OBJECT_SEQUENCE:
		case OBJECT_VIEW:
		case OBJECT_MATVIEW:
		case OBJECT_INDEX:
		case OBJECT_FOREIGN_TABLE:
			return RenameRelation(stmt);

		case OBJECT_COLUMN:
		case OBJECT_ATTRIBUTE:
			return renameatt(stmt);

		case OBJECT_RULE:
			return RenameRewriteRule(stmt->relation, stmt->subname,
									 stmt->newname);

		case OBJECT_TRIGGER:
			return renametrig(stmt);

		case OBJECT_POLICY:
			return rename_policy(stmt);

		case OBJECT_DOMAIN:
		case OBJECT_TYPE:
			return RenameType(stmt);

		case OBJECT_AGGREGATE:
		case OBJECT_COLLATION:
		case OBJECT_CONVERSION:
		case OBJECT_EVENT_TRIGGER:
		case OBJECT_FDW:
		case OBJECT_FOREIGN_SERVER:
		case OBJECT_FUNCTION:
		case OBJECT_OPCLASS:
		case OBJECT_OPFAMILY:
		case OBJECT_LANGUAGE:
		case OBJECT_PROCEDURE:
		case OBJECT_ROUTINE:
		case OBJECT_STATISTIC_EXT:
		case OBJECT_TSCONFIGURATION:
		case OBJECT_TSDICTIONARY:
		case OBJECT_TSPARSER:
		case OBJECT_TSTEMPLATE:
		case OBJECT_PUBLICATION:
		case OBJECT_SUBSCRIPTION:
			{
				ObjectAddress address;
				Relation	catalog;
				Relation	relation;

				address = get_object_address(stmt->renameType,
											 stmt->object,
											 &relation,
											 AccessExclusiveLock, false);
				Assert(relation == NULL);

				catalog = table_open(address.classId, RowExclusiveLock);
				AlterObjectRename_internal(catalog,
										   address.objectId,
										   stmt->newname);
				table_close(catalog, RowExclusiveLock);

				return address;
			}

		default:
			elog(ERROR, "unrecognized rename stmt type: %d",
				 (int) stmt->renameType);
			return InvalidObjectAddress;	/* keep compiler happy */
	}
}