    int RemoteIo::close()
    {
        if (p_->isMalloced_) {
            p_->eof_ = false;
            p_->idx_ = 0;
        }
#ifdef EXIV2_DEBUG_MESSAGES
        std::cerr << "RemoteIo::close totalRead_ = " << p_->totalRead_ << std::endl;
#endif
        if ( bigBlock_ ) {
            delete [] bigBlock_;
            bigBlock_=nullptr;
        }
        return 0;
    }