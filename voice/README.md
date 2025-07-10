# Voice Control Implementation

## Overview

This section documents the decision-making process for implementing voice control in TiltyBot, including technical challenges encountered, solutions tested, and the final implementation choice. This serves as guidance for future researchers and developers working on similar HRI systems.

## Implementation Options Evaluated

### Option 1: Whisper-Based Local Processing
**Architecture:** Phone → Laptop (Whisper) → Robot
- Advanced speech recognition with confidence scores
- Offline processing capability  
- Research-grade transcription accuracy
- Custom vocabulary and model training potential

### Option 2: Browser Speech Recognition
**Architecture:** Phone → Cloud Speech API → Robot  
- Built-in browser capabilities
- Simplified deployment
- Reliable cross-platform support
- No additional hardware requirements

## Testing Process and Challenges Encountered

### Whisper Implementation Attempts

#### **Attempt 1: Three-Device Network Architecture**
```
Challenge: Complex network bridging between TinyPico AP and laptop
- TinyPico creates isolated WiFi network (192.168.4.1)
- Laptop runs Whisper service on different network segment
- Phone unable to reach laptop speech service reliably
- IP discovery failures and CORS issues

Result: Multiple days of network debugging without reliable operation
```

#### **Attempt 2: Service Auto-Discovery**
```
Challenge: Dynamic IP assignment and service detection
- Laptop IP varies on TinyPico network (192.168.4.2, 192.168.4.3, etc.)
- Auto-discovery timeouts in browser environment
- Service availability inconsistent across different devices
- Manual IP configuration required for each deployment

Result: System worked intermittently but required technical setup for each use
```

#### **Attempt 3: Simplified Whisper Companion**
```
Challenge: Maintaining TiltyBot's design philosophy
- Added complexity conflicted with original 2-device simplicity
- Required laptop dependency for workshops and demos
- Increased setup time from minutes to 30+ minutes
- Multiple failure points (laptop, network, service discovery)

Result: Technically functional but violated core design principles
```

### Key Technical Roadblocks

1. **Network Isolation:** TinyPico's AP mode creates isolated network without internet
2. **Service Discovery:** Unreliable automatic detection of laptop services
3. **Setup Complexity:** Multi-device coordination required for each session
4. **Debugging Difficulty:** Multiple network layers made troubleshooting complex
5. **Workshop Limitations:** Laptop dependency reduced portability and ease of use

## Decision Matrix

| Criteria | Whisper Implementation | Browser Speech Recognition |
|----------|----------------------|---------------------------|
| **Setup Time** | 30+ minutes per session | <5 minutes per session |
| **Reliability** | Intermittent network issues | Consistent operation |
| **Hardware Requirements** | Phone + Robot + Laptop | Phone + Robot |
| **Research Data Quality** | Excellent (detailed logs) | Good (structured logs) |
| **Workshop Suitability** | Poor (complex setup) | Excellent (immediate use) |
| **Maintenance** | High (multiple services) | Low (browser standards) |
| **Reproducibility** | Difficult (network config) | Easy (standard setup) |
| **Development Time** | 2-3 months | 2-3 weeks |

## Final Implementation Choice: Browser Speech Recognition

### Rationale

1. **Keeps it accessible and reliable:** Preserves the original 2-device simplicity that makes TiltyBot accessible and reliable

2. **Research practicality:** Enables immediate deployment for studies without technical barriers for participants

3. **Workshop readiness:** Multiple participants can use the system without complex setup procedures

4. **Development efficiency:** Allows focus on research questions rather than infrastructure debugging

### Implementation Requirements

#### Prerequisites

- TiltyBot system already running and functional
- Access to a WiFi network with internet connectivity
- TinyPico development environment set up

#### Technical Modifications
#### Step 1: Add Internet Bridge Capability to TinyPico
Once your TiltyBot is running and you want to add voice capabilities with browser speech recognition, follow these steps:

#### 1 Edit init.h - Add extern declarations for WiFi credentials
Add these lines after the #define statements and before "using namespace httpsserver;"

```cpp
extern const char *lab_wifi_ssid;
extern const char *lab_wifi_password;
```

#### 2 Edit network.h - Add your WiFi network credentials
```cpp
const char *ssid = "YOURGROUPNAME";
const char *password = "";

// Add these new lines for internet bridge:
const char *lab_wifi_ssid = "YourWiFiNetworkName";      // Replace with your WiFi name
const char *lab_wifi_password = "YourWiFiPassword";     // Replace with your WiFi password
```

#### 3 Edit init.cpp - Replace the initWiFi function

