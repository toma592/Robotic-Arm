#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>
#include <ESP8266WebServer.h>
#include <Servo.h>

// Configuration WiFi
const char* ssid = "nameofWiFi";
const char* password = "password";

// Creating servos
Servo servo1, servo2, servo3, servo4, servo5, servo6;

// Configuration WebSockets and HTTP server
WebSocketsServer webSocket(81);
ESP8266WebServer server(80);

// PIN to LED (D4, GPIO2)
const int ledPin = 2; // GPIO2, pin D4 na ESP8266

// Time of flashing LED
unsigned long previousMillis = 0;
const long intervalConnected = 3000; // 3 seconds
const long intervalDisconnected = 500; //  0,5 second
bool ledState = LOW; // LED status

void setup() {
  Serial.begin(115200);
  Serial.println("Starting setup...");

  // Connecting with WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Servos configuration
  servo1.attach(5, 544, 2400);  // D1 to GPIO5 WAZNA JEST KALIBRACJA POZYCJI
  servo2.attach(4, 544, 2400);  // D2 to GPIO4 WAZNA JEST KALIBRACJA POZYCJI
  servo3.attach(0, 544, 2400);  // D3 to GPIO0 WAZNA JEST KALIBRACJA POZYCJI
  servo4.attach(2, 544, 2400);  // D4 to GPIO2 WAZNA JEST KALIBRACJA POZYCJI
  servo5.attach(14, 544, 2400); // D5 to GPIO14 WAZNA JEST KALIBRACJA POZYCJI
  servo6.attach(12, 544, 2400); // D6 to GPIO12 WAZNA JEST KALIBRACJA POZYCJI

  // Start WebSocket server
  webSocket.begin();
  webSocket.onEvent(handleWebSocketEvent);

  // Start HTTP server
  server.on("/", handleRoot);
  server.begin();

  Serial.println("WebSocket server started!");
  Serial.println("HTTP server started!");

  // Konfiguracja pinu diody LED
  pinMode(ledPin, OUTPUT); // Ustaw pin D4 jako wyjście
  Serial.println("Setup completed!");
}

void loop() {
  webSocket.loop();
  server.handleClient();

  // Sprawdź status połączenia Wi-Fi
  bool connected = (WiFi.status() == WL_CONNECTED);
  
  // Oblicz aktualny czas
  unsigned long currentMillis = millis();
  
  // Ustal interwał migania diody na podstawie statusu połączenia
  long interval = connected ? intervalConnected : intervalDisconnected;
  
  // Miganie diody co ustalony interwał
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    
    // Zmień stan diody
    ledState = (ledState == LOW) ? HIGH : LOW;
    digitalWrite(ledPin, ledState);
    Serial.print("LED State: ");
    Serial.println(ledState == HIGH ? "ON" : "OFF");
  }
}

// Funkcja do obsługi WebSocketów
void handleWebSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  if (type == WStype_CONNECTED) {
    Serial.printf("Client %u connected\n", num);
    webSocket.sendTXT(num, "Welcome to WebSocket Server");
  } else if (type == WStype_DISCONNECTED) {
    Serial.printf("Client %u disconnected\n", num);
  } else if (type == WStype_TEXT) {
    String message = String((char *)payload).substring(0, length);
    Serial.printf("Received message from client %u: %s\n", num, message.c_str());
    if (message.startsWith("P")) {
      int servoNumber = message.substring(1, 2).toInt();
      int position = message.substring(3).toInt();
      Serial.printf("Moving servo %d to position %d\n", servoNumber, position);

      switch (servoNumber) {
        case 1: servo1.write(position); break;
        case 2: servo2.write(position); break;
        case 3: servo3.write(position); break;
        case 4: servo4.write(position); break;
        case 5: servo5.write(position); break;
        case 6: servo6.write(position); break;
        default: Serial.println("Invalid servo number"); break;
      }
    } else if (message == "START") {
      Serial.println("Test started");
    } else if (message == "STOP") {
      Serial.println("Test stopped");
    } else {
      Serial.println("Invalid message format");
    }
  } else if (type == WStype_ERROR) {
    Serial.println("WebSocket error");
  }
}

