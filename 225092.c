static void vgacon_deinit(struct vc_data *c)
{
	/* When closing the active console, reset video origin */
	if (con_is_visible(c)) {
		c->vc_visible_origin = vga_vram_base;
		vga_set_mem_top(c);
	}

	if (!--vgacon_refcount)
		con_free_unimap(c);
	c->vc_uni_pagedir_loc = &c->vc_uni_pagedir;
	con_set_default_unimap(c);
}