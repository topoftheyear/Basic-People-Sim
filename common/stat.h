#ifndef __STAT_H_INCLUDED__
#define __STAT_H_INCLUDED__

class Stat{
    public:
        Stat();
		Stat(int);

        void addXP(int);
		int getLevel();
		int getXP();

    private:
		int level;
		int currentXP;
        int XPToNextLevel;
};

#endif