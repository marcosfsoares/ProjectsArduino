//Sensor de luz com LDR
int ledPin = 7; //Led no pino 7
int ldrPin = 5; //LDR no pino analógico 5
int ldrValor = 0; //Valor lido do LDR
void setup(){
pinMode(ledPin,OUTPUT); //define a porta 7 como saída
Serial.begin(9600); //Inicia a comunicação serial
}
void loop(){
//ler o valor do LDR
ldrValor = analogRead(ldrPin); //O valor lido será entre 0 e 1023
//se o valor lido for maior que 800, liga o led
if (ldrValor>= 800) digitalWrite(ledPin,HIGH);
//senão, apaga o led
else digitalWrite(ledPin,LOW);
//imprime o valor lido do LDR no monitor serial
Serial.println(ldrValor);
delay(100);
}
