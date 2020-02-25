class Emg {
  private:
    int32_t emgValue;
    int32_t emgAvg;
    
  public:
    Emg();
    int32_t emgRead();
    void emgCal();
    int32_t getEmgValue();
    int32_t getEmgAvg();
};
