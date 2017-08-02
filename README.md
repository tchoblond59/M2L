# M2L

M2L stand for MQTT To [Larahome](https://github.com/tchoblond59/LaraHome)

Basically this is a gateway between MQTT and a Website. M2L subscribe to a specific topic on the MQTT broker then send a POST Request to a given url.

The topic must match the [MySensors](https://github.com/mysensors/MySensors) specification.

There is a config file in `/home/username/.config/MySensors/M2L.conf` 

    [Larahome]
    url=http://larahome.dev/api/
    api_id=0123456789
    api_key=ABCDEFGHIJLMKM
    
    [MQTT]
    ip=192.168.0.1
    port=1883
    client_id=M2L
    channel=mysensors-out/#
    user=user
    password=password

This program is write in C++ with Qt. M2L uses [emqqt](https://github.com/emqtt/qmqtt) and [SimpleQtLogger](https://github.com/Mokolea/SimpleQtLogger) library.
