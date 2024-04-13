static void ttxt_parse_text_box(GF_XMLNode *n, GF_BoxRecord *box)
{
	u32 i=0;
	GF_XMLAttribute *att;
	memset(box, 0, sizeof(GF_BoxRecord));
	while ( (att=(GF_XMLAttribute *)gf_list_enum(n->attributes, &i))) {
		if (!stricmp(att->name, "top")) box->top = atoi(att->value);
		else if (!stricmp(att->name, "bottom")) box->bottom = atoi(att->value);
		else if (!stricmp(att->name, "left")) box->left = atoi(att->value);
		else if (!stricmp(att->name, "right")) box->right = atoi(att->value);
	}
}