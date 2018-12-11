// Incluir las librerias necesearias para el control de la placa Shield Ethernet W5100
// y para poder utilizarla como un servidor web
#include <SPI.h>
#include <Ethernet.h>

// Dirección MAC asignada a la placa Ethernet
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
// Dirección IP asignada a la placa Ethernet
IPAddress ip(192, 168, 1, 177);
// Inicialización de la librería en modo servidor web
// Utilizando el puerto 80 para comunicación mediante el protocolo HTTP
EthernetServer server(80);

// Definimos el pin analógico donde se conecta el sensor de temperatura
#define sensorT A1
// Definimos el pin analógico donde se conecta la fotoresistencia
#define LDRPin A0
// Definimos el pin digital donde se conecta la bocina
#define buzzer 9
// Definimos el pin digital donde se conecta el ventilador
const int pinventi = 8;

// DECLARACION DE VARIABLES PARA PINES DE ULTRASONICO
const int pinecho = 6;
const int pintrigger = 7;
// VARIABLES PARA CALCULOS DEL SENSOR ULTRASONICO
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
// Variable para saber si el ventialdor está activo o no (0=NO, 1=SI)
int ventiladorActivo = 0;

// Definición de pines digitales para el led RGB
int ledRojo = 5;
int ledVerde = 4;
int ledAzul = 3;

// Variable booleana para saber si el ventilador fue activado o no desde la página web
boolean ventiGET = false;

