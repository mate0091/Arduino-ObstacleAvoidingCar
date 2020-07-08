KetaiBluetooth bt;
boolean isConfiguring = true;
String lastMsg = "";
boolean isConnecting = false;
float timeScale = 0.001;

void onCreate(Bundle savedInstanceState)
{
  super.onCreate(savedInstanceState);
  bt = new KetaiBluetooth(this);
}

void onActivityResult(int requestCode, int resultCode, Intent data)
{
  bt.onActivityResult(requestCode, resultCode, data);
}

void onBluetoothDataEvent(String who, byte[] data)
{
  lastMsg = new String(data);
}

void sendToRobot(byte[] msg)
{
  bt.writeToDeviceName("HC-06", msg);
}

void connectToRobot()
{
  isConnecting = true;
  bt.connectToDeviceByName("HC-06");
  
  isConfiguring = false;
}
