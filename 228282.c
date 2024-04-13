png_error_handler(const char *code, const char *msg) {
    set_add_response(code, "%s", msg);
}