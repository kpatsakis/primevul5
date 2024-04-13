    void unsubscribeAll(Subscriber *subscriber, bool mayFlush = true) {
        if (subscriber) {
            for (Topic *topic : subscriber->subscriptions) {

                /* We do not want to flush when closing a socket, it makes no sense to do so */

                /* If this topic is triggered, drain the tree before we leave */
                if (mayFlush && topic->triggered) {
                    drain();
                }

                /* Remove us from the topic's set */
                topic->subs.erase(subscriber);
                trimTree(topic);
            }
            subscriber->subscriptions.clear();
        }
    }