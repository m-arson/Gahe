#include <WiFi.h>
#include <WiFiClient.h>
#include <SPIFFS.h>

WiFiClient client;

const char *ssid = "Hello Budi";
const char *pass = "Kepeleseketene";

const char *host = "192.168.1.11";
const int port = 80;

#define CHUNK_SIZE 256

#define DD_OPEN "------abc123\r\n"\
  "Content-Disposition: form-data;name='id'\r\n\r\n"\
  "1\r\n"\
  "------abc123\r\n"\
  "Content-Disposition: form-data;name='name'\r\n\r\n"\
  "Arson\r\n"\
  "------abc123\r\n"\
  "Content-Disposition: form-data;name='gambar';filename='image.png'\r\n"\
  "Content-Type: application/octet-stream\r\n\r\n"

#define DD_CLOSE "\r\n------abc123--\r\n"

File root;

void setup() {
  Serial.begin(115200);
  if (!SPIFFS.begin(true)) {
    Serial.println("Mount Failed...");
  }
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.printf(".");
  }
  Serial.println("Connected!");
}
void loop() {
  if (client.connect(host, port)) {
    root = SPIFFS.open("/image.png");
    unsigned long sizee = strlen(DD_OPEN) + strlen(DD_CLOSE) + root.size();
    if (root) {
      client.print("POST /handler/handler.php HTTP/1.1\r\n");
      client.print("Host: 192.168.1.11\r\n");
      client.print("Connection: keep-alive\r\n");
      client.print("Accept: */*\r\n");
      client.printf("Content-Length: %ld\r\n", sizee);
      client.print("Content-Type: multipart/form-data;boundary=----abc123\r\n\r\n");
      client.print(DD_OPEN);
      Serial.println("Sending Data...");
      if (root.available()) {
        long imglen = root.size();
        while (imglen > 0) {
          if (imglen / CHUNK_SIZE >= 1) {
            char data[CHUNK_SIZE];
            for (int i = 0; i < CHUNK_SIZE; ++i) {
              data[i] = (char) root.read();
            }
            client.write((char *) data, CHUNK_SIZE);
            imglen -= CHUNK_SIZE;
          } else {
            char data[imglen];
            for (int i = 0; i < imglen; ++i) {
              data[i] = (char) root.read();
            }
            client.write((char *) data, imglen);
            imglen -= imglen;
          }
        }
      }
      client.print(DD_CLOSE);
      root.close();


      unsigned long timeout = millis();
      while (client.available() == 0) {
        if (millis() - timeout > 5000) {
          Serial.printf(">>> Client Timeout !\n");
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
      Serial.println("File Not Found!!");
    }
  } else {
    Serial.println("Not Connect!");
  }
  delay(5000);
}
