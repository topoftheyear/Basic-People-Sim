#ifndef __TIMER_H_INCLUDED__
#define __TIMER_H_INCLUDED__

class Timer{
    public:
        Timer();
        
        void start();
        void stop();
        void pause();
        void unpause();

        unsigned int getTicks();

        bool isStarted();
        bool isPaused();

    private:
        unsigned int mStartTicks;
        unsigned int mPausedTicks;

        bool mPaused;
        bool mStarted;
};

#endif