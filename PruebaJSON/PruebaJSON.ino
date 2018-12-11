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
/* Variable de tipo alarma, para saber que alarmas estan activas
 * 7 Todas las alarmas
 * 6 Temperatura e iluminación
 * 5 Temperatura y distancia
 * 4 Iluminación y distancia
 * 3 Temperatura
 * 2 Iluminación
 * 1 Distancia
 * 0 Ninguna activada
 */
int tipoAlarma = 0;
int ventiladorActivo = 0;

int ledRojo = 5;
int ledVerde = 4;
int ledAzul = 3;

boolean ventiGET = false;
void setup(){
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
  if (temperatura >= 40 && iluminacion >= 50 && distancia <= 8) {
    //Serial.println("Todos los sensores");
    //Se guarda el tipo de alarma que esta activada
    tipoAlarma =7;
    //Se prende el led RGB en color blanco
    Color(255,255,255);
    //Accionar el tipo de alarma, cuando se acciona la alarma de iluminosidad y distancia
    tone(buzzer,800);
    //Prender el ventilador
    digitalWrite(pinventi, HIGH);
    delay(500);
  }
  else if (temperatura >= 40 && iluminacion >= 50) {
    //Se prende el led RGB en color amarillo
    //Serial.println("Temp e ilum");
    //Se guarda el tipo de alarma que esta activada
    tipoAlarma =6;
    //Se prende el led RGB en color amarillo
    Color(255,255,0);
    //Accionar el tipo de alarma, para la iluminación
    tone(buzzer,400);
    //Prender el ventilador
    digitalWrite(pinventi, HIGH);
    delay(500);
  }
  else if (temperatura >= 40 && distancia <= 8) {
    //Serial.println("Temp y dist");
    //Se guarda el tipo de alarma que esta activada
    tipoAlarma =5;
    //Se prende el led RGB en color rosa
    Color(255,0,255);
    //Accionar el tipo de alarma, para la distancia
    tone(buzzer,600);
    //Prender el ventilador
    digitalWrite(pinventi, HIGH);
    delay(500);
  }
  else if (iluminacion >= 50 && distancia <= 8) {
    //Serial.println("Ilum y dist");
    //Se guarda el tipo de alarma que esta activada
    tipoAlarma =4;
    //Se prende el led RGB en color cyan
    Color(0,255,255);
    //Apagar el ventilador si estuviera prendido
    if( !ventiGET ) {
      digitalWrite(pinventi, LOW);
    }
    //Accionar el tipo de alarma, cuando se acciona la alarma de iluminosidad y distancia
    tone(buzzer,800);
    delay(500);
  }
  else if (temperatura >= 40) {
    //Serial.println("Temperatura");
    //Se guarda el tipo de alarma que esta activada
    tipoAlarma =3;
    //Se prende el led RGB en color rojo
    Color(255,0,0);
    //Apagar la bocina si no estan activadas las alarmas de iluminosidad o distancia
    noTone(buzzer);
    //Prender el ventilador
    digitalWrite(pinventi, HIGH);
    delay(500);
  }
  else if (iluminacion >= 50) {
    //Serial.println("Iluminiacion");
    //Se guarda el tipo de alarma que esta activada
    tipoAlarma =2;
    //Se prende el led RGB en color verde
    Color(0,255,0);
    //Accionar el tipo de alarma, para iluminosidad
    tone(buzzer,400);
    //Apagar el ventilador si estuviera prendido
    if( !ventiGET ) {
      digitalWrite(pinventi, LOW);
    }
    delay(500);
  }
  else if (distancia <= 8) {
    //Serial.println("Distancia");
    //Se guarda el tipo de alarma que esta activada
    tipoAlarma =1;
    //Se prende el led RGB en color azul
    Color(0,0,255);
    //Accionar el tipo de alarma, para distancia
    tone(buzzer,600);
    //Apagar el ventilador si estuviera prendido
    if( !ventiGET ) {
      digitalWrite(pinventi, LOW);
    }
    delay(500);
  }else{
    //Serial.println("Default");
    //Se guarda el tipo de alarma que esta activada
    tipoAlarma =0;
    //Se prende el led RGB en color naranja
    Color(255,117,20);
    //Apagar la bocina si no estan activadas las alarmas de iluminosidad o distancia
    noTone(buzzer);
    //Prender el ventilador
    if( !ventiGET ) {
      digitalWrite(pinventi, LOW);
    }
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
   //------------- Serial.println("new client");
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
         //Prueba luis llamas, IGNORAR
         //Sirve para saber la forma en la cual se reciben los datos desde el método get
         
          if (command == "data1=0") { //Ventilador
            digitalWrite(pinventi, LOW);
            ventiGET = false;
          } else if (command == "data1=1") {
            digitalWrite(pinventi, HIGH);
            ventiGET = true;
          } else if (command == "data2=0") {
            //digitalWrite(pinLed2, LOW);
          } else if (command == "data2=1"){
            //digitalWrite(pinLed2, HIGH);
          }
        }

        // Al recibir linea en blanco, servir página a cliente
        if (c == '\n' && currentLineIsBlank) {
          /* Variable de tipo alarma, para saber que alarmas estan activas
           * 7 Todas las alarmas
           * 6 Temperatura e iluminación
           * 5 Temperatura y distancia
           * 4 Iluminación y distancia
           * 3 Temperatura
           * 2 Iluminación
           * 1 Distancia
           * 0 Ninguna activada
           */
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: application/json;charset=utf-8");
          
          client.println();
          client.print(F("{\"temp\": \""));
          client.print(temperatura);
          client.print(F("\", \"lumels\": \""));
          client.print(iluminacion);
          client.print(F("\", \"distancia\": \""));
          client.print(distancia);
          client.print(F("\", \"tipo_alarma\": \""));
          client.print(tipoAlarma);
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
