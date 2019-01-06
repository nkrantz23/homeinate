// This #include statement was automatically added by the Particle IDE.
#include <Adafruit_SSD1306.h>

// This #include statement was automatically added by the Particle IDE.
#include <OneWire.h>

#define OLED_DC D3
#define OLED_CS D4
#define OLED_RESET D5
#define TEMP_PIN D2

const int ERR_NO_MORE_ADDRESS = -255;
const int ERR_CRC_INVALID = -256;
const int ERR_UNKNOWN_DEVICE_TYPE = -257;
const int TEXT_MEDIUM = 5;
const int TEXT_SMALL = 2;
const int LOOP_TIMER = 10000;

OneWire ds = OneWire(TEMP_PIN);
Adafruit_SSD1306 display(OLED_DC, OLED_RESET, OLED_CS);
String displayMode = "temp";
double lastTemp;
int lastLight;

void setup()
{
    setupSensors();
    setupDisplay();
    setupVariables();
    setupFunctions();
}

void loop(void)
{
    double temp;

    temp = readTemperature();

    if (temp <= -255)
    {
        // continue the loop and try again if temperature reading comes
        // back bad. not sure exactly why, but the sensor here sometimes
        // will output gibberish and return bad data.
        return;
    }

    lastTemp = temp;

    publishTemperature(lastTemp);

    lastLight = readLight();

    publishLight(lastLight);

    updateDisplay();

    delay(LOOP_TIMER);
}

// -------------------------------------------------------------------------------------
// PRIVATE
// -------------------------------------------------------------------------------------
void setupSensors()
{
    Serial.begin(9600);

    pinMode(D3, OUTPUT);
    pinMode(D5, OUTPUT);

    digitalWrite(D3, LOW);
    digitalWrite(D5, HIGH);
}

void setupDisplay()
{
    display.begin(SSD1306_SWITCHCAPVCC);
    display.setTextSize(TEXT_MEDIUM);
    display.setTextColor(WHITE);
}

void setupVariables()
{
    Particle.variable("temperature", &lastTemp, DOUBLE);
    Particle.variable("light", &lastLight, INT);
}

void setupFunctions()
{
    Particle.function("slack", slack);
}

void publishTemperature(double temperature)
{
    Serial.printlnf(" Celsius %f", temperature);
    double fahrenheit = celsiusToFarenheit(temperature);
    Serial.printlnf(" Fahrenheit %f", fahrenheit);
    Particle.publish("natehome/temperature", String(fahrenheit), PRIVATE);
}

int readLight()
{
    return analogRead(A0);
}

void publishLight(int light)
{
    Serial.printlnf(" Light %d", light);
    Particle.publish("natehome/light", String(light), PRIVATE);
}

void updateDisplay()
{
    Serial.printlnf(" Display Mode %s", displayMode.c_str());

    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(TEXT_MEDIUM);

    if (displayMode == "light")
    {
        display.print(String(lastLight));
        display.setTextSize(TEXT_SMALL);
        display.print(" us");
        displayMode = "temp";
    }
    else
    {
        display.print(farenheitDisplay(lastTemp));
        display.setTextSize(TEXT_SMALL);
        display.print(" F");
        displayMode = "light";
    }

    display.display();
}

int slack(String command)
{
    bool success;
    String data;

    if (command == "")
    {
        data = "It is currently " + farenheitDisplay(lastTemp) + "F in the house.";
    }
    else
    {
        data = command;
    }

    success = Particle.publish("slack/message", data, 60, PRIVATE);

    if (!success)
    {
        Particle.publish("slack/message/failed");

        return -1;
    }

    return 1;
}

String farenheitDisplay(double temperature)
{
    return String((int)(celsiusToFarenheit(temperature) + 0.5));
}

double celsiusToFarenheit(double celsius)
{
    return celsius * 1.8 + 32.0;
}

