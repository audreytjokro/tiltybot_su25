#include <Arduino.h>
#include <tuple>

#include "init.h"
#include "network.h"

// // Includes for the server
#include <HTTPSServer.hpp>
#include <HTTPRequest.hpp>
#include <HTTPResponse.hpp>
#include <util.hpp>
#include <WebsocketHandler.hpp>

#define INDEX_PAGE "/voice.html"

// // Robot setup
#include "XL330.h"

XL330 robot;
int prevM1 = 0;
int prevM2 = 0;

using namespace httpsserver;

HTTPSServer *secureServer;

// class for robot Controler
class ControlHandler : public WebsocketHandler
{
public:
    static WebsocketHandler *create();
    // This method is called when a message arrives
    void onMessage(WebsocketInputStreambuf *input);
};

void setup()
{
    // For logging
    Serial.begin(115200);
    Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);

    SSLCert *cert = initLittleFS();

    // Connect to WiFi
    initWiFi(ssid, password, INDEX_PAGE, AP);

    // Create the server with the certificate we loaded before
    secureServer = initServer(cert);

    // create ws node
    WebsocketNode *controlNode = new WebsocketNode("/ws", &ControlHandler::create);

    // Adding the node to the server works in the same way as for all other nodes
    secureServer->registerNode(controlNode);

    // Add HTTP GET handler for moving the motors
    ResourceNode *moveNode = new ResourceNode("/move", "GET", [](HTTPRequest *req, HTTPResponse *res) {
        Serial.println("Received HTTP GET /move");
        // Get the 'dir' parameter from the request
        String uri = String(req->getRequestString().c_str());
        String dir = "";
        int idx = uri.indexOf("dir=");
        if (idx >= 0) {
            int endIdx = uri.indexOf("&", idx);
            if (endIdx == -1) endIdx = uri.length();
            dir = uri.substring(idx + 4, endIdx);
        }
        if (!dir.isEmpty()) {
            int posDelta = 600;
            int steps = 1;

            int stepsIdx = uri.indexOf("steps=");
            if (stepsIdx >= 0) {
                int stepsEndIdx = uri.indexOf("&", stepsIdx);
                if (stepsEndIdx == -1) stepsEndIdx = uri.length();
                steps = uri.substring(stepsIdx + 6, stepsEndIdx).toInt();
                if (steps <= 0) steps = 1;
            }

            int moveDelta = posDelta * steps;
            if (uri.indexOf("move") >= 0 && uri.indexOf("steps=") == -1) {
                steps = 1;
                moveDelta = posDelta;
            }

            Serial.print("Direction: ");
            Serial.println(dir);
            Serial.print("Steps: ");
            Serial.println(steps);
            Serial.print("Delta: ");
            Serial.println(moveDelta);

            int speed = 200;      // how fast they spin
            int duration = 600 * steps; // how long they spin. double to move further every time we speak

            if (
                dir == "forward" ||
                dir == "go" ||
                dir == "goforward" ||
                dir == "go_forward" ||
                dir == "go front" ||
                dir == "go_front" ||
                dir == "front"
            ) {
                robot.setJointSpeed(MOTOR1, speed);
                robot.setJointSpeed(MOTOR2, speed);
                delay(duration);
                robot.setJointSpeed(MOTOR1, 0);
                robot.setJointSpeed(MOTOR2, 0);
                Serial.println("Received command: forward");
            } else if (
                dir == "backward" ||
                dir == "gobackward" ||
                dir == "go back" ||
                dir == "go_back" ||
                dir == "back"
            ) {
                robot.setJointSpeed(MOTOR1, -speed);
                robot.setJointSpeed(MOTOR2, -speed);
                delay(duration);
                robot.setJointSpeed(MOTOR1, 0);
                robot.setJointSpeed(MOTOR2, 0);
                Serial.println("Received command: backward");
            } else if (
                dir == "left" ||
                dir == "go left" ||
                dir == "go_left"
            ) {
                robot.setJointSpeed(MOTOR1, -speed);
                robot.setJointSpeed(MOTOR2, speed);
                delay(duration);
                robot.setJointSpeed(MOTOR1, 0);
                robot.setJointSpeed(MOTOR2, 0);
                Serial.println("Received command: left");
            } else if (
                dir == "right" ||
                dir == "go right" ||
                dir == "go_right"
            ) {
                robot.setJointSpeed(MOTOR1, speed);
                robot.setJointSpeed(MOTOR2, -speed);
                delay(duration);
                robot.setJointSpeed(MOTOR1, 0);
                robot.setJointSpeed(MOTOR2, 0);
                Serial.println("Received command: right");
            } else if (
                dir == "turnaround" ||
                dir == "turn_around" ||
                dir == "turn around" ||
                dir == "go around left"
            ) {
                // Step 1: forward
                robot.setJointSpeed(MOTOR1, speed);
                robot.setJointSpeed(MOTOR2, speed);
                delay(duration);

                // Step 2: left
                robot.setJointSpeed(MOTOR1, -speed);
                robot.setJointSpeed(MOTOR2, speed);
                delay(duration);

                // Step 3: left again
                robot.setJointSpeed(MOTOR1, -speed);
                robot.setJointSpeed(MOTOR2, speed);
                delay(duration);

                // Step 4: forward again
                robot.setJointSpeed(MOTOR1, speed);
                robot.setJointSpeed(MOTOR2, speed);
                delay(duration);

                // Stop
                robot.setJointSpeed(MOTOR1, 0);
                robot.setJointSpeed(MOTOR2, 0);

                Serial.println("Received command: turnaround");
            }
            else if (dir == "custom") {
                int m1 = 90;  // default stop
                int m2 = 90;
                if (uri.indexOf("m1=") >= 0) {
                    int m1Start = uri.indexOf("m1=") + 3;
                    int m1End = uri.indexOf("&", m1Start);
                    if (m1End == -1) m1End = uri.length();
                    m1 = uri.substring(m1Start, m1End).toInt();
                }
                if (uri.indexOf("m2=") >= 0) {
                    int m2Start = uri.indexOf("m2=") + 3;
                    int m2End = uri.indexOf("&", m2Start);
                    if (m2End == -1) m2End = uri.length();
                    m2 = uri.substring(m2Start, m2End).toInt();
                }

                Serial.printf("🛠️ Custom Motor Control: M1=%d, M2=%d\n", m1, m2);

                robot.setJointSpeed(MOTOR1, m1);
                robot.setJointSpeed(MOTOR2, m2);
                delay(500);  // can adjust this
                robot.setJointSpeed(MOTOR1, 0);
                robot.setJointSpeed(MOTOR2, 0);
            }
            res->setStatusCode(200);
            res->println("OK");
            res->flush();
        } else {
            res->setStatusCode(400);
            res->println("Missing dir parameter.");
            res->flush();
        } 
    });
    secureServer->registerNode(moveNode);

    Serial.println("Starting server...");
    secureServer->start();
    if (secureServer->isRunning())
    {
        Serial.println("Server ready.");
    }
    initRobot(Serial2, robot, DRIVE_MODE);
    
}

