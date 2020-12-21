#ifndef Statsd_h
#define Statsd_h

#include <Arduino.h>
#include <IPAddress.h>
#include <Udp.h>

#define TAG_STYLE_DATADOG 1
#define TAG_STYLE_TELEGRAF 2


class Statsd {
public:
    Statsd(UDP& udp, String host, uint16_t port);
    Statsd(UDP& udp, String host, uint16_t port, String tags);
    void setTagStyle(int style);
    int begin();
    // Event
    void event(String title, String text, String tags);
    void event(String title, String text);
    // Count
    void count(String metric, float value, String tags, float sample_rate);
    void count(String metric, float value);
    void count(String metric, float value, float sample_rate);
    // Gauge
    void gauge(String metric, float value, String tags, float sample_rate);
    void gauge(String metric, float value);
    void gauge(String metric, float value, float sample_rate);
    // Set
    void set(String metric, float value, String tags, float sample_rate);
    void set(String metric, float value);
    void set(String metric, float value, float sample_rate);
    // Timing
    void timing(String metric, float value, String tags, float sample_rate);
    void timing(String metric, float value);
    void timing(String metric, float value, float sample_rate);
    // Increment
    void increment(String metric, String tags, float sample_rate);
    void increment(String metric);
    void increment(String metric, float sample_rate);
    // Decrement
    void decrement(String metric, String tags, float sample_rate);
    void decrement(String metric);
    void decrement(String metric, float sample_rate);
private:
    String joinTags(String t1, String t2);
    String formatTags(String constant_tags, String tags);
    String formatSampleRate(float sample_rate);
    bool shouldSend(float sample_rate);
    void send(String metric, float value, String tags, String type, float sample_rate);
    UDP &_udp;
    String _host;
    uint16_t _port;
    String _constant_tags;
    int _tag_style;
};

#endif
