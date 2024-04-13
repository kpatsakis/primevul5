    void subscribe(std::string_view topic, Subscriber *subscriber) {
        /* Start iterating from the root */
        Topic *iterator = root;

        /* Traverse the topic, inserting a node for every new segment separated by / */
        for (size_t start = 0, stop = 0; stop != std::string::npos; start = stop + 1) {
            stop = topic.find('/', start);
            std::string_view segment = topic.substr(start, stop - start);

            auto lb = iterator->children.lower_bound(segment);

            if (lb != iterator->children.end() && !(iterator->children.key_comp()(segment, lb->first))) {
                iterator = lb->second;
            } else {
                /* Allocate and insert new node */
                Topic *newTopic = new Topic;
                newTopic->parent = iterator;
                newTopic->name = new char[segment.length()];
                newTopic->length = segment.length();
                newTopic->terminatingWildcardChild = nullptr;
                newTopic->wildcardChild = nullptr;
                memcpy(newTopic->name, segment.data(), segment.length());

                /* For simplicity we do insert wildcards with text */
                iterator->children.insert(lb, {std::string_view(newTopic->name, segment.length()), newTopic});

                /* Store fast lookup to wildcards */
                if (segment.length() == 1) {
                    /* If this segment is '+' it is a wildcard */
                    if (segment[0] == '+') {
                        iterator->wildcardChild = newTopic;
                    }
                    /* If this segment is '#' it is a terminating wildcard */
                    if (segment[0] == '#') {
                        iterator->terminatingWildcardChild = newTopic;
                    }
                }

                iterator = newTopic;
            }
        }

        /* If this topic is triggered, drain the tree before we join */
        if (iterator->triggered) {
            drain();
        }

        /* Add socket to Topic's Set */
        auto [it, inserted] = iterator->subs.insert(subscriber);

        /* Add Topic to list of subscriptions only if we weren't already subscribed */
        if (inserted) {
            subscriber->subscriptions.push_back(iterator);
        }
    }