// Funkcja do obsługi żądania HTTP
void handleRoot() {
  server.send(200, "text/html", R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Robotic Arm</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #061532;
            margin: 0;
            padding: 20px;
        }

        h1 {
            text-align: center;
            color: #ffffff;
        }

        .container {
            max-width: 700px;
            margin: 0 auto;
            background: #eef9ff;
            padding: 20px;
            border-radius: 8px;
            box-shadow: 0 0 10px #06153284;
            text-align: center;
        }

        label {
            display: block;
            text-align: left;
            font-size: 16px;
            font-weight: bold;
            color: #333;
            margin: 10px;
        }

        input[type="range"] {
            -webkit-appearance: none;
            appearance: none;
            width: 100%;
            cursor: pointer;
            outline: none;
            border-radius: 15px;
            height: 6px;
            background: #cccccc;
            margin-top: 5px;
        }
    
        input[type="range"]::-webkit-slider-thumb {
            -webkit-appearance: none;
            appearance: none;
            height: 30px;
            width: 30px;
            background-image: url("https://e7.pngegg.com/pngimages/565/538/png-clipart-gear-computer-icons-cog-copyright-cogwheel-thumbnail.png");
            background-size: cover;
            border-radius: 50%;
            border: none;
            transition: .2s ease-in-out;
            transform: rotateZ(var(--thumb-rotate, 0deg));
        }
    
        input[type="range"]::-moz-range-thumb {
            height: 30px;
            width: 30px;
            background-image: url("https://e7.pngegg.com/pngimages/565/538/png-clipart-gear-computer-icons-cog-copyright-cogwheel-thumbnail.png");
            background-size: cover;
            border-radius: 50%;
            border: none;
            transition: .2s ease-in-out;
            transform: rotateZ(var(--thumb-rotate, 0deg));
        }

        input[type="range"]::-webkit-slider-thumb:hover {
            box-shadow: 0 0 0 13px rgba(134, 57, 19, 0.1);
        }
    
        input[type="range"]:active::-webkit-slider-thumb {
            box-shadow: 0 0 0 13px rgba(134, 57, 19, 0.1);
        }
    
        input[type="range"]:focus::-webkit-slider-thumb {
            box-shadow: 0 0 0 13px rgba(134, 57, 19, 0.1);
        }
    
        input[type="range"]::-moz-range-thumb:hover {
            box-shadow: 0 0 0 13px rgba(134, 57, 19, 0.1);
        }
    
        input[type="range"]:active::-moz-range-thumb {
            box-shadow: 0 0 0 13px rgba(134, 57, 19, 0.1);
        }
    
        input[type="range"]:focus::-moz-range-thumb {
            box-shadow: 0 0 0 13px rgba(134, 57, 19, 0.1);
        }

        span {
            display: block;
            text-align: left;
            font-size: 16px;
            font-weight: bold;
            color: #333;
            margin-top: 10px;
            margin-left: 10px;
        }

        button {
            font-weight: bold;
            width: 100%;
            padding: 10px;
            margin: 10px 0;
            font-size: 16px;
            background-color: #E1CBB4;
            color: #333;
            border: none;
            border-radius: 5px;
            cursor: pointer;
            transition: background-color .2s, color .2s;
        }

        button.red {
            background-color: #E74C3C;
            color: #fff;
        }

        button:hover {
            background-color: #c0a780;
            color: #fff;
        }

        button.red:hover {
            background-color: #c0392b;
        }

        #instruction, #instructionText {
            font-weight: bold;
            color: #333;
            display: inline-block;
            margin-bottom: 3px;
        }

        #instruction {
            font-size: 25px;
        }

        #instructionText {
            text-align: center;
            font-size: 15px;
        }

        @media (max-width: 600px) {
            .container {
                padding: 10px;
            }

            h1 {
                font-size: 24px;
            }

            label {
                font-size: 16px;
            }

            button {
                font-size: 14px;
                padding: 8px;
            }
        }
    </style>
