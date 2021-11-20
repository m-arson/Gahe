#include <WiFi.h>
#include <WiFiClient.h>

WiFiClient client;

const char *ssid = "Hello Budi";
const char *pass = "Kepeleseketene";

const char *host = "192.168.1.11";
const int port = 80;

#define DD "------abc123\r\n"\
    "Content-Disposition: form-data;name='id'\r\n\r\n"\
    "1\r\n"\
    "------abc123\r\n"\
    "Content-Disposition: form-data;name='name'\r\n\r\n"\
    "Arson\r\n"\
    "------abc123--\r\n"

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.printf(".");
  }
  Serial.println("Connected!");
}
void loop() {
  if (client.connect(host, port)) {
    client.print("POST /handler/handler.php HTTP/1.1\r\n");
    client.print("Host: 192.168.1.11\r\n");
    client.print("Connection: keep-alive\r\n");
    client.print("Accept: */*\r\n");
    client.printf("Content-Length: %d\r\n", strlen(DD));
    client.print("Content-Type: multipart/form-data;boundary=----abc123\r\n\r\n");
    client.print(DD);
    
   
    unsigned long timeout = millis();
    while (client.available() == 0) {
        if (millis() - timeout > 5000) {
            Serial.printf(">>> Client Timeout ! %d\n", strlen(DD));
            client.stop();
            return;
        }
    }
    Serial.println("Reading Response : ");
    while (client.available()) {
      String line = client.readStringUntil('\r');
      Serial.println(line);
    }
  } else {
    Serial.println("Not Connect!");
  }
  delay(5000);
}
