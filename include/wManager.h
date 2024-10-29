#ifdef LED_SCORE_BAR
    #if (LED_TEAM_COLOR==1) 
        const char wmAPNAME[] = "BitcornBoardRED_";
    #else 
        const char wmAPNAME[] = "BitcornBoardBLUE_";
    #endif
#endif
const char wmPASSWD[] = "PlayForSats";

void init_WifiManager();
