#include <SPI.h>
#include <Ethernet.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 177);
EthernetServer server(80);

const int pinLed1 = 4;
const int pinLed2 = 5;

// Definimos el pin analógico donde se conecta el sensor de temperatura
#define sensorT A1
// Definimos el pin analógico donde se conecta la fotoresistencia
#define LDRPin A0

#define buzzer 13
const int pinventi = 7;

// DECLARACION DE VARIABLES PARA PINES DE ULTRASONICO
const int pinecho = 9;
const int pintrigger = 8;

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

void setup()
{
  Serial.begin(9600);

  Ethernet.begin(mac, ip);
  server.begin();

  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());

  pinMode(pinLed1, OUTPUT);
  pinMode(pinLed2, OUTPUT);
  digitalWrite(pinLed1, LOW);
  digitalWrite(pinLed2, LOW);

  pinMode(buzzer,OUTPUT);
  pinMode(pinventi,OUTPUT);
  // CONFIGURAR PINES DE ENTRADA Y SALIDA
  pinMode(pinecho, INPUT);
  pinMode(pintrigger, OUTPUT);
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
  if (client)
  {
    Serial.println("new client");
    bool currentLineIsBlank = true;
    String cadena = "";
    while (client.connected())
    {
      if (client.available())
      {
        char c = client.read();
        Serial.write(c);

        if (cadena.length() < 50)
        {
          cadena.concat(c);

          // Buscar campo data
          int posicion = cadena.indexOf("data");
          String command = cadena.substring(posicion);

          if (command == "data1=0")
          {
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
          }
        }

        // Al recibir linea en blanco, servir página a cliente
        if (c == '\n' && currentLineIsBlank)
        {
          client.println(F("HTTP/1.1 200 OK\nContent-Type: text/html"));
          client.println();

          client.println(F("<html>\n<head>\n<title>Luis Llamas</title>\n</head>\n<body>"));
          client.println(F("<div style='text-align:center;'>"));
          client.println(F("<h2>Salidas digitales</h2>"));

          client.print(F("Estado LED 1 = "));
          client.println(digitalRead(pinLed1) == LOW ? "OFF" : "ON");
          client.println(F("<br/>"));
          client.println(F("<button onClick=location.href='./?data1=0'>ON</button>"));
          client.println(F("<button onClick=location.href='./?data1=1'>OFF</button>"));
          client.println(F("<br/><br/>"));

          client.print(F("Estado LED 2 = "));
          client.println(digitalRead(pinLed2) == LOW ? "OFF" : "ON");
          client.println(F("<br/>"));
          client.println(F("<button onClick=location.href='./?data2=0'>ON</button>"));
          client.println(F("<button onClick=location.href='./?data2=1'>OFF</button>"));
          client.println(F("<br/>"));
          client.println(F("<h1>Temperatura: "));
          client.println(temperatura);
          client.println(F(" grados C.</h1><br>"));
          client.println(F("<h1>Luminosidad: "));
          client.println(iluminacion);
          client.println(F(" lumens.</h1><br>"));
          client.println(F("<h1>Distancia: "));
          client.println(distancia);
          client.println(F(" cm.</h1><br>"));
          client.println(F("<a href='http://192.168.1.177'>Refrescar</a>"));
          client.println(F("</div>\n</body></html>"));
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
