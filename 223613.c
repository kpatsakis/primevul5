    size_t RemoteIo::Impl::populateBlocks(size_t lowBlock, size_t highBlock)
    {
        assert(isMalloced_);

        // optimize: ignore all true blocks on left & right sides.
        while(!blocksMap_[lowBlock].isNone()  && lowBlock  < highBlock) lowBlock++;
        while(!blocksMap_[highBlock].isNone() && highBlock > lowBlock)  highBlock--;

        size_t rcount = 0;
        if (blocksMap_[highBlock].isNone())
        {
            std::string data;
            getDataByRange( (long) lowBlock, (long) highBlock, data);
            rcount = data.length();
            if (rcount == 0) {
                throw Error(kerErrorMessage, "Data By Range is empty. Please check the permission.");
            }
            byte* source = (byte*)data.c_str();
            size_t remain = rcount, totalRead = 0;
            size_t iBlock = (rcount == size_) ? 0 : lowBlock;

            while (remain) {
                size_t allow = std::min(remain, blockSize_);
                blocksMap_[iBlock].populate(&source[totalRead], allow);
                remain -= allow;
                totalRead += allow;
                iBlock++;
            }
        }

        return rcount;
    }