void setup(){
  // Configurar la comunicación serial.
  Serial.begin(9600);
  // Inicializamos la placa Ethernet con las direcciones declaradas previamente
  Ethernet.begin(mac, ip);
  server.begin();

  // Se muestra en el puerto serial la dirección IP configurada
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());

  // Configurar pin de salida para bocina
  pinMode(buzzer,OUTPUT);
  // Configurar pin de salida para ventilador
  pinMode(pinventi,OUTPUT);
  // CONFIGURAR PINES DE ENTRADA Y SALIDA PARA EL SENSOR ULTRASONICO
  pinMode(pinecho, INPUT);
  pinMode(pintrigger, OUTPUT);

  // Configurar pines de salida para led RGB
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

  //Los colores reflejados en el led RGB al activarse cada sensor son
  // Temperetura: Rojo, Fotoresistencia: Verde, Ultrasonico: Azul
  if (temperatura >= 40 && iluminacion >= 50 && distancia <= 8) {
    // Todos los sensores activados
    //Se guarda el tipo de alarma que esta activada
    tipoAlarma =7;
    //Se prende el led RGB en color blanco
    Color(255,255,255);
    //Accionar la bocina cuando se acciona el sensor de luminosidad y distancia
    tone(buzzer,800);
    //Prender el ventilador cuando se acciona el sensor de temperatura
    digitalWrite(pinventi, HIGH);
    delay(500);
  }
  else if (temperatura >= 40 && iluminacion >= 50) {
    // Sensores de Temperatura e Iluminación activados
    //Se guarda el tipo de alarma que esta activada
    tipoAlarma =6;
    //Se prende el led RGB en color amarillo
    Color(255,255,0);
    //Accionar la bocina cuando se acciona el sensor de luminosidad
    tone(buzzer,400);
    //Prender el ventilador cuando se acciona el sensor de temperatura
    digitalWrite(pinventi, HIGH);
    delay(500);
  }
  else if (temperatura >= 40 && distancia <= 8) {
    // Sensores de Temperatura y Distancia activados
    //Se guarda el tipo de alarma que esta activada
    tipoAlarma =5;
    //Se prende el led RGB en color rosa
    Color(255,0,255);
    //Accionar la bocina cuando se acciona el sensor de distancia
    tone(buzzer,600);
    //Prender el ventilador cuando se acciona el sensor de temperatura
    digitalWrite(pinventi, HIGH);
    delay(500);
  }
  else if (iluminacion >= 50 && distancia <= 8) {
    // Sensores de Luminosidad y Distancia activados
    //Se guarda el tipo de alarma que esta activada
    tipoAlarma =4;
    //Se prende el led RGB en color cyan
    Color(0,255,255);
    //Apagar el ventilador si estuviera prendido y si no está activado desde el sitio web
    if( !ventiGET ) {
      digitalWrite(pinventi, LOW);
    }
    //Accionar la bocina cuando se acciona el sensor de luminosidad y distancia
    tone(buzzer,800);
    delay(500);
  }
  else if (temperatura >= 40) {
    // Sensor de Temperatura activado
    //Se guarda el tipo de alarma que esta activada
    tipoAlarma =3;
    //Se prende el led RGB en color rojo
    Color(255,0,0);
    //Apagar la bocina si no estan activadas las alarmas de luminosidad o distancia
    noTone(buzzer);
    //Prender el ventilador cuando se acciona el sensor de temperatura
    digitalWrite(pinventi, HIGH);
    delay(500);
  }
  else if (iluminacion >= 50) {
    // Sensor de Iluminación activado
    //Se guarda el tipo de alarma que esta activada
    tipoAlarma =2;
    //Se prende el led RGB en color verde
    Color(0,255,0);
    //Accionar la bocina cuando se acciona el sensor de iluminación
    tone(buzzer,400);
    //Apagar el ventilador si estuviera prendido y si no está activado desde el sitio web
    if( !ventiGET ) {
      digitalWrite(pinventi, LOW);
    }
    delay(500);
  }
  else if (distancia <= 8) {
    // Sensor de Ditancia activado
    //Se guarda el tipo de alarma que esta activada
    tipoAlarma =1;
    //Se prende el led RGB en color azul
    Color(0,0,255);
    //Accionar la bocina cuando se acciona el sensor de distancia
    tone(buzzer,600);
    //Apagar el ventilador si estuviera prendido y si no está activado desde el sitio web
    if( !ventiGET ) {
      digitalWrite(pinventi, LOW);
    }
    delay(500);
  }else{
    // Ningún sensor activado
    //Se guarda el tipo de alarma que esta activada
    tipoAlarma =0;
    //Se prende el led RGB en color naranja
    Color(255,117,20);
    //Apagar la bocina si no estan activadas las alarmas de luminosidad o distancia
    noTone(buzzer);
    //Apagar el ventilador si estuviera prendido y si no está activado desde el sitio web
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

  // Detecta si hay una nueva conexión hacia el servidor web proporcionado por la placa Ethernet
  EthernetClient client = server.available();
  // Si hay una conexión activa cumple la condición
  if (client)  {
    // Variables para comprobar el final del envío de información hacia el servidor
    bool currentLineIsBlank = true;
    String cadena = "";
    // Mientras el cliente se encuentre en una conexión
    while (client.connected()) {
      // Si el cliente se encuentra disponible
      if (client.available()) {
        // Lee cada caracter transmitido por el servidor
        char c = client.read();
        // Lo envía al puerto serial
        Serial.write(c);

        if (cadena.length() < 50){
          // Concatena cada caracter para formar información útil
          cadena.concat(c);

          // Buscar campo data
          int posicion = cadena.indexOf("data");
           Serial.println( cadena );
          String command = cadena.substring(posicion);
         //Al recibir desde la página web el valor de data1 se efectuará una acción sobre el ventilador
          if (command == "data1=0") {
            //Si se recibe 0 se apaga el ventilador
            digitalWrite(pinventi, LOW);
            ventiGET = false;
          } else if (command == "data1=1") {
            //Si se recibe 1 se enciende el ventilador aunque no esté siendo activado por el sensor de temperatura
            digitalWrite(pinventi, HIGH);
            ventiGET = true;
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
           // Los datos mostrados por el servidor son las mediciones de los sensores
           // las cuáles son enviadas en un formato JSON para posteriormente
           // una página web cliente utilizar dichas mediciones y generar gráficas
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: application/json;charset=utf-8");

          // Transmisión de datos hacia el servidor web
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
          // Marca el fin de la transmisión
          currentLineIsBlank = true;
        }
        else if (c != '\r')
        {
          // Continúa la transmisión
          currentLineIsBlank = false;
        }
      }
    }

    delay(1);
    // Una vez completada la transmisión de los datos se cierra la conexión con el cliente
    client.stop();
  }
}

//Método para controlar el led RGB, recibiendo como parámetro un número entre 0 y 255 para cada atributo
void Color(int R, int G, int B) {
  analogWrite(ledRojo, R) ;   // Red    - Rojo
  analogWrite(ledVerde, G) ;   // Green - Verde
  analogWrite(ledAzul, B) ;   // Blue - Azul
}