Find this existing function:
```cpp
void initWiFi(const char *ssid, const char *password, const char *index, int mode = AP)
```
Replace the entire function with:
```cpp
void initWiFi(const char *ssid, const char *password, const char *index, int mode = AP)
{
    // Include WiFi credentials
    extern const char *lab_wifi_ssid;
    extern const char *lab_wifi_password;
    
    if (mode == AP)
    {
        // BRIDGE MODE: Both AP and client
        WiFi.mode(WIFI_AP_STA);                    // Enable both AP and Station mode
        
        // Create access point for phones
        WiFi.softAP(ssid, password);
        IPAddress AP_IP = WiFi.softAPIP();
        Serial.print("AP IP address: ");
        Serial.print("HTTPS://");
        Serial.print(AP_IP);
        Serial.println(index);
        
        // Also connect to external WiFi for internet access
        Serial.print("Connecting to WiFi: ");
        Serial.println(lab_wifi_ssid);
        WiFi.begin(lab_wifi_ssid, lab_wifi_password);
        
        // Wait for connection (with timeout)
        int attempts = 0;
        while (WiFi.status() != WL_CONNECTED && attempts < 20) {
            delay(500);
            Serial.print(".");
            attempts++;
        }
        
        if (WiFi.status() == WL_CONNECTED) {
            Serial.println();
            Serial.print("Connected to external WiFi! Local IP: ");
            Serial.println(WiFi.localIP());
            Serial.println("Bridge mode active: Phones have internet access");
        } else {
            Serial.println();
            Serial.println("Could not connect to external WiFi - AP mode only");
        }
    }
    else
    {
        // Original client mode (unchanged)
        WiFi.begin(ssid, password);
        while (WiFi.status() != WL_CONNECTED)
        {
            delay(1000);
            Serial.println("Connecting to WiFi..");
        }

        Serial.print("HTTPS://");
        Serial.print(WiFi.localIP());
        Serial.println(index);
    }
}
```

#### Flash and Test
```bash
bashpio run --target upload
```
#### Expected Serial Output:
```plaintext
AP IP address: HTTPS://192.168.4.1/[whichever of the motor.html files is being run].html
Connecting to WiFi: YourWiFiNetworkName
........
Connected to external WiFi! Local IP: 10.0.1.15
Bridge mode active: Phones have internet access
````


#### Step 2: HTML Enhancement

Add browser speech recognition with research features to HTML files (2motor, tilty, drive).
Implement voice command processing and motor control integration

#### Step 3: Data Collection

Implement structured logging and CSV export capabilities
Add session management and research metrics tracking

#### **Network Architecture**
```
Internet ← Lab WiFi ← TinyPico ← Phone
                        ↓
                   Robot Control
```

## Research Capabilities You Can Do 

### Data Collection Features
- **Interaction Logging:** Timestamp, transcript, confidence, success/failure
- **Alternative Analysis:** Multiple recognition candidates for comparison
- **A/B Testing Framework:** Different confidence thresholds and error recovery strategies
- **Session Management:** Participant tracking and study condition assignment
- **Real-time Metrics:** Success rates, confidence distributions, error patterns

### Research Studies Enabled
1. **Confidence Threshold Studies:** Impact of different acceptance thresholds on user trust
2. **Command Pattern Analysis:** Natural language variations in robot control
3. **Error Recovery Research:** Effectiveness of different failure handling strategies
4. **Adaptive Interaction:** System learning and personalization capabilities

## Lessons Learned and Best Practices

### For Future Implementations

1. **Preserve Core Design Principles:** Maintain the simplicity that makes systems accessible
2. **Prioritize Reliability Over Features:** Consistent operation enables better research than advanced features that fail
3. **Consider the Full User Journey:** From setup to data collection to analysis
4. **Test in Target Environment:** Workshop/demo conditions often differ from lab testing
5. **Document Decision Rationale:** Future developers benefit from understanding trade-offs made

## Known Issues and Limitations
- Browser speech recognition requires internet connectivity
- Performance varies across different mobile browsers
- Limited to English language commands currently

### When to Consider Whisper?

Whisper implementation becomes valuable when:
- **Offline Operation Required:** No internet access available
- **Custom Vocabulary Needed:** Domain-specific terminology requires training
- **Maximum Accuracy Critical:** Research requires highest possible transcription quality
- **Development Resources Available:** 2-3 months for robust implementation
- **Technical Support Present:** Dedicated personnel for setup and maintenance

## Recommendations for Future Work

1. **Start with Browser Implementation:** Establish baseline functionality and research capabilities
2. **Collect Usage Data:** Understand actual research requirements before adding complexity
3. **Consider Hybrid Approach:** Browser for demos/workshops, Whisper for detailed studies when needed
4. **Plan for Evolution:** Design system to accommodate future enhancements without breaking existing functionality

## Contact and Support

For questions about this implementation decision or technical details:
- **Built By:** Audrey Tjokro
- **Research Supervisor:** Professor Wendy Ju
- **Collaborator:** Albert Han 

---

*This documentation reflects lessons learned during Summer 2025 development cycle. Future implementations should consider evolving browser capabilities and research requirements.*
