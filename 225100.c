static void vga_set_palette(struct vc_data *vc, const unsigned char *table)
{
	int i, j;

	vga_w(vgastate.vgabase, VGA_PEL_MSK, 0xff);
	for (i = j = 0; i < 16; i++) {
		vga_w(vgastate.vgabase, VGA_PEL_IW, table[i]);
		vga_w(vgastate.vgabase, VGA_PEL_D, vc->vc_palette[j++] >> 2);
		vga_w(vgastate.vgabase, VGA_PEL_D, vc->vc_palette[j++] >> 2);
		vga_w(vgastate.vgabase, VGA_PEL_D, vc->vc_palette[j++] >> 2);
	}
}