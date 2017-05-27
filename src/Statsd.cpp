#include "Statsd.h"

Statsd::Statsd(UDP &udp, String host, uint16_t port):
    _udp(udp), _host(host), _port(port), _constant_tags(String("")) {
}

Statsd::Statsd(UDP &udp, String host, uint16_t port, String tags):
    _udp(udp), _host(host), _port(port), _constant_tags(tags) {
}

void Statsd::setTagStyle(int style) {
    switch (style) {
        case TAG_STYLE_DATADOG:
        case TAG_STYLE_TELEGRAF:
            _tag_style = style;
    }
}

int Statsd::begin() {
    return _udp.beginPacket(_host.c_str(), _port);
}

inline String Statsd::joinTags(String t1, String t2) {
    if(t1.length() == 0)
        return t2;
    if(t2.length() == 0)
        return t1;
    return t1 + ',' + t2;
}

inline String Statsd::formatTags(String constant_tags, String tags) {
    if(constant_tags.length() == 0 && tags.length() == 0) {
        return "";
    }
    switch (_tag_style) {
        case TAG_STYLE_TELEGRAF:
            return String(",") + joinTags(constant_tags, tags);
        case TAG_STYLE_DATADOG:
            return String("|#") + joinTags(constant_tags, tags);
        default:
            return "";
    }
}

inline String Statsd::formatSampleRate(float sample_rate) {
    if(sample_rate >= 1.0) {
        // Not required if sample rate is 100%.
        return "";
    }
    return String("|@") + sample_rate;
}

inline bool Statsd::shouldSend(float sample_rate) {
    if(sample_rate >= 1.0)
        return true;

    return random(100) < (sample_rate * 100);
}

void Statsd::send(String metric, int value, String tags, String type, float sample_rate) {
    if(!shouldSend(sample_rate))
        return;
    String msg;
    switch (_tag_style) {
        case TAG_STYLE_TELEGRAF:
            msg = metric + formatTags(_constant_tags, tags) + ':' + value + '|' + type + formatSampleRate(sample_rate);
            break;
        case TAG_STYLE_DATADOG:
        default:
            msg = metric + ':' + value + '|' + type + formatSampleRate(sample_rate) + formatTags(_constant_tags, tags);
    }
    const char *c_msg = msg.c_str();
    _udp.write((uint8_t *)c_msg, strlen(c_msg));
    _udp.endPacket();
}

// Event - a Datadog extension. see http://docs.datadoghq.com/guides/dogstatsd/#events-1
// Sending events to agents other than DogStatsd and gostatsd probably won't hurt,
// but it's not likely to do anything useful.
void Statsd::event(String title, String text, String tags) {
    String msg = String("_e{") + title.length() + "," + text.length() + "}:" + title + "|" + text + formatTags(_constant_tags, tags);
    const char *c_msg = msg.c_str();
    _udp.write((uint8_t *)c_msg, strlen(c_msg));
    _udp.endPacket();
}

void Statsd::event(String title, String text) {
    event(title, text, "");
}

// Count
void Statsd::count(String metric, int value, String tags, float sample_rate) {
    send(metric, value, tags, "c", sample_rate);
}

void Statsd::count(String metric, int value) {
    send(metric, value, "", "c", 1.0);
}

void Statsd::count(String metric, int value, float sample_rate) {
    send(metric, value, "", "c", sample_rate);
}

// Gauge
void Statsd::gauge(String metric, int value, String tags, float sample_rate) {
    send(metric, value, tags, "g", sample_rate);
}

void Statsd::gauge(String metric, int value) {
    send(metric, value, "", "g", 1.0);
}

void Statsd::gauge(String metric, int value, float sample_rate) {
    send(metric, value, "", "g", sample_rate);
}

// Set
void Statsd::set(String metric, int value, String tags, float sample_rate) {
    send(metric, value, tags, "s", sample_rate);
}

void Statsd::set(String metric, int value) {
    send(metric, value, "", "s", 1.0);
}

void Statsd::set(String metric, int value, float sample_rate) {
    send(metric, value, "", "s", sample_rate);
}

// Timing
void Statsd::timing(String metric, int value, String tags, float sample_rate) {
    send(metric, value, tags, "ms", sample_rate);
}

void Statsd::timing(String metric, int value) {
    send(metric, value, "", "ms", 1.0);
}

void Statsd::timing(String metric, int value, float sample_rate) {
    send(metric, value, "", "ms", sample_rate);
}

// Increment
void Statsd::increment(String metric, String tags, float sample_rate) {
    count(metric, 1, tags, sample_rate);
}

void Statsd::increment(String metric) {
    count(metric, 1, "", 1.0);
}

void Statsd::increment(String metric, float sample_rate) {
    count(metric, 1, "", sample_rate);
}

// Decrement
void Statsd::decrement(String metric, String tags, float sample_rate) {
    count(metric, -1, tags, sample_rate);
}

void Statsd::decrement(String metric) {
    count(metric, -1, "", 1.0);
}

void Statsd::decrement(String metric, float sample_rate) {
    count(metric, -1, "", sample_rate);
}
