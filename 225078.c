static void vgacon_cursor(struct vc_data *c, int mode)
{
	if (c->vc_mode != KD_TEXT)
		return;

	vgacon_restore_screen(c);

	switch (mode) {
	case CM_ERASE:
		write_vga(14, (c->vc_pos - vga_vram_base) / 2);
	        if (vga_video_type >= VIDEO_TYPE_VGAC)
			vgacon_set_cursor_size(c->state.x, 31, 30);
		else
			vgacon_set_cursor_size(c->state.x, 31, 31);
		break;

	case CM_MOVE:
	case CM_DRAW:
		write_vga(14, (c->vc_pos - vga_vram_base) / 2);
		switch (CUR_SIZE(c->vc_cursor_type)) {
		case CUR_UNDERLINE:
			vgacon_set_cursor_size(c->state.x,
					       c->vc_font.height -
					       (c->vc_font.height <
						10 ? 2 : 3),
					       c->vc_font.height -
					       (c->vc_font.height <
						10 ? 1 : 2));
			break;
		case CUR_TWO_THIRDS:
			vgacon_set_cursor_size(c->state.x,
					       c->vc_font.height / 3,
					       c->vc_font.height -
					       (c->vc_font.height <
						10 ? 1 : 2));
			break;
		case CUR_LOWER_THIRD:
			vgacon_set_cursor_size(c->state.x,
					       (c->vc_font.height * 2) / 3,
					       c->vc_font.height -
					       (c->vc_font.height <
						10 ? 1 : 2));
			break;
		case CUR_LOWER_HALF:
			vgacon_set_cursor_size(c->state.x,
					       c->vc_font.height / 2,
					       c->vc_font.height -
					       (c->vc_font.height <
						10 ? 1 : 2));
			break;
		case CUR_NONE:
			if (vga_video_type >= VIDEO_TYPE_VGAC)
				vgacon_set_cursor_size(c->state.x, 31, 30);
			else
				vgacon_set_cursor_size(c->state.x, 31, 31);
			break;
		default:
			vgacon_set_cursor_size(c->state.x, 1,
					       c->vc_font.height);
			break;
		}
		break;
	}
}