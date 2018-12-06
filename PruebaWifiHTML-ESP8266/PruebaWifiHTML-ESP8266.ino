#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
MDNSResponder mdns;
//Cambiar estos valores por los de nuestra red wifi
const char* red = "PROFESORES";
const char* pass = "Cualquiera12";

ESP8266WebServer server(80);//Puerto 80

//Declaración de varibles
String pagina = "";

void setup(void){
// Botones para la página, esto está en html
 pagina += "<h1> Sistemas programables</h1><p>PIN: <a href=\"dos\"><button>2</button></a>&nbsp;<a href=\"tres\"><button>3</button></a>&nbsp"
 "<br></br>"
 "<br></br>";
  delay(1000); 
  Serial.begin(115200);
  WiFi.begin(red, pass);
  Serial.println("");

  // Esperando para conectarnos
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Conectado a: ");
  Serial.println(red);
  Serial.print("Direccion IP: ");
  Serial.println(WiFi.localIP());//Nos muestra nuestra dirección ip
  
  if (mdns.begin("esp8266", WiFi.localIP())) {
    Serial.println("La conexion ha sido iniciada");
  }
  
  server.on("/", [](){
    server.send(200, "text/html", pagina);
  });
//Si recibimos el parámetro "dos" nos envía por el puerto serial del esp8266 al arduino el caracter 2
  server.on("/dos", [](){
    server.send(200, "text/html", pagina);
//    digitalWrite(pin0, HIGH);
    Serial.println("2");
    delay(100);
  });
  server.on("/tres", [](){
    server.send(200, "text/html", pagina);
//    digitalWrite(pin0, HIGH);
    Serial.println("3");
    delay(100);
  });
  server.begin();
  Serial.println("HTTP server started");
}
void loop(void){
  server.handleClient();
} 
