xsltFreeKeyDef(xsltKeyDefPtr keyd) {
    if (keyd == NULL)
	return;
    if (keyd->comp != NULL)
	xmlXPathFreeCompExpr(keyd->comp);
    if (keyd->usecomp != NULL)
	xmlXPathFreeCompExpr(keyd->usecomp);
    if (keyd->name != NULL)
	xmlFree(keyd->name);
    if (keyd->nameURI != NULL)
	xmlFree(keyd->nameURI);
    if (keyd->match != NULL)
	xmlFree(keyd->match);
    if (keyd->use != NULL)
	xmlFree(keyd->use);
    if (keyd->nsList != NULL)
        xmlFree(keyd->nsList);
    memset(keyd, -1, sizeof(xsltKeyDef));
    xmlFree(keyd);
}