#include <EtherCard.h>
#include <dht.h>
#define DHT11_PIN 5
#define DEBUG true

static uint32_t timer;
dht DHT;

static byte session;
Stash stash;
byte Ethernet::buffer[700];

static byte myip[] = { 172,16,2,2 };
static byte gwip[] = { 172,16,2,1 };
static byte dnsip[] = { 172,16,0,3 };
static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };
const char website[] PROGMEM = "adb.dokku.d.h";

static void my_callback(byte status, word off, word len)
{
    Serial.println(">>>");
    Ethernet::buffer[off+300] = 0;
    Serial.print((const char*) Ethernet::buffer + off);
    Serial.println("...");
}

static void sendData() {
    Serial.println("Sending data...");

    byte sd = stash.create();

    stash.print("{\"GPSlat\": null, \"GPSlog\": null, \"GPSalt\": null, \"temp\": ");
    stash.print(readTemp());
    stash.print(", \"errorCode\": null }");
    stash.save();
    int stash_size = stash.size();

    // Compose the http POST request, taking the headers below and appending
    // previously created stash in the sd holder.
    Stash::prepare(PSTR("POST http://$F/api/events HTTP/1.0" "\r\n"
    "Host: $F" "\r\n"
    "Content-Length: $D" "\r\n"
    "Content-Type: application/json" "\r\n"
    "x-adb-token: REDACTED" "\r\n"
    "\r\n"
    "$H"),
    website, website, stash_size, sd);

    // send the packet - this also releases all stash buffers once done
    // Save the session ID so we can watch for it in the main loop.
    session = ether.tcpSend();
}

String readTemp()
{
    DHT.read11(DHT11_PIN);
    return String(DHT.temperature, DEC);
}

void setup()
{
    Serial.begin(9600);

    if (DEBUG)
        while (!Serial) ;

    if (ether.begin(sizeof Ethernet::buffer, mymac) == 0)
        Serial.println(F("Failed to access Ethernet controller"));

    ether.staticSetup(myip, gwip, dnsip);

    ether.printIp("IP:", ether.myip);
    ether.printIp("GW:", ether.gwip);
    ether.printIp("DNS:", ether.dnsip);

    if (!ether.dnsLookup(website))
        Serial.println("DNS failed");

    ether.printIp("SRV:", ether.hisip);
    sendData();
}

void loop()
{
    ether.packetLoop(ether.packetReceive());

    const char* reply = ether.tcpReply(session);
    if (reply != 0) {
        Serial.println("Got a response!");
        Serial.println(reply);
    }
}
