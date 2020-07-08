import android.content.Intent;
import android.os.Bundle;
import ketai.net.bluetooth.*;
import ketai.sensors.*;
import java.nio.charset.Charset;

String msg;

KetaiSensor sensor;

float accX, accY, accZ;

long prevTime, currentTime;

long wait = 10l;
long time;

void setup()
{
  textAlign(CENTER, CENTER);
  textSize(18 * displayDensity);
  orientation(PORTRAIT);
  fullScreen();
  frameRate(60);
  ellipseMode(RADIUS);
  
  sensor = new KetaiSensor(this);
  sensor.start();
  bt.start();
  bt.getPairedDeviceNames();
  
  time = millis();
}

void draw()
{
  background(153, 224, 255);
  
  if(isConfiguring)
  {
    text("Touch to connect", width/2, height/2);
  }
  
  else
  {
    if(isConnecting)
    {
      if (bt.getConnectedDeviceNames().size() > 0)
      {
        isConnecting = false;
      }
      
      text("Connecting...", width/2, height/2);
    }
    
    else
    {
      if(millis() - time >= wait)
      {
        sendAcceleration();
        time = millis();
      }
      
      drawGUI();
      
      //sendAcceleration();
      text(accX, width/2, height/2 - 100);
      text(accY, width/2, height/2);
      
      if(bt.getConnectedDeviceNames().size() == 0)
      {
        isConfiguring = true;
      }
    }
  }
}

void mousePressed()
{
  if(isConfiguring && !isConnecting) connectToRobot();
}

void onAccelerometerEvent(float x, float y, float z)
{
  accX = x;
  accY = y;
  accZ = z;
  
  accX = constrain(accX, -4, 4);
  accY = constrain(accY, -4, 4);
  accZ = constrain(accZ, -4, 4);
  
  accX = map(accX, -4, 4, -1, 1);
  accY = map(accY, -4, 4, -1, 1);
  accZ = map(accZ, -4, 4, -1, 1);
  
  accX = floor(accX * 100.0f);
  accY = floor(accY * 100.0f);
  accZ = floor(accZ * 100.0f);
}

void sendAcceleration()
{  
  String msg = (int)accX + "," + (int) accY + ";";
  
  sendToRobot(msg.getBytes());
}
