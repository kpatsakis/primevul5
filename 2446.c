    bool unsubscribe(std::string_view topic, Subscriber *subscriber) {
        /* Subscribers are likely to have very few subscriptions (20 or fewer) */
        if (subscriber) {
            /* Lookup exact Topic ptr from string */
            Topic *iterator = root;
            for (size_t start = 0, stop = 0; stop != std::string::npos; start = stop + 1) {
                stop = topic.find('/', start);
                std::string_view segment = topic.substr(start, stop - start);

                std::map<std::string_view, Topic *>::iterator it = iterator->children.find(segment);
                if (it == iterator->children.end()) {
                    /* This topic does not even exist */
                    return false;
                }

                iterator = it->second;
            }

            /* Try and remove this topic from our list */
            for (auto it = subscriber->subscriptions.begin(); it != subscriber->subscriptions.end(); it++) {
                if (*it == iterator) {
                    /* If this topic is triggered, drain the tree before we leave */
                    if (iterator->triggered) {
                        drain();
                    }

                    /* Remove topic ptr from our list */
                    subscriber->subscriptions.erase(it);

                    /* Remove us from Topic's subs */
                    iterator->subs.erase(subscriber);
                    trimTree(iterator);
                    return true;
                }
            }
        }
        return false;
    }