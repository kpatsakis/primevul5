gpu_data_for_centered_image(ImageRenderData *ans, unsigned int screen_width_px, unsigned int screen_height_px, unsigned int width, unsigned int height) {
    static const ImageRef source_rect = { .src_rect = { .left=0, .top=0, .bottom=1, .right=1 }};
    const ImageRef *ref = &source_rect;
    float width_frac = 2 * MIN(1, width / (float)screen_width_px), height_frac = 2 * MIN(1, height / (float)screen_height_px);
    float hmargin = (2 - width_frac) / 2;
    float vmargin = (2 - height_frac) / 2;
    const ImageRect r = { .left = -1 + hmargin, .right = -1 + hmargin + width_frac, .top = 1 - vmargin, .bottom = 1 - vmargin - height_frac };
    set_vertex_data(ans, ref, &r);
}