// -------------------------------------------------------------------------------------
// COPY PASTA
// -------------------------------------------------------------------------------------
// basically the OneWire thing lets sensors output data on a single, standard pin.
// this just knows which bits in the pin output to parse as temperature.
// -------------------------------------------------------------------------------------
double readTemperature()
{
    byte i;
    byte present = 0;
    byte type_s;
    byte data[12];
    byte addr[8];
    double celsius;

    if (!ds.search(addr))
    {
        Serial.println("No more addresses.");
        Serial.println();
        ds.reset_search();
        delay(250);
        return ERR_NO_MORE_ADDRESS;
    }

    // The order is changed a bit in this example
    // first the returned address is printed
    Serial.print("ROM =");
    for (i = 0; i < 8; i++)
    {
        Serial.write(' ');
        Serial.print(addr[i], HEX);
    }

    // second the CRC is checked, on fail,
    // print error and just return to try again
    if (OneWire::crc8(addr, 7) != addr[7])
    {
        Serial.println("CRC is not valid!");
        return ERR_CRC_INVALID;
    }

    Serial.println();

    // we have a good address at this point
    // what kind of chip do we have?
    // we will set a type_s value for known types or just return

    // the first ROM byte indicates which chip
    switch (addr[0])
    {
    case 0x10:
        Serial.println("  Chip = DS1820/DS18S20");
        type_s = 1;
        break;
    case 0x28:
        Serial.println("  Chip = DS18B20");
        type_s = 0;
        break;
    case 0x22:
        Serial.println("  Chip = DS1822");
        type_s = 0;
        break;
    case 0x26:
        Serial.println("  Chip = DS2438");
        type_s = 2;
        break;
    default:
        Serial.println("Unknown device type.");
        return ERR_UNKNOWN_DEVICE_TYPE;
    }

    // this device has temp so let's read it

    ds.reset();      // first clear the 1-wire bus
    ds.select(addr); // now select the device we just found
    // ds.write(0x44, 1);     // tell it to start a conversion, with parasite power on at the end
    ds.write(0x44, 0); // or start conversion in powered mode (bus finishes low)

    // just wait a second while the conversion takes place
    // different chips have different conversion times, check the specs, 1 sec is worse case + 250ms
    // you could also communicate with other devices if you like but you would need
    // to already know their address to select them.

    delay(1000); // maybe 750ms is enough, maybe not, wait 1 sec for conversion

    // we might do a ds.depower() (parasite) here, but the reset will take care of it.
    // first make sure current values are in the scratch pad

    present = ds.reset();
    ds.select(addr);
    ds.write(0xB8, 0); // Recall Memory 0
    ds.write(0x00, 0); // Recall Memory 0

    // now read the scratch pad

    present = ds.reset();
    ds.select(addr);
    ds.write(0xBE, 0); // Read Scratchpad
    if (type_s == 2)
    {
        ds.write(0x00, 0); // The DS2438 needs a page# to read
    }

    // transfer and print the values

    Serial.print("  Data = ");
    Serial.print(present, HEX);
    Serial.print(" ");
    for (i = 0; i < 9; i++)
    { // we need 9 bytes
        data[i] = ds.read();
        Serial.print(data[i], HEX);
        Serial.print(" ");
    }
    Serial.print(" CRC=");
    Serial.print(OneWire::crc8(data, 8), HEX);
    Serial.println();

    // Convert the data to actual temperature
    // because the result is a 16 bit signed integer, it should
    // be stored to an "int16_t" type, which is always 16 bits
    // even when compiled on a 32 bit processor.
    int16_t raw = (data[1] << 8) | data[0];
    if (type_s == 2)
        raw = (data[2] << 8) | data[1];
    byte cfg = (data[4] & 0x60);

    switch (type_s)
    {
    case 1:
        raw = raw << 3; // 9 bit resolution default
        if (data[7] == 0x10)
        {
            // "count remain" gives full 12 bit resolution
            raw = (raw & 0xFFF0) + 12 - data[6];
        }
        celsius = (float)raw * 0.0625;
        break;
    case 0:
        // at lower res, the low bits are undefined, so let's zero them
        if (cfg == 0x00)
            raw = raw & ~7; // 9 bit resolution, 93.75 ms
        if (cfg == 0x20)
            raw = raw & ~3; // 10 bit res, 187.5 ms
        if (cfg == 0x40)
            raw = raw & ~1; // 11 bit res, 375 ms
        // default is 12 bit resolution, 750 ms conversion time
        celsius = (float)raw * 0.0625;
        break;

    case 2:
        data[1] = (data[1] >> 3) & 0x1f;
        if (data[2] > 127)
        {
            celsius = (float)data[2] - ((float)data[1] * .03125);
        }
        else
        {
            celsius = (float)data[2] + ((float)data[1] * .03125);
        }
    }

    return celsius;
}