void loop()
{
    // This call will let the server do its work
    secureServer->loop();

    delay(5);
}

WebsocketHandler *ControlHandler::create()
{
    Serial.println("Creating new control client!");
    ControlHandler *handler = new ControlHandler();
    return handler;
}

void ControlHandler::onMessage(WebsocketInputStreambuf *inbuf)
{
    std::tuple<int, int> result = parseData(inbuf);
    int xRaw = std::get<0>(result);
    int yRaw = std::get<1>(result);
    int x = map(xRaw, -100, 100, -300, 300); // adjusted from 885 to 300 for x to cap the speed range
    int y = map(yRaw, -100, 100, -300, 300);
    int speedM1 = constrain(x + y, -300, 300);
    int speedM2 = constrain(x - y, -300, 300);

    Serial.print(millis());
    Serial.print(",");
    Serial.print(speedM1);
    Serial.print(",");
    Serial.println(speedM2);

    if (abs(speedM1 - prevM1) > 5 || speedM1 == 0)
    {
        robot.setJointSpeed(MOTOR1, speedM1);
        prevM1 = speedM1;
        delay(10);
    }
    if (abs(speedM2 - prevM2) > 5 || speedM2 == 0)
    {
        robot.setJointSpeed(MOTOR2, speedM2); 
        delay(10); 
        prevM2 = speedM2;
    }
}