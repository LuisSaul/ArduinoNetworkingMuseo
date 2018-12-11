#include <SPI.h>
#include <Ethernet.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 177);
EthernetServer server(80);

// Definimos el pin analógico donde se conecta el sensor de temperatura
#define sensorT A1
// Definimos el pin analógico donde se conecta la fotoresistencia
#define LDRPin A0

#define buzzer 9
const int pinventi = 8;

// DECLARACION DE VARIABLES PARA PINES DE ULTRASONICO
const int pinecho = 6;
const int pintrigger = 7;

// VARIABLES PARA CALCULOS
unsigned int tiempo, distancia;

// Declaramos variables necesarias para calcular la temperatura
float voltaje = 0;
float temperatura = 0;
// Declaramos constantes necesarias para calcular la luminosidad
const long A = 1000; //Resistencia en oscuridad en KΩ
const int B = 15; //Resistencia a la luz (10 Lux) en KΩ
const int Rc = 10; //Resistencia calibracion en KΩ
// Declaramos variables necesarias para calcular la luminosidad
int V = 0;
int iluminacion = 0;

int bocinaActiva = 0;
int ventiladorActivo = 0;

int ledRojo = 5;
int ledVerde = 4;
int ledAzul = 3;

void setup()
{
  Serial.begin(9600);

  Ethernet.begin(mac, ip);
  server.begin();

  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
  
  pinMode(buzzer,OUTPUT);
  pinMode(pinventi,OUTPUT);
  // CONFIGURAR PINES DE ENTRADA Y SALIDA
  pinMode(pinecho, INPUT);
  pinMode(pintrigger, OUTPUT);

  //Configurar pines para led RGB
  
  pinMode(ledRojo, OUTPUT);
  pinMode(ledVerde, OUTPUT);
  pinMode(ledAzul, OUTPUT);
}

void loop() {
  // ----------TEMPERATURA----------
  // Leemos el voltaje producido por el sensor de temperatura
  voltaje = analogRead(sensorT)*3.3/1023;
  // Calculamos la temperatura en base al cálculo anterior
  temperatura = voltaje*100;
  
  // ----------LUMINOSIDAD----------
  // Leemos el voltaje producido por la fotoresistencia
  V = analogRead(LDRPin);
  // Calculamos la luminosidad en base al cálculo anterior
  iluminacion = ((long)V*A*10)/((long)B*Rc*(1024-V));
  /*Serial.print("Temperatura");
  Serial.println(temperatura);
  Serial.print("Iluminacion");
  Serial.println(iluminacion);
  Serial.print("Distancia");
  Serial.println(distancia);*/
  //los colores de cada componente son: Temperetura: Rojo, Fotoresistencia: Verde, Ultrasonico: Azul
  if (temperatura >= 40 && iluminacion >= 50 && distancia <= 20) {
    Serial.println("Todos los sensores");
    //Se prende el led RGB en color blanco
    Color(255,255,255);
    tone(buzzer,800);
    digitalWrite(pinventi, HIGH);
    delay(500);
  }
  else if (temperatura >= 40 && iluminacion >= 50) {
    //Se prende el led RGB en color amarillo
    Serial.println("Temp y ilum");
    
    //Se prende el led RGB en color amarillo
    Color(255,255,0);
    tone(buzzer,400);
    digitalWrite(pinventi, HIGH);
    delay(500);
  }
  else if (temperatura >= 40 && distancia <= 20) {
    Serial.println("Temp y dist");
    
    //Se prende el led RGB en color rosa
    Color(255,0,255);
    tone(buzzer,600);
    digitalWrite(pinventi, HIGH);
    delay(500);
  }
  else if (iluminacion >= 50 && distancia <= 20) {
    Serial.println("Ilum y dist");
    
    //Se prende el led RGB en color cyan
    Color(0,255,255);
    digitalWrite(pinventi, LOW);
    tone(buzzer,800);
    //tone(buzzer,800);
    delay(500);
  }
  else if (temperatura >= 40) {
    Serial.println("Temperatura");
    //Se prende el led RGB en color rojo
    Color(255,0,0);
    noTone(buzzer);
    digitalWrite(pinventi, HIGH);
    delay(500);
  }
  else if (iluminacion >= 50) {
    Serial.println("Iluminiacion");
    //Se prende el led RGB en color verde
    Color(0,255,0);
    tone(buzzer,400);
    digitalWrite(pinventi, LOW);
    delay(500);
  }
  else if (distancia <= 20) {
    Serial.println("Distancia");
    //Se prende el led RGB en color azul
    Color(0,0,255);
    tone(buzzer,600);
    digitalWrite(pinventi, LOW);
    delay(500);
  }else{
    Serial.println("Default");
    //Se prende el led RGB en color naranja
    Color(255,117,20);
    noTone(buzzer);
    digitalWrite(pinventi, LOW);
    delay(500);
  }
  // ENVIAR PULSO DE DISPARO EN EL PIN "TRIGGER"
  digitalWrite(pintrigger, LOW);
  delayMicroseconds(2);
  digitalWrite(pintrigger, HIGH);
  // EL PULSO DURA AL MENOS 10 uS EN ESTADO ALTO
  delayMicroseconds(10);
  digitalWrite(pintrigger, LOW);

  // MEDIR EL TIEMPO EN ESTADO ALTO DEL PIN "ECHO" EL PULSO ES PROPORCIONAL A LA DISTANCIA MEDIDA
  tiempo = pulseIn(pinecho, HIGH);

  // LA VELOCIDAD DEL SONIDO ES DE 340 M/S O 29 MICROSEGUNDOS POR CENTIMETRO
  // DIVIDIMOS EL TIEMPO DEL PULSO ENTRE 58, TIEMPO QUE TARDA RECORRER IDA Y VUELTA UN CENTIMETRO LA ONDA SONORA
  distancia = tiempo / 58;


  EthernetClient client = server.available();
  if (client)  {
    Serial.println("new client");
    bool currentLineIsBlank = true;
    String cadena = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);

        if (cadena.length() < 50){
          cadena.concat(c);

          // Buscar campo data
          int posicion = cadena.indexOf("data");
          Serial.println( cadena );
          String command = cadena.substring(posicion);
/*
          if (command == "data1=0") {
            digitalWrite(pinLed1, HIGH);
          }
          else if (command == "data1=1")
          {
            digitalWrite(pinLed1, LOW);
          }
          else if (command == "data2=0")
          {
            digitalWrite(pinLed2, LOW);
          }
          else if (command == "data2=1")
          {
            digitalWrite(pinLed2, HIGH);
          }*/
        }

        // Al recibir linea en blanco, servir página a cliente
        if (c == '\n' && currentLineIsBlank)
        {
          
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: application/json;charset=utf-8");
          
          client.println();
          client.print(F("{\"temp\": \""));
          client.print(temperatura);
          client.print(F("\", \"lumels\": \""));
          client.print(iluminacion);
          client.print(F("\", \"super\": \""));
          client.print(distancia);
          client.println(F("\"}"));
          client.println();
          break;
        }
        if (c == '\n')
        {
          currentLineIsBlank = true;
        }
        else if (c != '\r')
        {
          currentLineIsBlank = false;
        }
      }
    }

    delay(1);
    client.stop();
  }
}

void Color(int R, int G, int B) {
  analogWrite(ledRojo, R) ;   // Red    - Rojo
  analogWrite(ledVerde, G) ;   // Green - Verde
  analogWrite(ledAzul, B) ;   // Blue - Azul
}
