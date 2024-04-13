int RGWSwiftWebsiteHandler::serve_errordoc(const int http_ret,
                                           const std::string error_doc)
{
  /* Try to throw it all away. */
  s->formatter->reset();

  class RGWGetErrorPage : public RGWGetObj_ObjStore_SWIFT {
  public:
    RGWGetErrorPage(RGWRados* const store,
                    RGWHandler_REST* const handler,
                    req_state* const s,
                    const int http_ret) {
      /* Calling a virtual from the base class is safe as the subobject should
       * be properly initialized and we haven't overridden the init method. */
      init(store, s, handler);
      set_get_data(true);
      set_custom_http_response(http_ret);
    }

    int error_handler(const int err_no,
                      std::string* const error_content) override {
      /* Enforce that any error generated while getting the error page will
       * not be send to a client. This allows us to recover from the double
       * fault situation by sending the original message. */
      return 0;
    }
  } get_errpage_op(store, handler, s, http_ret);

  s->object = std::to_string(http_ret) + error_doc;

  RGWOp* newop = &get_errpage_op;
  RGWRequest req(0);
  return rgw_process_authenticated(handler, newop, &req, s, true);
}