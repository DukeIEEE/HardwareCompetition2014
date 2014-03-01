
//*************************************************************************************
//  Color Sensors
//
//*************************************************************************************

// Baud Rate for Color Sensor IO
const int ColorSensor::_color_sensor_baud = 4800;
const char* const ColorSensor::color[] = {"black", "white", "orange", "yelow", "green", "blue", "brown", "red", "other"};

ColorSensor::ColorSensor(int color_sensor_pin, int unused_pin) : pin(color_sensor_pin), serin(color_sensor_pin, unused_pin), serout(unused_pin, color_sensor_pin) {
  current_color_state[0] = current_color_state[1] = current_color_state[2] = 0;
}

void ColorSensor::prepare()
{
  // Reset the ColorPAL and enter direct command mode.
  reset();
  
  // Program ColorPAL to send $ then color data.
  serout.begin(_color_sensor_baud);

  pinMode(pin, OUTPUT);
  serout.print("= (00 $ m) !");	// buffer commmands, loop print $ and data end_loop now execute
  serout.end();

  // Now turn the sio pin into an input to read data from the color pal.
  serin.begin(_color_sensor_baud);
  pinMode(pin, INPUT);
  delay(200);
}


// ResetColorSensor: Sends a long break to reset ColorPAL and enter direct command mode.
void ColorSensor::reset()
{
  delay(200);
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);		// Pull sio low to eliminate any residual charge.
  pinMode(pin, INPUT);			// Return pin to input.
  while (digitalRead(pin) != HIGH);	// Wait for pin to be pulled high by ColorPAL.
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);		// Pull pin low.
  delay(80);					// Keep low for 80ms to enter Direct mode.
  pinMode(pin, INPUT);			// Return pin to input.
  delay(10);					// Pause another 10ms
}

void ColorSensor::update()
{
  int red;
  int grn;
  int blu;
  int sred;
  int sblu;
  int sgrn;
  for(int t = 0; t < 10; t++)
  {
    char buffer[32];
    if (serin.available() > 0)
    {     
      current_color_state[0] = 0;
      current_color_state[1] = 0;
      current_color_state[2] = 0;

      // Wait for a $ and then read three 3 digit hex numbers
      buffer[0] = serin.read();
      if (buffer[0] == '$')
      {
        for(int i = 0; i < 9; i++)
        {
          // Wait for the next input character.
          while (serin.available() == 0);     
          buffer[i] = serin.read();

          // every so often the data terminates early.  If this happens return
          if (buffer[i] == '$')
            return;
        }


        sred = 0000;
        sgrn = 0000;
        sblu = 0000;

        // parse the hex data into integers.
        //for ( int a =0 ; a < 10 ; a++)
        //{
          sscanf (buffer, "%3x%3x%3x", &red, &grn, &blu);
          sred = sred + red;
          sgrn = sgrn + grn;
          sblu = sblu + blu;
        //}

        // Return int array of red, green, blue values
        current_color_state[0] += sred;///10;
        current_color_state[1] += sgrn;///10;
        current_color_state[2] += sblu;///10; 
        delay(10);
        return;
      }
    }
  }
}

void ColorSensor::averageColorState()
{
  int a = 0;
  int b = 0;
  int c = 0;
  int numIterations = 1;
  //average color state information over numIterations
  for(int i = 0; i < numIterations; i++)
  {
     update();
     a += current_color_state[0] / numIterations;
     b += current_color_state[1] / numIterations;
     c += current_color_state[2] / numIterations;
  }
  
  current_color_state[0] = a;
  current_color_state[1] = b;
  current_color_state[2] = c;
}

//Fcn that interperates current color state given empirical measurments of color,distance
//RETURN VALUES:  0 = Black, 1=White, 2=Orange, 3=Yellow, 4=Green, 5=Blue, 6=Brown, 7=Red,8=Unknown
int ColorSensor::interperateColorState()
{
  static int colors[][3] = {{0,0,0},{255,255,255},{134,43,63},{130,59,48},{40,49,65},{44,24,80},{67,27,44},{108,24,55}};
  int bestColor = 0;
  int smallestDiff = 10000;
  for(int i = 0; i < 8; ++i) {
    int rdiff = current_color_state[0] - colors[i][0];
    int gdiff = current_color_state[1] - colors[i][1];
    int bdiff = current_color_state[2] - colors[i][2];
    int diff = abs(rdiff) + abs(gdiff) + abs(bdiff);
    if(diff < smallestDiff) {
      bestColor = i;
      smallestDiff = diff;
    }
  }
 return bestColor;
}

int ColorSensor::getColor() {
  prepare();
  int c[9]; //stores the counts for each of the colors
  for(int i = 0; i < 9; ++i) //intialize to zero
    c[i] = 0;
  for(int i = 0; i < 9; ++i) { //runs color code 10 times, each time, record the resulting color and increment counter
    averageColorState();
    ++c[interperateColorState()];
  }
  serin.end();
  reset();
  //choose the color with greatest number of counts
  int maxColor = 0;
  for(int i = 0; i < 8; ++i)
     if(c[i] > c[maxColor])
       maxColor = i;
  return maxColor;
}


const char* const ColorSensor::getColor(int c) const {
  return color[c]; //returns string represented by integer
}


