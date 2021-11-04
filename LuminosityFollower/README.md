# Luminosity Follower

## Class 3 - Servo Motor 
Use two LDRs (Light Depending Resistor) to create a luminosity follower.
The micro servo motor must aligh with the direction that has more clarity.

=======================================================================================

// Seguidor de Luminosidade

#include<Servo.h>

Servo Sg90; //Cria um objeto tipo Servo

int ldrePin = 1; // Pino de entrada medindo a tensão no ldr do lado esquerdo

int ldrdPin = 5;// Pino de entrada medindo a tensão no ldr do lado direito

int ldreValor = 0; // Valor da tensão do ldr do lado esquerdo

int ldrdValor = 0; // Valor da tensão do ldr do lado direito

int diferenca = 0; // Diferença entre os valores medidos nos ldrs.

int angulo = 90;

void setup() {

  Sg90.attach(5);

  Sg90.write(90);

  delay(10000);

  //Serial.begin(9600); //Porta serial foi desabilitada para dar maior rapidez

}

void loop() {

  ldreValor = analogRead(ldrePin);

  ldrdValor = analogRead(ldrdPin);

  diferenca = ldreValor - ldrdValor;

  if(diferenca < -20) { //Maior luminosidade sobre o ldr da esquerda. Valor de compensação para o servo não ficar indeciso.

    angulo -= 1;

    if(angulo < 0 ) { angulo = 0; }

    Sg90.write(angulo); //Gira para a esquerda

  }

  if(diferenca > 20) { //Maior luminosidade sobre o ldr da direita

    angulo += 1;

    if(angulo > 180) {angulo = 180; }

    Sg90.write(angulo); //Gira para a direita

  }

  //Serial.print("\nValor ldr esquerdo: ");

  //Serial.print(ldreValor);

  //Serial.print("\nValor ldr direito: ");

  //Serial.print(ldrdValor);

  //Serial.print("\nValor do angulo: ");

  //Serial.print(angulo);

  //Serial.print("\nValor da diferenca: ");

  //Serial.print(diferenca);

  delay(15);

}
