/*
  steering.write(28);
  delay(500);
  steering.write(68);
  delay(500);
  steering.write(48);
  delay(500);
  /*
  
  /*
  for(pos = 34; pos < 68; pos += 1)  // goes from 0 degrees to 180 degrees 
  {                                  // in steps of 1 degree 
    steering.write(pos);             // tell servo to go to position in variable 'pos' 
    Serial.println(pos);
    delay(pingSpeed);                       // waits 15ms for the servo to reach the position 
  } 
  /*for(pos = 30; pos>=1; pos-=1)     // goes from 180 degrees to 0 degrees 
  {                                
    steering.write(pos);              // tell servo to go to position in variable 'pos' 
    delay(15);                       // waits 15ms for the servo to reach the position 
  } 
  */  
