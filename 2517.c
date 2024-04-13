static void ebu_ttd_remove_samples(GF_XMLNode *root, GF_XMLNode **out_body_node)
{
	u32 idx = 0;
	GF_XMLNode *node = NULL;
	*out_body_node = NULL;
	while ( (node = (GF_XMLNode*)gf_list_enum(root->content, &idx))) {
		if (!strcmp(node->name, "body")) {
			GF_XMLNode *body_node;
			u32 body_idx = 0;
			*out_body_node = node;
			while ( (body_node = (GF_XMLNode*)gf_list_enum(node->content, &body_idx))) {
				if (!strcmp(body_node->name, "div")) {
					u32 body_num;
					body_num = gf_list_count(body_node->content);
					while (body_num--) {
						GF_XMLNode *content_node = (GF_XMLNode*)gf_list_get(body_node->content, 0);
						assert(gf_list_find(body_node->content, content_node) == 0);
						gf_list_rem(body_node->content, 0);
						gf_xml_dom_node_del(content_node);
					}
				}
			}
			return;
		}
	}
}