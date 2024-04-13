int __cil_reset_node(struct cil_tree_node *node,  __attribute__((unused)) uint32_t *finished, __attribute__((unused)) void *extra_args)
{
	switch (node->flavor) {
	case CIL_CLASS:
		cil_reset_class(node->data);
		break;
	case CIL_PERM:
	case CIL_MAP_PERM:
		cil_reset_perm(node->data);
		break;
	case CIL_CLASSPERMISSION:
		cil_reset_classpermission(node->data);
		break;
	case CIL_CLASSPERMISSIONSET:
		cil_reset_classpermissionset(node->data);
		break;
	case CIL_CLASSMAPPING:
		cil_reset_classmapping(node->data);
		break;
	case CIL_TYPEALIAS:
	case CIL_SENSALIAS:
	case CIL_CATALIAS:
		cil_reset_alias(node->data);
		break;
	case CIL_USERRANGE:
		cil_reset_userrange(node->data);
		break;
	case CIL_USERLEVEL:
		cil_reset_userlevel(node->data);
		break;
	case CIL_USER:
		cil_reset_user(node->data);
		break;
	case CIL_USERATTRIBUTE:
		cil_reset_userattr(node->data);
		break;
	case CIL_USERATTRIBUTESET:
		cil_reset_userattributeset(node->data);
		break;
	case CIL_SELINUXUSERDEFAULT:
	case CIL_SELINUXUSER:
		cil_reset_selinuxuser(node->data);
		break;
	case CIL_ROLE:
		cil_reset_role(node->data);
		break;
	case CIL_ROLEATTRIBUTE:
		cil_reset_roleattr(node->data);
		break;
	case CIL_ROLEATTRIBUTESET:
		cil_reset_roleattributeset(node->data);
		break;
	case CIL_TYPE:
		cil_reset_type(node->data);
		break;
	case CIL_TYPEATTRIBUTE:
		cil_reset_typeattr(node->data);
		break;
	case CIL_TYPEATTRIBUTESET:
		cil_reset_typeattributeset(node->data);
		break;
	case CIL_RANGETRANSITION:
		cil_reset_rangetransition(node->data);
		break;
	case CIL_AVRULE:
		cil_reset_avrule(node->data);
		break;
	case CIL_SENS:
		cil_reset_sens(node->data);
		break;
	case CIL_CAT:
		cil_reset_cat(node->data);
		break;
	case CIL_SENSCAT:
		cil_reset_senscat(node->data);
		break;
	case CIL_CATSET:
		cil_reset_catset(node->data);
		break;
	case CIL_LEVEL:
		cil_reset_level(node->data);
		break;
	case CIL_LEVELRANGE:
		cil_reset_levelrange(node->data);
		break;
	case CIL_CONTEXT:
		cil_reset_context(node->data);
		break;
	case CIL_SIDCONTEXT:
		cil_reset_sidcontext(node->data);
		break;
	case CIL_FILECON:
		cil_reset_filecon(node->data);
		break;
	case CIL_IBPKEYCON:
		cil_reset_ibpkeycon(node->data);
		break;
	case CIL_IBENDPORTCON:
		cil_reset_ibendportcon(node->data);
		break;
	case CIL_PORTCON:
		cil_reset_portcon(node->data);
		break;
	case CIL_NODECON:
		cil_reset_nodecon(node->data);
		break;
	case CIL_GENFSCON:
		cil_reset_genfscon(node->data);
		break;
	case CIL_NETIFCON:
		cil_reset_netifcon(node->data);
		break;
	case CIL_PIRQCON:
		cil_reset_pirqcon(node->data);
		break;
	case CIL_IOMEMCON:
		cil_reset_iomemcon(node->data);
		break;
	case CIL_IOPORTCON:
		cil_reset_ioportcon(node->data);
		break;
	case CIL_PCIDEVICECON:
		cil_reset_pcidevicecon(node->data);
		break;
	case CIL_DEVICETREECON:
		cil_reset_devicetreecon(node->data);
		break;
	case CIL_FSUSE:
		cil_reset_fsuse(node->data);
		break;
	case CIL_SID:
		cil_reset_sid(node->data);
		break;
	case CIL_CONSTRAIN:
	case CIL_MLSCONSTRAIN:
		cil_reset_constrain(node->data);
		break;
	case CIL_VALIDATETRANS:
	case CIL_MLSVALIDATETRANS:
		cil_reset_validatetrans(node->data);
		break;
	case CIL_DEFAULTUSER:
	case CIL_DEFAULTROLE:
	case CIL_DEFAULTTYPE:
		cil_reset_default(node->data);
		break;
	case CIL_DEFAULTRANGE:
		cil_reset_defaultrange(node->data);
		break;
	case CIL_BOOLEANIF:
		cil_reset_booleanif(node->data);
		break;
	case CIL_TUNABLEIF:
	case CIL_CALL:
		break; /* Not effected by optional block disabling */
	case CIL_MACRO:
	case CIL_SIDORDER:
	case CIL_CLASSORDER:
	case CIL_CATORDER:
	case CIL_SENSITIVITYORDER:
	case CIL_EXPANDTYPEATTRIBUTE:
		break; /* Nothing to reset */
	default:
		break;
	}

	return SEPOL_OK;
}