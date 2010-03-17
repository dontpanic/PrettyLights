#define PLA_NUM_LED  4
#define PLA_CONBYTE  42

// Globals
int g_pinIndx = 0;
int g_pins[PLA_NUM_LED] = {3, 5, 6, 9};

void setup() 
{
  // Setup serial
  Serial.begin(9600);
  Serial.flush();
  
  // Handshake
  while (!Serial.available())
  {
    if (Serial.read() == PLA_CONBYTE)
      break; 
    
    delay(500);
  }
  
  Serial.write(PLA_CONBYTE);
  
  // Setup LEDs
  for (int i = 0; i < PLA_NUM_LED; i++)
  {
    pinMode(g_pins[i], OUTPUT);
    analogWrite(g_pins[i], 175.0);
    delay(200);
    analogWrite(g_pins[i], 0);
  }
}

void loop()
{
  // Check for data on serial comm line
  if (Serial.available() >= 16)
  {
    read_data();
  }
  
  delay(20);
}

void read_data()
{
  for (int led = 0; led < PLA_NUM_LED; led++)
  {
     int r, g, b, i;
     
     // Get RGBI
     r = Serial.read();
     g = Serial.read();
     b = Serial.read();
     i = Serial.read();
       
     // Send value
     analogWrite(g_pins[led], i);
  }
}

