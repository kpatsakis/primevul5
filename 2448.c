    void drain() {

        /* Do nothing if nothing to send */
        if (!numTriggeredTopics) {
            return;
        }

        /* bug fix: Filter triggered topics without subscribers */
        int numFilteredTriggeredTopics = 0;
        for (int i = 0; i < numTriggeredTopics; i++) {
            if (triggeredTopics[i]->subs.size()) {
                triggeredTopics[numFilteredTriggeredTopics++] = triggeredTopics[i];
            } else {
                /* If we no longer have any subscribers, yet still keep this Topic alive (parent),
                 * make sure to clear its potential messages. */
                triggeredTopics[i]->messages.clear();
                triggeredTopics[i]->triggered = false;
            }
        }
        numTriggeredTopics = numFilteredTriggeredTopics;

        if (!numTriggeredTopics) {
            return;
        }

        /* bug fix: update min, as the one tracked via subscribe gets invalid as you unsubscribe */
        min = (Subscriber *)UINTPTR_MAX;
        for (int i = 0; i < numTriggeredTopics; i++) {
            if ((triggeredTopics[i]->subs.size()) && (min > *triggeredTopics[i]->subs.begin())) {
                min = *triggeredTopics[i]->subs.begin();
            }
        }

        /* Check if we really have any sockets still */
        if (min != (Subscriber *)UINTPTR_MAX) {

            /* Up to 64 triggered Topics per batch */
            std::map<uint64_t, std::pair<std::string, std::string>> intersectionCache;

            /* Loop over these here */
            std::set<Subscriber *>::iterator it[64];
            std::set<Subscriber *>::iterator end[64];
            for (int i = 0; i < numTriggeredTopics; i++) {
                it[i] = triggeredTopics[i]->subs.begin();
                end[i] = triggeredTopics[i]->subs.end();
            }

            /* Empty all sets from unique subscribers */
            for (int nonEmpty = numTriggeredTopics; nonEmpty; ) {

                Subscriber *nextMin = (Subscriber *)UINTPTR_MAX;

                /* The message sets relevant for this intersection */
                std::map<unsigned int, std::pair<std::string, std::string>> *perSubscriberIntersectingTopicMessages[64];
                int numPerSubscriberIntersectingTopicMessages = 0;

                uint64_t intersection = 0;

                for (int i = 0; i < numTriggeredTopics; i++) {
                    if ((it[i] != end[i]) && (*it[i] == min)) {

                        /* Mark this intersection */
                        intersection |= ((uint64_t)1 << i);
                        perSubscriberIntersectingTopicMessages[numPerSubscriberIntersectingTopicMessages++] = &triggeredTopics[i]->messages;

                        it[i]++;
                        if (it[i] == end[i]) {
                            nonEmpty--;
                        }
                        else {
                            if (nextMin > *it[i]) {
                                nextMin = *it[i];
                            }
                        }
                    }
                    else {
                        /* We need to lower nextMin to us, in the case of min being the last in a set */
                        if ((it[i] != end[i]) && (nextMin > *it[i])) {
                            nextMin = *it[i];
                        }
                    }
                }

                /* Generate cache for intersection */
                if (intersectionCache[intersection].first.length() == 0) {

                    /* Build the union in order without duplicates */
                    std::map<unsigned int, std::pair<std::string, std::string>> complete;
                    for (int i = 0; i < numPerSubscriberIntersectingTopicMessages; i++) {
                        complete.insert(perSubscriberIntersectingTopicMessages[i]->begin(), perSubscriberIntersectingTopicMessages[i]->end());
                    }

                    /* Create the linear cache, {inflated, deflated} */
                    std::pair<std::string, std::string> res;
                    for (auto &p : complete) {
                        res.first.append(p.second.first);
                        res.second.append(p.second.second);
                    }

                    cb(min, intersectionCache[intersection] = std::move(res));
                }
                else {
                    cb(min, intersectionCache[intersection]);
                }

                min = nextMin;
            }

        }

        /* Clear messages of triggered Topics */
        for (int i = 0; i < numTriggeredTopics; i++) {
            triggeredTopics[i]->messages.clear();
            triggeredTopics[i]->triggered = false;
        }
        numTriggeredTopics = 0;
    }