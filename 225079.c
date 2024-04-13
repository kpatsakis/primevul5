static void vga_pal_blank(struct vgastate *state)
{
	int i;

	vga_w(state->vgabase, VGA_PEL_MSK, 0xff);
	for (i = 0; i < 16; i++) {
		vga_w(state->vgabase, VGA_PEL_IW, i);
		vga_w(state->vgabase, VGA_PEL_D, 0);
		vga_w(state->vgabase, VGA_PEL_D, 0);
		vga_w(state->vgabase, VGA_PEL_D, 0);
	}
}