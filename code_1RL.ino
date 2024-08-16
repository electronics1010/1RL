#include <WiFi_Manager.h>
#include <Splitte_String.h>
#include <serial_setting.h>
#include <messege_MQTT.h>
#include <manifest.h>
#include <Cipher.h>
#include <system_device.h>


int pin_RL  = LED_BUILTIN;
int pin_swi = 16;
int pin_led = 5;

SYSTEM_DEVICE sys = SYSTEM_DEVICE();

void setup()
{
	delay(2000);
	Serial.begin(115200);
	Serial.setTimeout(1000);
	Serial.println();
	Serial.println("start");

	Serial.println("selecte Setting");
	Serial.println("s\tSetting from Serial");
	Serial.println("w\tSetting from WiFi");
	Serial.println("n\tSetting from internet");
	for (int i = 0; i < 10; i++)
	{
		delay(500);
		Serial.print(".");
		String rx = Serial.readString();
		if (rx == "s")
			sys.setting_Serial();
		else if (rx == "w")
			sys.setting_WIFI();
		else if (rx == "n")
			sys.setting_manifest();
	}
	Serial.println("End Setting");

	sys.begin();

	pinMode(pin_RL, OUTPUT);
	pinMode(pin_swi, INPUT);
	sys.led = pin_led;

	sys.loop();
}
void loop()
{
	sys.loop();

	if (digitalRead(pin_swi) == 1)
	{
		digitalWrite(pin_RL, !digitalRead(pin_RL));
		if (digitalRead(pin_RL) == 0)
			sys.send_data("on");
		if (digitalRead(pin_RL) == 1)
			sys.send_data("off");
		delay(1000);
	}

	String rx = Serial.readStringUntil(';');
	if (rx != "")
	{
		messege mm = messege("Serial", rx);
		sys.send_data(mm);
	}
}

void MQTT::mqtt_callback(char* topic, byte* payload, unsigned int length)
{
	String topic_s = topic;
	String payload_s = "";
	for (int i = 0; i < length; i++)
	{
		char c = payload[i];
		payload_s = payload_s + (String)c;
	}

	//payload_s = DES_decryption(payload_s);

	Serial.print("Received ");
	Serial.print(topic);
	Serial.print("=");
	Serial.println(payload_s);
	if (payload_s == "on")
		digitalWrite(pin_RL, 0);
	if (payload_s == "off")
		digitalWrite(pin_RL, 1);
}