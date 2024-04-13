static uint64_t sm501_disp_ctrl_read(void *opaque, hwaddr addr,
                                     unsigned size)
{
    SM501State *s = (SM501State *)opaque;
    uint32_t ret = 0;
    SM501_DPRINTF("sm501 disp ctrl regs : read addr=%x\n", (int)addr);

    switch (addr) {

    case SM501_DC_PANEL_CONTROL:
        ret = s->dc_panel_control;
        break;
    case SM501_DC_PANEL_PANNING_CONTROL:
        ret = s->dc_panel_panning_control;
        break;
    case SM501_DC_PANEL_COLOR_KEY:
        /* Not implemented yet */
        break;
    case SM501_DC_PANEL_FB_ADDR:
        ret = s->dc_panel_fb_addr;
        break;
    case SM501_DC_PANEL_FB_OFFSET:
        ret = s->dc_panel_fb_offset;
        break;
    case SM501_DC_PANEL_FB_WIDTH:
        ret = s->dc_panel_fb_width;
        break;
    case SM501_DC_PANEL_FB_HEIGHT:
        ret = s->dc_panel_fb_height;
        break;
    case SM501_DC_PANEL_TL_LOC:
        ret = s->dc_panel_tl_location;
        break;
    case SM501_DC_PANEL_BR_LOC:
        ret = s->dc_panel_br_location;
        break;

    case SM501_DC_PANEL_H_TOT:
        ret = s->dc_panel_h_total;
        break;
    case SM501_DC_PANEL_H_SYNC:
        ret = s->dc_panel_h_sync;
        break;
    case SM501_DC_PANEL_V_TOT:
        ret = s->dc_panel_v_total;
        break;
    case SM501_DC_PANEL_V_SYNC:
        ret = s->dc_panel_v_sync;
        break;

    case SM501_DC_PANEL_HWC_ADDR:
        ret = s->dc_panel_hwc_addr;
        break;
    case SM501_DC_PANEL_HWC_LOC:
        ret = s->dc_panel_hwc_location;
        break;
    case SM501_DC_PANEL_HWC_COLOR_1_2:
        ret = s->dc_panel_hwc_color_1_2;
        break;
    case SM501_DC_PANEL_HWC_COLOR_3:
        ret = s->dc_panel_hwc_color_3;
        break;

    case SM501_DC_VIDEO_CONTROL:
        ret = s->dc_video_control;
        break;

    case SM501_DC_CRT_CONTROL:
        ret = s->dc_crt_control;
        break;
    case SM501_DC_CRT_FB_ADDR:
        ret = s->dc_crt_fb_addr;
        break;
    case SM501_DC_CRT_FB_OFFSET:
        ret = s->dc_crt_fb_offset;
        break;
    case SM501_DC_CRT_H_TOT:
        ret = s->dc_crt_h_total;
        break;
    case SM501_DC_CRT_H_SYNC:
        ret = s->dc_crt_h_sync;
        break;
    case SM501_DC_CRT_V_TOT:
        ret = s->dc_crt_v_total;
        break;
    case SM501_DC_CRT_V_SYNC:
        ret = s->dc_crt_v_sync;
        break;

    case SM501_DC_CRT_HWC_ADDR:
        ret = s->dc_crt_hwc_addr;
        break;
    case SM501_DC_CRT_HWC_LOC:
        ret = s->dc_crt_hwc_location;
        break;
    case SM501_DC_CRT_HWC_COLOR_1_2:
        ret = s->dc_crt_hwc_color_1_2;
        break;
    case SM501_DC_CRT_HWC_COLOR_3:
        ret = s->dc_crt_hwc_color_3;
        break;

    case SM501_DC_PANEL_PALETTE ... SM501_DC_PANEL_PALETTE + 0x400 * 3 - 4:
        ret = sm501_palette_read(opaque, addr - SM501_DC_PANEL_PALETTE);
        break;

    default:
        qemu_log_mask(LOG_UNIMP, "sm501: not implemented disp ctrl register "
                      "read. addr=%" HWADDR_PRIx "\n", addr);
    }

    return ret;
}