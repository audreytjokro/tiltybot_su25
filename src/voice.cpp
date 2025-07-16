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

        delay(100);

        int moveSpeed = 200;
        int moveDuration = 600;  // ms

        if (dir == "forward" || dir == "go" || dir == "goforward") {
            Serial.print("M1 command: "); Serial.println(moveSpeed);
            robot.setJointSpeed(MOTOR1, moveSpeed);
            Serial.print("M2 command: "); Serial.println(moveSpeed);
            robot.setJointSpeed(MOTOR2, moveSpeed);
            delay(moveDuration);
            robot.setJointSpeed(MOTOR1, 0);
            robot.setJointSpeed(MOTOR2, 0);
            Serial.println("Received command: forward");
        } else if (dir == "back" || dir == "backward" || dir == "gobackward") {
            robot.setJointSpeed(MOTOR1, -moveSpeed);
            robot.setJointSpeed(MOTOR2, -moveSpeed);
            delay(moveDuration);
            robot.setJointSpeed(MOTOR1, 0);
            robot.setJointSpeed(MOTOR2, 0);
            Serial.println("Received command: backward");
        } else if (dir == "left" || dir == "turnleft") {
            robot.setJointSpeed(MOTOR1, -moveSpeed);
            robot.setJointSpeed(MOTOR2, moveSpeed);
            delay(moveDuration);
            robot.setJointSpeed(MOTOR1, 0);
            robot.setJointSpeed(MOTOR2, 0);
            Serial.println("Received command: left");
        } else if (dir == "right" || dir == "turnright") {
            robot.setJointSpeed(MOTOR1, moveSpeed);
            robot.setJointSpeed(MOTOR2, -moveSpeed);
            delay(moveDuration);
            robot.setJointSpeed(MOTOR1, 0);
            robot.setJointSpeed(MOTOR2, 0);
            Serial.println("Received command: right");  
        } else if (dir == "stop" || dir == "halt") {
            robot.setJointSpeed(MOTOR1, 0);
            robot.setJointSpeed(MOTOR2, 0);
        } else if (dir == "turnaround" || dir == "turn_around") {
            robot.setJointSpeed(MOTOR1, moveSpeed);
            robot.setJointSpeed(MOTOR2, moveSpeed);
            delay(moveDuration);

            robot.setJointSpeed(MOTOR1, -moveSpeed);
            robot.setJointSpeed(MOTOR2, moveSpeed);
            delay(moveDuration);

            robot.setJointSpeed(MOTOR1, -moveSpeed);
            robot.setJointSpeed(MOTOR2, moveSpeed);
            delay(moveDuration);

            robot.setJointSpeed(MOTOR1, moveSpeed);
            robot.setJointSpeed(MOTOR2, moveSpeed);
        }

        // delay(moveDuration);
        // robot.setJointSpeed(MOTOR1, 0);
        // robot.setJointSpeed(MOTOR2, 0);

        res->setStatusCode(200);
        res->println("OK");
        res->flush();
    });
    secureServer->registerNode(moveNode);

    Serial.println("Starting server...");
    secureServer->start();
    if (secureServer->isRunning()) {
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