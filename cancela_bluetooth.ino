#include <SoftwareSerial.h>
#include <Servo.h>
Servo Sg90; //Cria um objeto tipo Servo    
// Serial por software: RX = digital pin 10, TX = digital pin 11
SoftwareSerial bluetooth(10, 11);
//const int key = 7; // Chave de HC-05 (0 - Comando, 1 - AT)
int angulo = 90;

void setup()
{
Sg90.attach(5);
Sg90.write(angulo);  
delay(10000);
bluetooth.begin(9600);
//pinMode(key, OUTPUT);
//digitalWrite(key, LOW);
}

void loop()
{
// Se tem algum dado para ser lido do bluetooth
  if (bluetooth.available() > 0)
  {
    char inByte = bluetooth.read();
    
    if (inByte == 'A' && angulo == 90) 
    { 
      while ( angulo < 175 && angulo >= 90)
        {
          Sg90.write(angulo);
         delay(3);
         angulo = angulo+1;    
        }
    }
    else if (inByte == 'F' && angulo == 175) 
    {

      while ( angulo > 90 && angulo <= 175)
      {
        Sg90.write(angulo);
        delay(3);
        angulo = angulo-1;
      }
    }
  }
}
