
#include "WProgram.h"
void setup();
void loop();
void setup()
{    
  Serial.begin(9600);
}

void loop()
{  
  int iNumBytes = 0; // bytes available
  if ( (iNumBytes = Serial.available()) )
  {
    if (iNumBytes == 0)
      return;
      
    // Create buffer
    char* buf = (char*)malloc(iNumBytes);
    
    // Loop through data
    char c = 0;         // current char
    int iBytesRead = 0; // total bytes read & buff index
    
    while (iBytesRead < iNumBytes && c != '\n')
    {
      char c = Serial.read();
      buf[iBytesRead++] = c;      
    }
    
    char* pingstr = "PING: ";
    char* msgstr = (char*) malloc(strlen(pingstr) + iBytesRead + 1);
    memcpy(msgstr, pingstr, strlen(pingstr));
    memcpy(msgstr + strlen(pingstr), buf, iBytesRead);
    msgstr[strlen(pingstr) + iBytesRead] = '\0';
    
    Serial.write(msgstr);
  }
  
  delay(100);
}



int main(void)
{
	init();

	setup();
    
	for (;;)
		loop();
        
	return 0;
}

