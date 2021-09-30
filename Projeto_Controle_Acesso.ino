/* //  Projeto de Controle de Acesso, abrindo porta Automática através de Relé
 * //  Disciplina: Sensores e Atuadores
 * //  Curso de Pós Graduação em Sistemas Embarcados - SENAI-SP 
 * //  Aluno: Marcos Flávio Soares
 * --------------------------------------------------------------------------------------------------------------------
 * Cadastro e descadastro de cartões se dá através de aplicativo via bluetooth
 * --------------------------------------------------------------------------------------------------------------------
 * Hardware Utilizado: 
 * - Módulo MFRC522
 * - Módulo com 2 Relés
 * - Módulo Bluetooth HC-06
 * - leds e resistores
 * - Arduino Uno
 * Tipico layout para o módulo MFRC522:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 */

#include <SPI.h>                        // Inclui a biblioteca do protocolo SPI
#include <MFRC522.h>                    // Biblioteca para dispositivos Mifare RC522
#include <SoftwareSerial.h>

// Cria o objeto MFRC522
#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);       // Instancia da classe

// Cria o objeto bluetooth, de comunicação serial
SoftwareSerial bluetooth(6, 5);         // TX = 5, RX = 6(Bluetooth)

MFRC522::MIFARE_Key key;                // Cria uma variável para a chave de acesso


/****************************************/
// Init array that will store new NUID 
byte nuidPICC[4];

// Dados do Bloco a ser lido/gravado
byte dataBlock[]    = {
        0x53, 0x45, 0x4e, 0x48,       //  'S', 'E', 'N', 'H',
        0x41, 0x20, 0x90, 0x20,       //  'A', ' ', 'É', ' ',
        0x4d, 0x41, 0x52, 0x43,       //  'M', 'A', 'R', 'C',
        0x4f, 0x53, 0x20, 0x46        //  'O', 'S', ' ', 'F'
    };

byte blankBlock[]    = {
        0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00
    };

byte sector         = 2;
byte blockAddr      = 8;
byte trailerBlock   = 11;

// Saídas Digitais
#define LED_PIN 4
#define PORTA_RELE1 2

void setup() 
{ 
  // Configuração das portas e protocolos
  pinMode(LED_PIN,OUTPUT);
  pinMode(PORTA_RELE1, OUTPUT);
  digitalWrite(PORTA_RELE1, HIGH);
  bluetooth.begin(9600); 
  Serial.begin(9600);                     // Inicializa porta serial com o computador 
  SPI.begin();                            // Inicializa o protocolo SPI
  mfrc522.PCD_Init();                     // Inicializa o hardware do MFRC522 
  delay(4); 
  
  // Prepara a chave de acesso ( Será usada somente a chave A). 
  // Será deixada a configuração de fábrica (FF FF FF FF FF FF)h
  for (byte i = 0; i < 6; i++) 
  {
    key.keyByte[i] = 0xFF;
  }

}

void loop()
{
  while (bluetooth.available() > 0)             //Recebe Dados do aplicativovia bluetooth
  {
    char inByte = bluetooth.read();
    if (inByte == 'C')                          // Apertado o botão Cadastrar
    {
      
      while (!cadastrar());                     // Executa enquanto tiver erro no cadastro do cartão
      finalizar();
      return;
    }
    else if (inByte == 'D')                     // Apertado o botão Descadastrar
    {
      while (!descadastrar());                  // Executa enquanto tiver erro no descadastro do cartão
      finalizar();
      return;
    }  
  }
 
 while (ler())                                  // Executa se a leitura do cartão for válida
 {
  digitalWrite(PORTA_RELE1, LOW);               // Abre a porta por 3 segundos
  delay(3000);
  digitalWrite(PORTA_RELE1, HIGH);
 }
 finalizar();

}  


////////////////////// - Finalização da leitura - //////////////////////
void finalizar()
{
  // Parar o cartão PICC - Proximity Integrated Circuit Card
    mfrc522.PICC_HaltA(); 
    // Parar a criptografia do módulo PCD - Proximity Coupling Device
    mfrc522.PCD_StopCrypto1();
  }


///////////////////// - Leitura do cartão - /////////////////////////
/// Será verificada a chave de acesso e também o conteúdo gravado ///
///              que possibilita a abertura                       ///
/////////////////////////////////////////////////////////////////////
bool ler()
{
    // Será utilizado o setor #2 para a leitura
    // Será comparado o valor lido dos 16 bytes de dados do setor 2
    
    MFRC522::StatusCode status;
    byte buffer[18];
    byte size = sizeof(buffer);
 // Espera um cartão para ser lido.
  if ( ! mfrc522.PICC_IsNewCardPresent())
    return false;

 // Verifica se o cartão foi lido e reinicia o loop caso houve erro de leitura
  if ( ! mfrc522.PICC_ReadCardSerial())
  {
    ledErro(LED_PIN);
    delay(600);
    return false;
  }
  
  // Verifica o tipo de cartão PICC. Leitura somente para o tipo MIFARE clássico
  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&  
    piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
    piccType != MFRC522::PICC_TYPE_MIFARE_4K) 
  {
    ledErro(LED_PIN);
    delay(600);
    return false;
  }
  
  // Autenticação com a chave A
  status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) 
  {
      ledErro(LED_PIN);
      delay(600);
      return false;
  }

  // Leitura do bloco

  status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blockAddr, buffer, &size);
  if (status != MFRC522::STATUS_OK) 
  {
    ledErro(LED_PIN);                                 // Led piscando indicando erro de leitura
    delay(600);
    return false;    
  }
  
  // Validação do bloco de dados
  if(compare_byte_array(buffer, 16, dataBlock))      // Se valor lido for igual ao esperado
  { 
    return true; 
  }
  else
  {
      ledErro(LED_PIN);                               // Led piscando indicando cartão inválido
      delay(600);
      return false;
  }  
}

