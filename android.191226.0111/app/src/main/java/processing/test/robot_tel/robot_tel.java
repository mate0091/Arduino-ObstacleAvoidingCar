package processing.test.robot_tel;

import processing.core.*; 
import processing.data.*; 
import processing.event.*; 
import processing.opengl.*; 

import android.content.Intent; 
import android.os.Bundle; 
import ketai.net.bluetooth.*; 
import ketai.sensors.*; 
import java.nio.charset.Charset; 

import java.util.HashMap; 
import java.util.ArrayList; 
import java.io.File; 
import java.io.BufferedReader; 
import java.io.PrintWriter; 
import java.io.InputStream; 
import java.io.OutputStream; 
import java.io.IOException; 

public class robot_tel extends PApplet {







String msg;

KetaiSensor sensor;

float accX, accY, accZ;

long prevTime, currentTime;

Thread sender;

public void setup()
{
  textAlign(CENTER, CENTER);
  textSize(18 * displayDensity);
  orientation(PORTRAIT);
  
  frameRate(60);
  ellipseMode(RADIUS);
  
  sensor = new KetaiSensor(this);
  sensor.start();
  bt.start();
  bt.getPairedDeviceNames();
}

public void draw()
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
      sendAcceleration();
      
      drawGUI();
      
      //sendAcceleration();
      text(accX, width/2, height/2 - 100);
      text(accY, width/2, height/2);
      
      text("last sent: " + msg, width/2, height/2 + 300);
      
      text("last msg: " + lastMsg, width/2, height/2 + 500);
      
      if(bt.getConnectedDeviceNames().size() == 0)
      {
        isConfiguring = true;
      }
    }
  }
}

public void mousePressed()
{
  if(isConfiguring && !isConnecting) connectToRobot();
}

public void onAccelerometerEvent(float x, float y, float z)
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
  
  accX = floor(accX * 100.0f) / 100.0f;
  accY = floor(accY * 100.0f) / 100.0f;
  accZ = floor(accZ * 100.0f) / 100.0f;
}

public void sendAcceleration()
{
  msg = "{" + accX + ","+ accY + "}\n";
  
  sendToRobot(msg.getBytes());
}
KetaiBluetooth bt;
boolean isConfiguring = true;
String lastMsg = "";
boolean isConnecting = false;
float timeScale = 0.001f;

public void onCreate(Bundle savedInstanceState)
{
  super.onCreate(savedInstanceState);
  bt = new KetaiBluetooth(this);
}

public void onActivityResult(int requestCode, int resultCode, Intent data)
{
  bt.onActivityResult(requestCode, resultCode, data);
}

public void onBluetoothDataEvent(String who, byte[] data)
{
  lastMsg = new String(data);
}

public void sendToRobot(byte[] msg)
{  
  bt.writeToDeviceName("HC-06", msg);
}

public void connectToRobot()
{
  isConnecting = true;
  bt.connectToDeviceByName("HC-06");
  
  isConfiguring = false;
}
int radius = 200;

public void drawGUI()
{
  fill(255, 255, 255);
  circle(width/2, height/2 - 500, radius);
  
  fill(0, 0, 0);
  stroke(0, 0, 0);
  circle(width / 2 - accX * radius, height / 2 + accY * radius - 500, 16);
}
public class SenderThread extends Thread
{
  public void run()
  {
    
  }
}
  public void settings() {  fullScreen(); }
}
