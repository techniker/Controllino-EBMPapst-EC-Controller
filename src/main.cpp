//<tec aett sixtopia.net>
//Simple Controllio based EBMPapst EC fan controller
//with integrated Web-Server and MQTT Client

#include <SPI.h>
#include <Ethernet.h>
#include <Controllino.h>
#include <PubSubClient.h>


byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(10, 100, 0, 177);
EthernetServer server(80);

// MQTT Broker settings
const char* mqttServer = "10.22.5.5";
const int mqttPort = 1883;
const char* mqttUser = "mqtt";  // Replace with your MQTT username
const char* mqttPassword = "k-u9xedDZoizdMRh";  // Replace with your MQTT password

const int ledPin = CONTROLLINO_D0;
int lastPwmValue = 0;

EthernetClient ethClient;
PubSubClient mqttClient(ethClient);

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("MQTT Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);

  // Process the mqtt message
  if (String(topic) == "fan01") {
    int pwmValue = message.toInt();
    pwmValue = constrain(pwmValue, 0, 100);
    analogWrite(ledPin, map(pwmValue, 0, 100, 0, 255));
    lastPwmValue = pwmValue;
    Serial.println("PWM value set via MQTT: " + String(pwmValue));
  }
}

void connectToMqtt() {
  while (!mqttClient.connected()) {
    Serial.println("Connecting to MQTT Broker...");
    if (mqttClient.connect("ControllinoClient", mqttUser, mqttPassword)) {
      Serial.println("MQTT connected!");
      mqttClient.subscribe("fan01");
    } else {
      Serial.print("MQTT connection failed, rc=");
      Serial.print(mqttClient.state());
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  analogWrite(ledPin, 0);
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("Web-Server is online at ");
  Serial.println(Ethernet.localIP());
  // Setup MQTT Client
  mqttClient.setServer(mqttServer, mqttPort);
  mqttClient.setCallback(mqttCallback);
  connectToMqtt();
}

void loop() {
  EthernetClient client = server.available();
  if (client) {
    String request = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        request += c;
        if (c == '\n' && request.endsWith("\r\n")) {
          delay(10);
          while (client.available()) {
            request += (char)client.read();
          }
          if (request.indexOf("POST /set_pwm") != -1) {
            int pwmStart = request.indexOf("pwm=") + 4;
            int pwmEnd = request.indexOf("&", pwmStart);
            if (pwmEnd == -1) {
              pwmEnd = request.length();
            }
            String pwmValueStr = request.substring(pwmStart, pwmEnd);
            int pwmValue = pwmValueStr.toInt();
            pwmValue = constrain(pwmValue, 0, 100);
            analogWrite(ledPin, map(pwmValue, 0, 100, 0, 255));
            Serial.print("PWM value set to: ");
            Serial.println(pwmValue);
            lastPwmValue = pwmValue;
          }
          client.println("HTTP/1.1 200 OK");
          client.println("Content-type:text/html");
          client.println("Connection: close");
          client.println();
          client.println("<html><head>");
          client.println("<meta name='viewport' content='width=device-width, initial-scale=1.0'>");
          client.println("<style id='themeStyle'>");
          client.println("body { font-family: Arial, sans-serif; padding: 20px; background-color: #fff; color: #000; }");
          client.println("input[type=number], input[type=range], input[type=submit], input[type=button] { width: 100%; margin-bottom: 10px; padding: 10px; box-sizing: border-box; }");
          client.println("h1, h2 { font-size: 1.5em; }");
          client.println("</style>");
          client.println("</head><body onload='loadTheme()'>");
          client.println("<h1>Fan Speed Controller</h1>");
          client.println("<h4>FAN-01</h4>");
          client.println("<button onclick='toggleDarkMode()'>Toggle Dark Mode</button>");
          client.println("<form method='POST' action='/set_pwm'>");
          client.println("Set PWM value (0-100): <input type='number' name='pwm' min='0' max='100' id='pwmInput' oninput='updateSlider(this.value)' value='" + String(lastPwmValue) + "'><br>");
          client.println("Slider: <input type='range' name='pwmSlider' min='0' max='100' id='pwmSlider' oninput='updateInput(this.value)' value='" + String(lastPwmValue) + "'><br>");
          client.println("<input type='submit' value='Set PWM'>");
          client.println("</form>");
          client.println("<h2>Quick Set</h2>");
          int fixedValues[] = {0, 3, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 60, 70, 80, 100};
          for(int i = 0; i < 16; i++) {
            client.println("<input type='button' value='Set PWM to " + String(fixedValues[i]) + "' onclick='setPwmValue(" + String(fixedValues[i]) + ")'><br>");
          }
          client.println("<script>");
          client.println("function updateInput(val) { document.getElementById('pwmInput').value = val; }");
          client.println("function updateSlider(val) { document.getElementById('pwmSlider').value = val; }");
          client.println("function setPwmValue(val) { document.getElementById('pwmInput').value = val; document.getElementById('pwmSlider').value = val; document.forms[0].submit(); }");
          client.println("function toggleDarkMode() {");
          client.println("  var style = document.getElementById('themeStyle');");
          client.println("  if (style.innerHTML.includes('background-color: #fff')) {");
          client.println("    style.innerHTML = 'body { font-family: Arial, sans-serif; padding: 20px; background-color: #000; color: #fff; } input[type=number], input[type=range], input[type=submit], input[type=button] { width: 100%; margin-bottom: 10px; padding: 10px; box-sizing: border-box; } h1, h2 { font-size: 1.5em; }';");
          client.println("    localStorage.setItem('theme', 'dark');");
          client.println("  } else {");
          client.println("    style.innerHTML = 'body { font-family: Arial, sans-serif; padding: 20px; background-color: #fff; color: #000; } input[type=number], input[type=range], input[type=submit], input[type=button] { width: 100%; margin-bottom: 10px; padding: 10px; box-sizing: border-box; } h1, h2 { font-size: 1.5em; }';");
          client.println("    localStorage.setItem('theme', 'light');");
          client.println("  }");
          client.println("}");
          client.println("function loadTheme() {");
          client.println("  var theme = localStorage.getItem('theme');");
          client.println("  var style = document.getElementById('themeStyle');");
          client.println("  if (theme === 'dark') {");
          client.println("    style.innerHTML = 'body { font-family: Arial, sans-serif; padding: 20px; background-color: #000; color: #fff; } input[type=number], input[type=range], input[type=submit], input[type=button] { width: 100%; margin-bottom: 10px; padding: 10px; box-sizing: border-box; } h1, h2 { font-size: 1.5em; }';");
          client.println("  }");
          client.println("}");
          client.println("</script>");
          client.println("</body></html>");
          client.stop();
          break;
        }
      }
    }
    delay(1);
    client.stop();
    Serial.println("Client disconnected");
  }
  // MQTT client loop
  if (!mqttClient.connected()) {
    connectToMqtt();
  }
  mqttClient.loop();
}