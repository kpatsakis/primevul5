RGWOp* RGWSwiftWebsiteHandler::get_ws_redirect_op()
{
  class RGWMovedPermanently: public RGWOp {
    const std::string location;
  public:
    explicit RGWMovedPermanently(const std::string& location)
      : location(location) {
    }

    int verify_permission() override {
      return 0;
    }

    void execute() override {
      op_ret = -ERR_PERMANENT_REDIRECT;
      return;
    }

    void send_response() override {
      set_req_state_err(s, op_ret);
      dump_errno(s);
      dump_content_length(s, 0);
      dump_redirect(s, location);
      end_header(s, this);
    }

    const char* name() const override {
      return "RGWMovedPermanently";
    }
  };

  return new RGWMovedPermanently(s->info.request_uri + '/');
}