// El número del pin del LED
const int led2 =  2;
const int led3 =  3;
// Variable de lectura del estado de los pines
int ledEstado2 = 0;
int ledEstado3 = 0;
String inputString = "";         // Una cadena para almacenar datos entrantes
boolean stringComplete = false;  // Si la cadena está completa

void setup() {

   // Inicializar los pines como una salida:
  pinMode(led3, OUTPUT);
  pinMode(led2, OUTPUT);
   // Inicializa el puerto serial a 9600
  Serial.begin(115200);
  // Reserva de 200 bytes para el inputString
  inputString.reserve(200);
}

//----------------------------------------------------
//Esos datos son asignados a la variable inputString
void serialEvent() {
  while (Serial.available()) {
    // Obtener el nuevo byte:
    char inChar = (char)Serial.read();
    // Si el caracter recibido es una nueva linea 
    //La cadena está completa
    inputString += inChar;
      if (inChar == '\n') {
      stringComplete = true;
    }
  }
}
//-----------------------------------------------------
void loop() {

   
    // ---------------3----------------------
  if (inputString == "3") {
    ledEstado3 = digitalRead(led3);
    delay(200);
    // Enciende Led 3
    if( ledEstado3== 0){
    digitalWrite(led3, HIGH);
    }
    }
    // Apaga Led 3
    if( ledEstado3== 1){
    digitalWrite(led3, LOW);
    }
    // --------------2----------------------
  if (inputString == "2") {
    ledEstado2 = digitalRead(led2);
    delay(200);
    // Enciende Led 2
    if( ledEstado2== 0){
    digitalWrite(led2, HIGH);
    }
    }
    // Apaga Led 2
    if( ledEstado2== 1){
    digitalWrite(led2, LOW);
    }
  //---------------------------------------  
  //Imprime la cadena "inputString" cuando llega una nueva línea
  if (stringComplete) {
    Serial.println(inputString);
    // Borrar la cadena, para el proximo uso
    inputString = "";
    stringComplete = false;
  }
}
