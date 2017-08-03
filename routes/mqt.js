var mqtt = require('mqtt');
var options = {
    port: 10513,
    host: 'mqtt://m12.cloudmqtt.com',
    clientId: 'mqttjs_' + Math.random().toString(16).substr(2, 8),
    username: 'xxx',
    password: 'xx',
    keepalive: 60,
    reconnectPeriod: 1000,
    protocolId: 'MQIsdp',
    protocolVersion: 3,
    clean: true,
    encoding: 'utf8'
};


module.exports = {

    startAndPush: function (message) {

        var client = mqtt.connect('mqtt://m12.cloudmqtt.com', options);
        client.on('connect', function() { // When connected
            console.log('connected');
            // subscribe to a topic
            client.subscribe('/topic/accounts', function() {
                // when a message arrives, do something with it
                client.on('message', function(topic, message, packet) {
                    console.log("Received '" + message + "' on '" + topic + "'");
                });
            });

            // publish a message to a topic
            client.publish('/topic/accounts', message, function() {
                console.log("Message is published");
                //client.end(); // Close the connection when published
            });
        });

    }

}