#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#include <Statsd.h>

// Create the UDP object and configure the StatsD client.
WiFiUDP udp;  // or EthernetUDP, as appropriate.
// The last argument is the comma separated list of tags to be applied to
// all metrics emitted by this client.
Statsd statsd(udp, "192.168.0.16", 8125, "host=speck");


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
  // If sending to DogStatsd or gostatsd use the Datadog tag style.
  // stats.setTagStyle(TAG_STYLE_DATADOG);
  // If sending to Telegraf use its own tag style.
  statsd.setTagStyle(TAG_STYLE_TELEGRAF);
}

void loop() {
  int val = random(20);
  // Emit a named metric with given value, some number of comma separated tags at a given sample rate.
  // metric: some.metric
  // value: val
  // tags: sensor, location: basement (and host: speck, configured in the constant tags for the client)
  // sample rate: 0 -> 1.0 (100%)
  // Use tags when you have a metric that will benefit from being able to drill down by tags.
  // In this example we include the information that this datapoint comes from a sensor in
  // the basement. If we had many sensors and this was a temperature reading we could look
  // at the min, max and averages of temperatures in a building as well as those in a particular area.
  // Note that when sending tags as a k/v pair, your tags should be formatted as k=v when sending
  // to Telegraf, and k:v when sending to Datadog/gostatsd.
  statsd.gauge("some.metric", val, "sensor,location=basement", 1.0);
  delay(1000);
}
