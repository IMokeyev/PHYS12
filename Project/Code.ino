#include <AccelStepper.h>
#include <WiFi.h>
#include <WebServer.h>

#define WIFI_SSID "Baybak"
#define WIFI_PASSWORD "maO9O329"

#define NEMA8_CONFIG 1
#define STEPPER28BYJ_CONFIG 8

#define NEMA8_STEP_PIN_1 14
#define NEMA8_DIR_PIN_1 27
#define NEMA8_STEP_PIN_2 26
#define NEMA8_DIR_PIN_2 25
#define NEMA8_STEP_PIN_3 33
#define NEMA8_DIR_PIN_3 32
#define NEMA8_STEP_PIN_4 35
#define NEMA8_DIR_PIN_4 34

#define STEPPER28BYJ_PIN1 19
#define STEPPER28BYJ_PIN2 21
#define STEPPER28BYJ_PIN3 22
#define STEPPER28BYJ_PIN4 23

AccelStepper nema8Stepper1(NEMA8_CONFIG, NEMA8_STEP_PIN_1, NEMA8_DIR_PIN_1);
AccelStepper nema8Stepper2(NEMA8_CONFIG, NEMA8_STEP_PIN_2, NEMA8_DIR_PIN_2);
AccelStepper nema8Stepper3(NEMA8_CONFIG, NEMA8_STEP_PIN_3, NEMA8_DIR_PIN_3);
AccelStepper nema8Stepper4(NEMA8_CONFIG, NEMA8_STEP_PIN_4, NEMA8_DIR_PIN_4);

AccelStepper stepper28BYJ(STEPPER28BYJ_CONFIG, STEPPER28BYJ_PIN1, STEPPER28BYJ_PIN3, STEPPER28BYJ_PIN2, STEPPER28BYJ_PIN4);

struct Pose {
  long nema8Pos1;
  long nema8Pos2;
  long nema8Pos3;
  long nema8Pos4;
  long stepper28BYJPos;
};

Pose pose1 = {1000, 2000, 3000, 4000, 5120};
Pose pose2 = {500, 1500, 2500, 3500, 2560};

WebServer server(80);

void moveToPose(Pose targetPose) {
  nema8Stepper1.moveTo(targetPose.nema8Pos1);
  nema8Stepper2.moveTo(targetPose.nema8Pos2);
  nema8Stepper3.moveTo(targetPose.nema8Pos3);
  nema8Stepper4.moveTo(targetPose.nema8Pos4);
  stepper28BYJ.moveTo(targetPose.stepper28BYJPos);
}

void runToPose(Pose targetPose) {
  moveToPose(targetPose);
  while (nema8Stepper1.isRunning() || nema8Stepper2.isRunning() || nema8Stepper3.isRunning() || nema8Stepper4.isRunning() || stepper28BYJ.isRunning()) {
    nema8Stepper1.run();
    nema8Stepper2.run();
    nema8Stepper3.run();
    nema8Stepper4.run();
    stepper28BYJ.run();
  }
}

void handleRoot() {
  String html = "<html><body>";
  html += "<h1>Stepper Motor Control</h1>";
  html += "<p>Select a pose to run:</p>";
  html += "<form method='post' action='/runPose'>";
  html += "<button type='submit' name='pose' value='1'>Pose 1</button>";
  html += "<button type='submit' name='pose' value='2'>Pose 2</button>";
  html += "</form>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void handleRunPose() {
  if (server.hasArg("pose")) {
    int poseNumber = server.arg("pose").toInt();
    if (poseNumber == 1) {
      runToPose(pose1);
    } else if (poseNumber == 2) {
      runToPose(pose2);
    }
  }
  server.send(200, "text/plain", "Running pose...");
}

void setup() {
  Serial.begin(115200);
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  
  Serial.println("Connected to WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Set pin modes for Nema 8 stepper control pins
  pinMode(NEMA8_STEP_PIN_1, OUTPUT);
  pinMode(NEMA8_DIR_PIN_1, OUTPUT);
  pinMode(NEMA8_STEP_PIN_2, OUTPUT);
  pinMode(NEMA8_DIR_PIN_2, OUTPUT);
  pinMode(NEMA8_STEP_PIN_3, OUTPUT);
  pinMode(NEMA8_DIR_PIN_3, OUTPUT);
  pinMode(NEMA8_STEP_PIN_4, OUTPUT);
  pinMode(NEMA8_DIR_PIN_4, OUTPUT);

  // Set pin modes for 28BYJ-48 stepper control pins
  pinMode(STEPPER28BYJ_PIN1, OUTPUT);
  pinMode(STEPPER28BYJ_PIN2, OUTPUT);
  pinMode(STEPPER28BYJ_PIN3, OUTPUT);
  pinMode(STEPPER28BYJ_PIN4, OUTPUT);


  server.on("/", handleRoot);
  server.on("/runPose", handleRunPose);
  server.begin();
  
  nema8Stepper1.setMaxSpeed(1000);
  nema8Stepper2.setMaxSpeed(1000);
  nema8Stepper3.setMaxSpeed(1000);
  nema8Stepper4.setMaxSpeed(1000);
  stepper28BYJ.setMaxSpeed(500);
  
  nema8Stepper1.setAcceleration(500);
  nema8Stepper2.setAcceleration(500);
  nema8Stepper3.setAcceleration(500);
  nema8Stepper4.setAcceleration(500);
  stepper28BYJ.setAcceleration(200);
}

void loop() {
  server.handleClient();
}
