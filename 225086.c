static inline void vga_set_mem_top(struct vc_data *c)
{
	write_vga(12, (c->vc_visible_origin - vga_vram_base) / 2);
}