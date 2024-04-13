static void vga_vesa_unblank(struct vgastate *state)
{
	/* restore original values of VGA controller registers */
	raw_spin_lock_irq(&vga_lock);
	vga_w(state->vgabase, VGA_MIS_W, vga_state.CrtMiscIO);

	outb_p(0x00, vga_video_port_reg);	/* HorizontalTotal */
	outb_p(vga_state.HorizontalTotal, vga_video_port_val);
	outb_p(0x01, vga_video_port_reg);	/* HorizDisplayEnd */
	outb_p(vga_state.HorizDisplayEnd, vga_video_port_val);
	outb_p(0x04, vga_video_port_reg);	/* StartHorizRetrace */
	outb_p(vga_state.StartHorizRetrace, vga_video_port_val);
	outb_p(0x05, vga_video_port_reg);	/* EndHorizRetrace */
	outb_p(vga_state.EndHorizRetrace, vga_video_port_val);
	outb_p(0x07, vga_video_port_reg);	/* Overflow */
	outb_p(vga_state.Overflow, vga_video_port_val);
	outb_p(0x10, vga_video_port_reg);	/* StartVertRetrace */
	outb_p(vga_state.StartVertRetrace, vga_video_port_val);
	outb_p(0x11, vga_video_port_reg);	/* EndVertRetrace */
	outb_p(vga_state.EndVertRetrace, vga_video_port_val);
	outb_p(0x17, vga_video_port_reg);	/* ModeControl */
	outb_p(vga_state.ModeControl, vga_video_port_val);
	/* ClockingMode */
	vga_wseq(state->vgabase, VGA_SEQ_CLOCK_MODE, vga_state.ClockingMode);

	/* restore index/control registers */
	vga_w(state->vgabase, VGA_SEQ_I, vga_state.SeqCtrlIndex);
	outb_p(vga_state.CrtCtrlIndex, vga_video_port_reg);
	raw_spin_unlock_irq(&vga_lock);
}