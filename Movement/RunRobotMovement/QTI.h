#ifndef QTI_H
#define QTI_H

#define QTI_THRESHOLD 650 //threshold for reading in black on qtis

class QTI {
  private:
    int pin;
    long time;
  public:
    QTI(int pin) : pin(pin), time(-1) {}
    void Setup();
    long UpdateState(); //call RCTime and read in current value
    bool isWhite(bool update=true); //check if over white, if update = true, calls UpdateState first
    bool isBlack(bool update=true);
};

#endif
