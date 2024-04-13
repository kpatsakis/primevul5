    TopicTree(std::function<int(Subscriber *, std::pair<std::string_view, std::string_view>)> cb) {
        this->cb = cb;
    }