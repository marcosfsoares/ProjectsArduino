
#include <Servo.h>
Servo MeuServo; // Cria um objeto Servo
void setup()
{
  Serial.begin(9600);
  MeuServo.attach(8); // Servo no pino 8
}
void loop()
{ int angulo;
  if (Serial.available() > 0)
{
  angulo = Serial.parseInt(); // Lê o valor inserido
  
  //angulo = map(angulo, 0, 1023, 0, 180); // Mudança de Escala
  MeuServo.write(angulo); // Escreve o ângulo para o servomotor
  //delay(20); // Espera de 20ms para que o servomotor atinja a posição
  Serial.println(angulo);
}
}
