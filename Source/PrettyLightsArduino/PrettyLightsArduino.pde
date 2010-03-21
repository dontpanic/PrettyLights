#define PLA_NUM_LED  4
#define PLA_CONBYTE  42

#define ERROR_PIN    13

// Globals
int g_pinIndx = 0;
int g_pins[PLA_NUM_LED] = {3, 5, 6, 9};

void setup() 
{  
  // Setup serial
  Serial.begin(9600);
  Serial.flush();
  
  // Handshake
  int brecv = 0;
  while (brecv != PLA_CONBYTE)
  {
    while (!Serial.available())
    {    
      delay(200);    
    }
    
    brecv = Serial.read();
  }
  
  Serial.write(PLA_CONBYTE);  
  Serial.println("Arduino connected.");
  
  // Setup LEDs
  for (int i = 0; i < PLA_NUM_LED; i++)
  {
    pinMode(g_pins[i], OUTPUT);
    analogWrite(g_pins[i], 175.0);
    delay(100);
    analogWrite(g_pins[i], 0);
  }
}

void loop()
{
  // Check for data on serial comm line
  if (Serial.available() >= 4)
  {
     unsigned char r, g, b, i;       
     if ((r = Serial.read()) == -1)
       error();
     if ((g = Serial.read()) == -1)
       error();
     if ((b = Serial.read()) == -1)
       error();
     if ((i = Serial.read()) == -1)
       error();
       
     i = 1.16 * pow(2.71828183, 0.147 * ((float)i / 7.29));
     
     //(int)((float)i / 3.0)
     analogWrite(g_pins[g_pinIndx++], i);
     
     if (g_pinIndx == PLA_NUM_LED) 
       g_pinIndx = 0;
  }
}

void error()
{
  Serial.println("Error while reading from serial.");
}
