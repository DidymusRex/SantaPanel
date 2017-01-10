//
// File: SantaPanel.ino
// Desc: Control XMas tree and outside lights via two relays
//       connected to a ESP8266 "Thing"
// Audit Trail
//  12/01/2016 John Grover
//             Original Code
//  12/17/2016 John Grover
//             Added color and formatting to HTML
//

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

// Replace with your network credentials
const char* ssid = "Lincoln Manor";
const char* password = "...---... sos ...---...";

IPAddress ip(192, 168, 0, 125);                                                              
IPAddress ns(8, 8, 8, 8);
IPAddress gw(192, 168, 0, 1);
IPAddress sn(255, 255, 255, 0);

ESP8266WebServer server(80);

int LEDYLW = 4;
int LEDGRN = 5;
int RELAY1 = 12;
int RELAY2 = 13;
bool r1open = true;
bool r2open = true;

String webPage(){
  String s = "";
  s = "<!DOCTYPE HTML><HTML><HEAD><TITLE>Santa Panel</TITLE>";
  s += "<style>body{font-family:sans-serif;font-size:2em;}";
  s += "a:link,a:visited{color:white;text-decoration:none;}";
  s += ".grn{background-color:green;border:1px dotted black;}";
  s += ".red{background-color:red;border:1px dotted black;}";
  s += ".off{background-color:#444444;border:1px dotted black;}";
  s += "</style></HEAD><BODY>";
  s += "<h1 align=center>Santa Panel!</><TABLE ALIGN=CENTER BORDER=0 WIDTH=50%>";
  s += "<TR><TD><a href=t-on><div class=";
  if (r1open) {s += "off";} else {s += "red";}
  s += ">TREE ON</div></a></TD>";
  s += "<TD><a href=t-off><div class=";
  if (r1open) {s += "red";} else {s += "off";}
  s += ">TREE OFF</div></a></TR>";
  s += "<TR><TD><a href=l-on><div class=";
  if (r2open) {s += "off";} else {s += "grn";}
  s += ">LITE ON</div></a></TD>";
  s += "<TD><a href=l-off><div class=";
  if (r2open) {s += "grn";} else {s +="off";}
  s += ">LITE OFF</div></a></TR>";
  s += "</TABLE></BODY></HTML>";

  return s;
}

void setup(void){
  // preparing GPIOs
  pinMode(LEDYLW, OUTPUT);
  digitalWrite(LEDYLW, LOW);
  
  pinMode(RELAY1, OUTPUT);
  digitalWrite(RELAY1, HIGH);
  r1open = true;
  
  pinMode(LEDGRN, OUTPUT);
  digitalWrite(LEDGRN, LOW);
  
  pinMode(RELAY2, OUTPUT);
  digitalWrite(RELAY2, HIGH);
  r2open = true;
  
  Serial.begin(74880); 
  delay(1000);
  Serial.println("");

  WiFi.begin(ssid, password);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi?");
    digitalWrite(LEDYLW, HIGH);
    delay(250);
    digitalWrite(LEDYLW, LOW);
    delay(250);
  }
  
  // Use a static IP
  WiFi.config(ip, ns, gw, sn);
  digitalWrite(LEDGRN, HIGH);

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  delay(250);
  digitalWrite(LEDGRN, LOW);
  
  // root page
  server.on("/", [](){
    server.send(200, "text/html", webPage());
  });

  //tree on
  server.on("/t-on", [](){
    digitalWrite(LEDGRN, HIGH);
    digitalWrite(RELAY1, LOW);
    r1open = false;
    server.send(200, "text/html", webPage());
    delay(500);
  });
  //tree off
  server.on("/t-off", [](){
    digitalWrite(LEDGRN, LOW);
    digitalWrite(RELAY1, HIGH);
    r1open = true;
    server.send(200, "text/html", webPage());
    delay(500); 
  });

  // lights on
  server.on("/l-on", [](){
    digitalWrite(LEDYLW, HIGH);
    digitalWrite(RELAY2, LOW);
    r2open = false;
    server.send(200, "text/html", webPage());
    delay(500);
  });
  // lights off
  server.on("/l-off", [](){
    digitalWrite(LEDYLW, LOW);
    digitalWrite(RELAY2, HIGH);
    r2open = true;
    server.send(200, "text/html", webPage());
    delay(500); 
  });

  server.begin();
  Serial.println("HTTP server started");
}
 
void loop(void){
  server.handleClient();
}

