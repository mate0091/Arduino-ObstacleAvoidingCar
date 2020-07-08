int radius = 200;

void drawGUI()
{
  fill(255, 255, 255);
  circle(width/2, height/2 - 500, radius);
  
  fill(0, 0, 0);
  stroke(0, 0, 0);
  circle(width / 2 - accX * radius, height / 2 + accY * radius - 500, 16);
}