</head>
<body>
    <h1>Robotic Arm Operation Panel</h1>
    <div class="container"> 

        <button id="connectButton">Connect with ESP8266</button>

        <!-- New Button -->
        <button id="homePositionButton">Go to the home position</button>

        <div>
            <label id="instruction">Manual & description</label>
            <label id="instructionText">
                <br>Sliders below are responsible for controlling every axis of the robotic arm
                <br>The first axis is responsible for rotating full arm in 180°
                <br>Each subsequent axis is identified from bottom to top
                <br>The last, sixth axis is responsible for the movement of the gripper
            </label>
        </div>
    
        <!-- Servo 1 -->
        <div>
            <label for="servoSlider1">AXIS 1:</label>
            <input type="range" id="servoSlider1" min="0" max="180" value="90" oninput="updateServoPosition(1)">
            <span id="servoValue1">90°</span>
        </div>
    
        <!-- Servo 2 -->
        <div>
            <label for="servoSlider2">AXIS 2:</label>
            <input type="range" id="servoSlider2" min="20" max="130" value="40" oninput="updateServoPosition(2)">
            <span id="servoValue2">40°</span>
        </div>
    
        <!-- Servo 3 -->
        <div>
            <label for="servoSlider3">AXIS 3:</label>
            <input type="range" id="servoSlider3" min="20" max="180" value="40" oninput="updateServoPosition(3)">
            <span id="servoValue3">40°</span>
        </div>
    
        <!-- Servo 4 -->
        <div>
            <label for="servoSlider4">AXIS 4:</label>
            <input type="range" id="servoSlider4" min="0" max="140" value="130" oninput="updateServoPosition(4)">
            <span id="servoValue4">130°</span>
        </div>
    
        <!-- Servo 5 -->
        <div>
            <label for="servoSlider5">AXIS 5:</label>
            <input type="range" id="servoSlider5" min="0" max="180" value="95" oninput="updateServoPosition(5)">
            <span id="servoValue5">95°</span>
        </div>
    
        <!-- Servo 6 -->
        <div>
            <label for="servoSlider6">AXIS 6:</label>
            <input type="range" id="servoSlider6" min="65" max="150" value="100" oninput="updateServoPosition(6)">
            <span id="servoValue6">100°</span>
        </div>
    </div>
    
    <script>
        let socket;
        let isConnected = false;
        let hasPendingConnectionRequest = false;

        function connectToServer() {
            if (isConnected) {
                // If already connected, close the existing connection
                socket.close();
                isConnected = false;
                console.log('Disconnected from WebSocket server');
                document.getElementById('connectButton').textContent = 'Connect with ESP8266';
                document.getElementById('connectButton').classList.remove('red');
                return;
            }

            if (hasPendingConnectionRequest) {
                console.log('Connection request is already pending. Please wait.');
                return;
            }

            hasPendingConnectionRequest = true;

            // Establish a new connection
            socket = new WebSocket('ws://192.168.82.24:81');

            socket.onopen = () => {
                console.log('Connected to WebSocket server');
                isConnected = true;
                hasPendingConnectionRequest = false;
                document.getElementById('connectButton').textContent = 'Disconnect with ESP8266';
                document.getElementById('connectButton').classList.add('red');
            };

            socket.onmessage = event => console.log('Received message:', event.data);

            socket.onerror = error => {
                console.error('WebSocket error:', error);
                hasPendingConnectionRequest = false;
            };

            socket.onclose = () => {
                console.log('WebSocket connection closed');
                isConnected = false;
                document.getElementById('connectButton').textContent = 'Connect with ESP8266';
                document.getElementById('connectButton').classList.remove('red');
                hasPendingConnectionRequest = false;
            };
        }

        function updateServoPosition(servoNumber) {
          const slider = document.getElementById(`servoSlider${servoNumber}`);
          const output = document.getElementById(`servoValue${servoNumber}`);
          const position = slider.value;
          output.textContent = position + '°';

          const minRange = slider.min;
          const maxRange = slider.max;
          
          //Formula for minimal ammount for slider
          const progress = ((position - minRange) / (maxRange - minRange)) * 100;
          
          const thumbRotate = (position / maxRange) * 2160;

          slider.style.background = `linear-gradient(to right, #E1CBB4 ${progress}%, #ccc ${progress}%)`;
          slider.style.setProperty("--thumb-rotate", `${thumbRotate}deg`);

          if (socket && socket.readyState === WebSocket.OPEN) {
              socket.send(`P${servoNumber}:${position}`);
              console.log(`Servo position sent ${servoNumber}: ${position}`);
              } else {
                  console.log('Failure while connecting with ESP8266');
              }
        }

        // Function to set all servos to their home position
        function setHomePosition() {
            const homePositions = [90, 40, 40, 130, 95, 100];
            for (let i = 1; i <= 6; i++) {
                document.getElementById(`servoSlider${i}`).value = homePositions[i-1];
                updateServoPosition(i);
            }
        }

        document.getElementById('connectButton').addEventListener('click', connectToServer);
        document.getElementById('homePositionButton').addEventListener('click', setHomePosition);
    </script>
</body>
</html>

)rawliteral");
}