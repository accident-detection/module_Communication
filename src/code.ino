#include <EtherCard.h>

// ethernet interface ip address
static byte myip[] = { 172,16,2,2 };
static byte gwip[] = { 172,16,2,1 };
static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };

byte Ethernet::buffer[500];

void setup()
{
    Serial.begin(9600);

    // while the serial stream is not open, do nothing:
    while (!Serial) ;

    if (ether.begin(sizeof Ethernet::buffer, mymac) == 0)
        Serial.println( "Failed to access Ethernet controller");

    ether.staticSetup(myip, gwip);

    ether.printIp("IP:  ", ether.myip);
    ether.printIp("GW:  ", ether.gwip);
    ether.printIp("DNS: ", ether.dnsip);
}

void loop()
{
    // Send request to http://log.dokku.d.h/api/events
}
