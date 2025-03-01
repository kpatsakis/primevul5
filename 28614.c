static void vmsvga_fifo_run(struct vmsvga_state_s *s)
{
    uint32_t cmd, colour;
    int args, len, maxloop = 1024;
    int x, y, dx, dy, width, height;
    struct vmsvga_cursor_definition_s cursor;
    uint32_t cmd_start;

    len = vmsvga_fifo_length(s);
    while (len > 0 && --maxloop > 0) {
        /* May need to go back to the start of the command if incomplete */
        cmd_start = s->fifo_stop;

        switch (cmd = vmsvga_fifo_read(s)) {
        case SVGA_CMD_UPDATE:
        case SVGA_CMD_UPDATE_VERBOSE:
            len -= 5;
            if (len < 0) {
                goto rewind;
            }

            x = vmsvga_fifo_read(s);
            y = vmsvga_fifo_read(s);
            width = vmsvga_fifo_read(s);
            height = vmsvga_fifo_read(s);
            vmsvga_update_rect_delayed(s, x, y, width, height);
            break;

        case SVGA_CMD_RECT_FILL:
            len -= 6;
            if (len < 0) {
                goto rewind;
            }

            colour = vmsvga_fifo_read(s);
            x = vmsvga_fifo_read(s);
            y = vmsvga_fifo_read(s);
            width = vmsvga_fifo_read(s);
            height = vmsvga_fifo_read(s);
#ifdef HW_FILL_ACCEL
            if (vmsvga_fill_rect(s, colour, x, y, width, height) == 0) {
                break;
            }
#endif
            args = 0;
            goto badcmd;

        case SVGA_CMD_RECT_COPY:
            len -= 7;
            if (len < 0) {
                goto rewind;
            }

            x = vmsvga_fifo_read(s);
            y = vmsvga_fifo_read(s);
            dx = vmsvga_fifo_read(s);
            dy = vmsvga_fifo_read(s);
            width = vmsvga_fifo_read(s);
            height = vmsvga_fifo_read(s);
#ifdef HW_RECT_ACCEL
            if (vmsvga_copy_rect(s, x, y, dx, dy, width, height) == 0) {
                break;
            }
#endif
            args = 0;
            goto badcmd;

        case SVGA_CMD_DEFINE_CURSOR:
            len -= 8;
            if (len < 0) {
                goto rewind;
            }

            cursor.id = vmsvga_fifo_read(s);
            cursor.hot_x = vmsvga_fifo_read(s);
            cursor.hot_y = vmsvga_fifo_read(s);
            cursor.width = x = vmsvga_fifo_read(s);
            cursor.height = y = vmsvga_fifo_read(s);
            vmsvga_fifo_read(s);
            cursor.bpp = vmsvga_fifo_read(s);

            args = SVGA_BITMAP_SIZE(x, y) + SVGA_PIXMAP_SIZE(x, y, cursor.bpp);
            if (cursor.width > 256 ||
                cursor.height > 256 ||
                cursor.bpp > 32 ||
                SVGA_BITMAP_SIZE(x, y) > sizeof cursor.mask ||
                SVGA_PIXMAP_SIZE(x, y, cursor.bpp) > sizeof cursor.image) {
                    goto badcmd;
            }

            len -= args;
            if (len < 0) {
                goto rewind;
            }

            for (args = 0; args < SVGA_BITMAP_SIZE(x, y); args++) {
                cursor.mask[args] = vmsvga_fifo_read_raw(s);
            }
            for (args = 0; args < SVGA_PIXMAP_SIZE(x, y, cursor.bpp); args++) {
                cursor.image[args] = vmsvga_fifo_read_raw(s);
            }
#ifdef HW_MOUSE_ACCEL
            vmsvga_cursor_define(s, &cursor);
            break;
#else
            args = 0;
            goto badcmd;
#endif

        /*
         * Other commands that we at least know the number of arguments
         * for so we can avoid FIFO desync if driver uses them illegally.
         */
        case SVGA_CMD_DEFINE_ALPHA_CURSOR:
            len -= 6;
            if (len < 0) {
                goto rewind;
            }
            vmsvga_fifo_read(s);
            vmsvga_fifo_read(s);
            vmsvga_fifo_read(s);
            x = vmsvga_fifo_read(s);
            y = vmsvga_fifo_read(s);
            args = x * y;
            goto badcmd;
        case SVGA_CMD_RECT_ROP_FILL:
            args = 6;
            goto badcmd;
        case SVGA_CMD_RECT_ROP_COPY:
            args = 7;
            goto badcmd;
        case SVGA_CMD_DRAW_GLYPH_CLIPPED:
            len -= 4;
            if (len < 0) {
                goto rewind;
            }
            vmsvga_fifo_read(s);
            vmsvga_fifo_read(s);
            args = 7 + (vmsvga_fifo_read(s) >> 2);
            goto badcmd;
        case SVGA_CMD_SURFACE_ALPHA_BLEND:
            args = 12;
            goto badcmd;

        /*
         * Other commands that are not listed as depending on any
         * CAPABILITIES bits, but are not described in the README either.
         */
        case SVGA_CMD_SURFACE_FILL:
        case SVGA_CMD_SURFACE_COPY:
        case SVGA_CMD_FRONT_ROP_FILL:
        case SVGA_CMD_FENCE:
        case SVGA_CMD_INVALID_CMD:
            break; /* Nop */

        default:
            args = 0;
        badcmd:
            len -= args;
            if (len < 0) {
                goto rewind;
            }
            while (args--) {
                vmsvga_fifo_read(s);
            }
            printf("%s: Unknown command 0x%02x in SVGA command FIFO\n",
                   __func__, cmd);
            break;

        rewind:
            s->fifo_stop = cmd_start;
            s->fifo[SVGA_FIFO_STOP] = cpu_to_le32(s->fifo_stop);
            break;
        }
    }

    s->syncing = 0;
}