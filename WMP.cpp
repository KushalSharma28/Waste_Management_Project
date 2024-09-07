#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <Servo.h>
Adafruit_SSD1306 o(LED_BUILTIN);
Servo myservo;
const int echo = D7;
const int trigger = D8;
const int echo2 = D5;
const int trigger2 = D3;
long dur;
int dist;
long dur2;
int dist2;
const char* ssid     = "waste";
const char* password = "12345678";
const char* host = "cloud.sistec.ac.in";
void setup()
{
  Serial.begin(9600);
  myservo.attach(D0);
  pinMode(trigger, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(trigger2, OUTPUT);
  pinMode(echo2, INPUT);
  o.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  o.clearDisplay();
  o.setTextColor(WHITE);
  o.setTextSize(2);
  o.setCursor(0, 0);
  o.print("SMART");
  o.setCursor(0, 20);
  o.print("WASTE");
  o.setCursor(0, 40);
  o.print("MANAGEMENT");
  o.display();
  delay(2000);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

}
void loop()
{
  digitalWrite(trigger, LOW);
  delayMicroseconds(2);
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger, LOW);
  dur = pulseIn(echo, HIGH);
  dist = dur * 0.034 / 2;
  if(dist < 1)
  {
    dist = 1;
  }
  else if(dist > 19)
  {
    dist = 19;
  }
  int distPer = map(dist,19,0,1,100);
  digitalWrite(trigger2, LOW);
  delayMicroseconds(2);
  digitalWrite(trigger2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger2, LOW);
  dur2 = pulseIn(echo2, HIGH);
  dist2 = dur2 * 0.034 / 2;

  Serial.println(dist);
  Serial.println(dist2);
  if (dist2 <= 40)
  {
    myservo.write(150);
  }
  else
  {
    myservo.write(1);
  }
  o.clearDisplay();
  o.setTextSize(1);
  o.setCursor(0, 0);
  o.print("SmartWaste Management");
  o.println();
  o.setTextSize(2);
  o.setCursor(0, 16);
  o.print("S1: ");
  o.print(distPer);
  o.print(" %");
  o.setCursor(44, 16);
  o.println();
  o.print("S2: ");
  o.print(dist2);
  o.print(" cm");
  o.display();
  delay(1000);

  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) 
  {
    Serial.println("connection failed");
    return;
  }
  
  String url = "http://cloud.sistec.ac.in/0187cs161049/getData.php";
  url += "level=";
  url += distPer;
  url += "&bin_id=AC89";
  
  Serial.print("Requesting URL: ");
  Serial.println(url);
  client.print(String("GET ") + url + " HTTP/1.1\r\n" + 
                "Host: " + host + "\r\n" + 
                "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) 
  {
    if (millis() - timeout > 5000) 
    {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }
  while(client.available())
  {
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  
  Serial.println();
  Serial.println("closing connection");
}
