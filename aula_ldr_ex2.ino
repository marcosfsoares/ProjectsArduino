int bin; float tensao, res, lum; // Declara variáveis
void setup()
{
  Serial.begin(9600); // Inicializa serial
}
void loop()
{
  bin = analogRead(A1); // Lê binário
  tensao = (bin/1023.0)*5.0; // Calc. tensão
  res = tensao*10000.0/(5.0-tensao); // Calc. resist.
  lum = pow(10,6.5-1.25*log10(res)); // Calc. lumnos.
  Serial.print("\n\nDado: ");
  Serial.print(bin); // Exibe binário
  Serial.print("\nTensão [V]: ");
  Serial.print(tensao); // Exibe tensão
  Serial.print("\nResistência [kΩ]: ");
  Serial.print(res/1000); // Exibe resistência
  Serial.print("\nLuminosidade [Lux]: ");
  Serial.print(lum); // Exibe luminosidade
  delay(2000);
}
