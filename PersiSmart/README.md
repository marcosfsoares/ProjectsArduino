### PersiSmart 

Using bluetooth module and servo motor

-You've just got hired by a company thar develops products for home automation.

-Your first project is to create a product that controls the opening of shuttle windows, based on the desired luminosity(PersiSmart).

-This control must be totally executed via cell phone. Use the App Bluetooth Electronics (or another one) to create your user interface.

-The display must present the current luminance and the desired one. The servo motor will case about window oppening and the LDR will measure the current luminance.


================================================================
// Projeto de Persiana Automática com controle de luminosidade via Bluetooth - Prova de conceito

// Sensores e Atuadores

// Aluno: Marcos Flávio Soares

#include <SoftwareSerial.h>

#include <Servo.h>

Servo Sg90; // Cria um objeto tipo Servo

int ldrPin = 5, angulo = 90; // Entrada Analógica do LDR, Ângulo inicial da janela

int ldrBits=0, lumdes=0; // Leitura digital do LDR, luminosidade desejada em inteiro

float ldrV, ldrR, lumin; // Valores calculados de tensão,resistência e luminosidade

char mostrar_lumdes[4] = " "; // Armazenar a luminosidade desejada em vetor de caracteres. Iniciada com três espaços vazios

void setup()

{

// Comunicação serial via bluetooth: RX = pino RX, TX = pino TX do arduino

Serial.begin(9600);

Sg90.attach(9); // Servomotor conectado na porta 9(PWM)

Sg90.write(angulo);

delay(5000);

}

void loop()

{

ldrBits = analogRead(ldrPin); // Valor da leitura do LDR, digitalizado

ldrV = (ldrBits/1023.0) * 5.0; // Valor da tensão no LDR

ldrR = ldrV*10000.0/(5.0 - ldrV); // Valor da resistência do LDR (divisor de tensão)

lumin = pow(10, 6.5-1.25*log10(ldrR)); // Luminosidade em Lux

int ilumin = lumin; // Luminosidade transformada para inteiro

if(ilumin % 5 != 0)

{

ilumin = ilumin - ilumin % 5; // Garante a luminosidade múltipla de 5

}

char textlumin[5];

sprintf(textlumin, "%d", ilumin);

Serial.write("*");

Serial.write("L");

Serial.write(textlumin);

Serial.write("*");

// Ler iluminação solicitada

int contaA = 0; // Conta o indicador de início e fim de sequência recebida. Representado pelo caracter 'A'

int contaBytes = 0; //Conta a quantidade de bytes a serem lidos

while (Serial.available() > 0) //Enquanto houver dados a serem recebidos

{

char inByte = Serial.read();

if (inByte == 'A') //Exclui o caracter indicador de início e fim. Inicia a sequência a ser mostrada da lum. desejada

{

contaA++;

if (contaA%2 != 0) //Identifica o início da sequência. Primeiro A(ímpar).

{

Serial.write("*"); // *D são necessários para início da sequência do que será mostrado no aplicativo

Serial.write("D");

}

else

{

Serial.write("*"); // Identifica o fim da sequência. Segundo A(par)

mostrar_lumdes[contaBytes] = '\0'; // Indica o fim do array, para evitar lixo

contaBytes=0; // Zera a posição do array - Vai valer o último valor quando o slider é solto.

}

}

else

{

//Serial.print("\ninByte: ");

//Serial.print(inByte);

mostrar_lumdes[contaBytes] = inByte;

Serial.write(mostrar_lumdes[contaBytes]); // Mostra no display o valor do slider

contaBytes++;

}

lumdes = atoi(mostrar_lumdes); // Conversão para inteiro

}

int diferenca = lumdes - ilumin;

// Modificar posição da persiana

//Exibe mensagem se tentar aumentar acima do limite ou diminuir abaixo do limite

if (diferenca > 5)

{

angulo += 1;

if(angulo > 180) {angulo = 180; }

Sg90.write(angulo);

}

if (diferenca < -5)

{

angulo -= 1;

if(angulo < 90) {angulo = 90; }

Sg90.write(angulo);

}

delay(100);

}
