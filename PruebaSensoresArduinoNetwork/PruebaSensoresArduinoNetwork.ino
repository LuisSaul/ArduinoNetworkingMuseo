// Definimos el pin analógico donde se conecta el sensor de temperatura
#define sensorT A1
// Definimos el pin analógico donde se conecta la fotoresistencia
#define LDRPin A0

#define buzzer 13
const int pinled = 11;
const int pinventi = 7;

// DECLARACION DE VARIABLES PARA PINES
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

void setup() {
  // Inicializamos comunicación serie
  Serial.begin(115200);
  // Comenzamos el sensor DHT
  dht.begin();
  pinMode(pinled, OUTPUT);
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
  // Mostramos la temperatura obtenida por el sensor
  Serial.print("Temperatura: ");
  Serial.print(temperatura);
  Serial.println("°C");

  if (temperatura >= 30) {
    digitalWrite(pinled, HIGH);
    tone(buzzer,400);
    digitalWrite(pinventi, HIGH);
    delay(500);
  } else {
    digitalWrite(pinled, LOW);
    noTone(buzzer);
    digitalWrite(pinventi, LOW);
  }

  // ----------LUMINOSIDAD----------
  // Leemos el voltaje producido por la fotoresistencia
  V = analogRead(LDRPin);
  // Calculamos la luminosidad en base al cálculo anterior
  iluminacion = ((long)V*A*10)/((long)B*Rc*(1024-V));
  /*Serial.print("Iluminación: ");
  Serial.print(iluminacion);
  Serial.println(" lumens");

  if (iluminacion >= 50) {
    digitalWrite(pinled, HIGH);
    tone(buzzer,800);
    delay(500);
  } else {
    digitalWrite(pinled, LOW);
    noTone(buzzer);
  }*/

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
 
  // ENVIAR EL RESULTADO AL MONITOR SERIAL
  Serial.print(distancia);
  Serial.println(" cm");
  delay(200);
 
  // ENCENDER EL LED CUANDO SE CUMPLA CON CIERTA DISTANCIA
  /*if (distancia <= 15) {
    digitalWrite(pinled, HIGH);
    tone(buzzer,400);
    delay(500);
  } else {
    digitalWrite(pinled, LOW);
    noTone(buzzer);
  }*/

  //Serial.println("");
  // Hacemos una pausa de 5 segundos entre cada medición
  //delay(5000);
}