/// Função para validar o cartão, comparando o valor lido com o esperado.///
bool compare_byte_array(byte *buffer, byte bufferSize, byte *dataBlock) 
{
  bool comparador = false;
  for (byte i = 0; i < bufferSize; i++) 
  { 
    if (buffer[i] != dataBlock[i])
    {
      comparador = false;
      return comparador;
    }
    else
    { comparador = true; }

    return comparador;
    }
}

///////////////////  - Piscar o led três vezes - /////////////////////
void ledErro(int pino)
{
  for (int i = 0; i < 3; i++)
  {
    digitalWrite(pino, HIGH);
    delay(300);
    digitalWrite(pino, LOW);
    delay(300);
  }  
}

////////////////////////////  - Cadastrar - ////////////////////////////////////
bool cadastrar()
{

  if ( ! mfrc522.PICC_IsNewCardPresent())
  {
    digitalWrite(LED_PIN, HIGH);
    return false;
  }  

  // Verifica se o cartão foi lido e retorna falso caso houve erro de leitura
  if ( ! mfrc522.PICC_ReadCardSerial())
  {
    ledErro(LED_PIN);                                 // Erro de leitura
    delay(600);
    return false;
  } 
  MFRC522::StatusCode status;
  byte buffer[18];
  byte size = sizeof(buffer);

  // Autenticação com a chave A
  status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) 
  {
    ledErro(LED_PIN);                                 // Erro de autenticação
    delay(600);
    return false;
  }
  status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(blockAddr, dataBlock, 16);
  if (status != MFRC522::STATUS_OK) 
  {
    ledErro(LED_PIN);                                // Falha na gravação
    delay(600);
    return false;
  }
    
  // Ler novamente dados do Bloco
 
  status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blockAddr, buffer, &size);
  if (status != MFRC522::STATUS_OK) 
  {
    ledErro(LED_PIN);                               // Erro de leitura
    delay(600);
    return false;
  }
  // Validar a gravação a partir da quantidade de bytes gravada
  byte count = 0;
  for (byte i = 0; i < 16; i++) 
  {
      // Comparando os valores gravados (buffer) com os valores de validação (dataBlock)
        if (buffer[i] == dataBlock[i])
            count++;
  }
  if (count == 16) 
  {
    digitalWrite(LED_PIN, LOW);                       // Sucesso na gravação
    delay(600);
    return true;
  } 
    else
  {
    ledErro(LED_PIN);                                 // Falha na gravação - Valores lidos são diferentes do que foi  gravado
    delay(600);
    return false;
  } 
}

////////////////////////////  - Descadastrar - ////////////////////////////////////
bool descadastrar()
{
  if ( ! mfrc522.PICC_IsNewCardPresent())
  {
    digitalWrite(LED_PIN, HIGH);                    // Led indicador para passar o cartão para descadastrar
    return false;
  }
  // Verifica se o cartão foi lido e retorna falso caso houve erro de leitura
  if ( ! mfrc522.PICC_ReadCardSerial())
  {
    ledErro(LED_PIN);                               // Erro de leitura
    delay(600);
    return false;
  }
  MFRC522::StatusCode status;
  byte buffer[18];
  byte size = sizeof(buffer);
    
  // Autenticação com a chave A
  status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) 
  {
        ledErro(LED_PIN);                          // Falha na autenticação
        delay(600);
        return false;
  }
  status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(blockAddr, blankBlock, 16);
  if (status != MFRC522::STATUS_OK) 
  {
    ledErro(LED_PIN);                             // Falha na gravação
    delay(600);
    return false;
  }
    
// Ler novamente dados do Bloco para confirmar gravação
  status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blockAddr, buffer, &size);
  if (status != MFRC522::STATUS_OK) 
  {
    ledErro(LED_PIN);                           // Falha na leitura
    delay(600);
    return false;
  }

    // Validar a gravação a partir da quantidade de bytes gravada
  byte count = 0;
  for (byte i = 0; i < 16; i++) 
  {
        // Comparando os valores gravados (buffer) com os valores de validação (dataBlock)
        if (buffer[i] == blankBlock[i])
            count++;
  }
  if (count == 16) 
  {
    digitalWrite(LED_PIN, LOW);                 // Sucesso no descadastro do cartão
    delay(600);
    return true;
  } 
  else
  {
    ledErro(LED_PIN);                           // Erro de gravação
    delay(600);
    return false;
  }    
}
