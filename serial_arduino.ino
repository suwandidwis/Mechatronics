const int pwm1 = 2 ;	//initializing pin 2 as pwm
const int in_1 = 8 ;
const int in_2 = 9 ;
const int pwm2 = 3 ;	//initializing pin 3 as pwm
const int in_3 = 10 ;
const int in_4 = 11 ;

//For providing logic to L298 IC to choose the direction of the DC motor 

void setup()
{
	Serial.begin(9600);     //Starting serial communication
	pinMode(pwm1,OUTPUT) ;  	//we have to set PWM pin as output
	pinMode(in_1,OUTPUT) ; 	//Logic pins are also set as output
	pinMode(in_2,OUTPUT) ;
	pinMode(pwm2,OUTPUT) ;  	//we have to set PWM pin as output
	pinMode(in_3,OUTPUT) ; 	//Logic pins are also set as output
	pinMode(in_4,OUTPUT) ;
}

void loop()
{
	float dq1Ratio, dq2Ratio;
	char buf;

	digitalWrite(in_1,HIGH) ;
      	digitalWrite(in_2,LOW) ;
      	digitalWrite(in_3,HIGH) ;
      	digitalWrite(in_4,LOW) ;
  	//while(Serial.available())
  	//{
		Serial.println("Y");       
        	dq1Ratio = Serial.read();
        	dq1Ratio = dq1Ratio*100/255;
                Serial.println(dq1Ratio);
         
        	Serial.println("Z");   
        	dq2Ratio = Serial.read();
        	dq2Ratio = dq2Ratio*100/255;
                Serial.println(dq2Ratio);

		analogWrite(pwm1,dq1Ratio) ;
        	analogWrite(pwm2,dq2Ratio) ;
		delay(2000) ;
  	//}
}
