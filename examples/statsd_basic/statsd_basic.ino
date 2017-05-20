#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#include <Statsd.h>

// Create the UDP object and configure the StatsD client.
WiFiUDP udp;  // or EthernetUDP, as appropriate.
Statsd statsd(udp, "192.168.0.16", 8125);


void setup() {
  const char* ssid = "my_wifi_ssid";
  const char* wifi_password = "my_wifi_password";

  Serial.begin(115200);

  // Connect to Wifi.
  Serial.printf("\n\n\nConnecting to %s ", ssid);
  WiFi.begin(ssid, wifi_password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected");

  delay(100);

  // Once connected tell client to begin.
  statsd.begin();
}

void loop() {
  int val = random(20);
  // Emit a named metric with given value, some number of comma separated tags at a given sample rate.
  // metric: some.metric
  // value: val
  // tags: some_tag, whatever: 10
  // sample rate: 0 -> 1.0 (100%)
  statsd.gauge("some.metric", val, "some_tag,whatever=10", 1.0);
  delay(1000);
}
