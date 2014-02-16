#ifndef COLOR_SENSOR_H
#define COLOR_SENSOR_H
class ColorSensor {
  private:
    int pin;
    int current_color_state[3];
    
    //mimics PBASIC serin and serout
    SoftwareSerial serin;
    SoftwareSerial serout;
  private:
    static const char* const color[];
    static const int _color_sensor_baud;
  private:
    void update();
    void averageColorState();
    int interperateColorState();
  public:
    ColorSensor(int,int);
    void prepare(); //initializes sensor
    int getColor(); //gets current color
    void reset();
    const char* const getColor(int) const; //returns name of color
};
#endif
