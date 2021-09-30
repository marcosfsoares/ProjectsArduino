#include <Servo.h>
Servo MeuServo; // Cria um objeto Servo
void setup()
{
MeuServo.attach(5); // Servo no pino 5
MeuServo.write(0);
}
void loop()
{
int angulo = analogRead(A0); // Lê o valor do potenciômetro
angulo = map(angulo, 0, 1023, 0, 180); // Mudança de Escala
MeuServo.write(angulo); // Escreve o ângulo para o servomotor
delay(1000); // Espera de 20ms para que o servomotor atinja a posição
}
