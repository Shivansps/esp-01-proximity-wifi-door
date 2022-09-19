
ESP8266/ESP-01 Proximity based WIFI Door
This code is based on kalanda ESP8266 Wifi sniffer https://github.com/kalanda/esp8266-sniffer

The idea is to sniff all packets in a specific channel, since you already know the channel of your AP,
so you know where to look. Then check for the RSSI values, a RSSI value of -30 means that the cellphone
is closer to the ESP8266 than a value of -50. This is were the TARGET_RSSI come sinto play, we set a value
and anything higher it is considered to be in range.
Once in range the code will compare the device MAC with string containing all allowed macs, if a match is found
then the relay is enabled, opening, for example, a magnetic lock.

Allowed MAC list is downloaded from a remote web server, for this the ESP will first connect to AP, download the MACs then switch to scan mode.

Cellphones does not always send wifi packages, specially if they are already connected to an AP and the cellphone is locked.
In android i found that unlocking the phone is enoght for wifi to start transmitting, so unlocking the phone close enoght to the
ESP should be enoght to open the door. Failing that opening whatsapp or any app that uses internet should do it.

This code will reset the ESP once an hour to re-download the list of macs. This is the RESET_INTERVAL value, a value of 0 disables the reset.On the ESP-01 TX PIN and built Led pin is the same (GPIO1). Serial output must be disabled if you assign 1 as LED_GPIO. On ESP-01S led is wired to GPIO2.

Keep in mind this is not perfect, and some times it may take a little bit of time to detect the phone. And since the ESP-01 is 2.4Ghz only the phone would have to be connected to 2.4Ghz wifi.

The php file is is running on a internal web server, my router has a script that every time something connects to the wifi and a IP lease is given it calls this same php web file to store the mac into a database.
