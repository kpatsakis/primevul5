CairoFont::CairoFont(Ref ref, cairo_font_face_t *cairo_font_face, FT_Face face,
    Gushort *codeToGID, int codeToGIDLen) : ref(ref), cairo_font_face(cairo_font_face),
					    face(face), codeToGID(codeToGID),
					    codeToGIDLen(codeToGIDLen) { }