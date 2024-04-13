    void publish(std::string_view topic, std::pair<std::string_view, std::string_view> message) {
        publish(root, 0, 0, topic, message);
        messageId++;
    }