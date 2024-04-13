static GF_Err ttml_push_resources(GF_TXTIn *ctx, TTMLInterval *interval, GF_XMLNode *node, GF_XMLNode *parent_source_node)
{
	u32 i;
	char szURN[1024];
	u8 *f_data;
	u32 f_size;
	u32 idx;
	GF_Err e;
	GF_XMLAttribute *att, *data_type = NULL;
	GF_XMLNode *child;
	Bool is_source = GF_FALSE;
	Bool is_data = GF_FALSE;
	Bool check_src = GF_FALSE;

	if (!ctx->ttml_embed)
		return GF_OK;

	if (!strcmp(node->name, "source")) {
		is_source = GF_TRUE;
		check_src = GF_TRUE;
	}
	else if (!strcmp(node->name, "data")) {
		is_data = parent_source_node ? GF_TRUE : GF_FALSE;
		check_src = GF_TRUE;
	}
	//we don't embed chunks
	else if (!strcmp(node->name, "chunk")) {
		return GF_OK;
	}
	else if (!strcmp(node->name, "audio") || !strcmp(node->name, "font") || !strcmp(node->name, "image")) {
		check_src = GF_TRUE;
	}

	if (check_src) {
		i = 0;
		while ( (att = (GF_XMLAttribute*)gf_list_enum(node->attributes, &i))) {
			char *url;
			if (!att->value) continue;
			if (is_data && !strcmp(att->name, "type")) {
				data_type = att;
				continue;
			}
			if (strcmp(att->name, "src")) continue;
			if (att->value[0]=='#') continue;

			if (!strncmp(att->value, "file://", 7)) {}
			else if (strstr(att->value, "://"))
				continue;

			url = gf_url_concatenate(ctx->file_name, att->value);
			//embed image
			e = gf_file_load_data(url, &f_data, &f_size);
			gf_free(url);
			if (e) return e;

			e = ttml_push_res(ctx, interval, f_data, f_size);
			if (e) return e;

			idx = gf_list_count(interval ? interval->resources : ctx->ttml_resources);
			gf_free(att->value);
			sprintf(szURN, "urn:mpeg:14496-30:%d", idx);
			att->value = gf_strdup(szURN);
			if (!att->value) return GF_OUT_OF_MEM;

			ctx->has_images = GF_TRUE;
		}
	}

	i = 0;
	while ( (child = (GF_XMLNode*) gf_list_enum(node->content, &i))) {
		if (child->type) {
			if (!is_data) continue;
			u8 *data = child->name;
			u32 ilen = (u32) strlen(data);
			f_size = 3*ilen/4;
			f_data = gf_malloc(sizeof(u8) * f_size);

			f_size = gf_base64_decode(data, ilen, f_data, f_size);

			e = ttml_push_res(ctx, interval, f_data, f_size);
			if (e) return e;

			idx = gf_list_count(interval ? interval->resources : ctx->ttml_resources);
			sprintf(szURN, "urn:mpeg:14496-30:%d", idx);

			GF_SAFEALLOC(att, GF_XMLAttribute)
			if (att) {
				att->name = gf_strdup("src");
				att->value = gf_strdup(szURN);
				gf_list_add(parent_source_node->attributes, att);
			}
			if (!att || !att->value || !att->name) return GF_OUT_OF_MEM;
			if (data_type) {
				gf_list_del_item(node->attributes, data_type);
				gf_list_add(parent_source_node->attributes, data_type);
			}
			gf_xml_dom_node_reset(parent_source_node, GF_FALSE, GF_TRUE);
			ctx->has_images = GF_TRUE;
			return GF_OK;
		}

		e = ttml_push_resources(ctx, interval, child, is_source ? node : NULL);
		if (e) return e;
	}
	return GF_OK;
}