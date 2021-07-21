#define USE_USBCON

#include <Servo.h>
#include <ros.h>
#include <geometry_msgs/Twist.h>

int buzzer_pin = 17;
int red_led_pin = 10;
int green_led_pin = 8;

byte read_angle_pin = 5;
byte read_linear_pin = 6;
float read_angle_value;
float read_linear_value;

ros::NodeHandle  nh;
geometry_msgs::Twist geo_msg;

Servo linear_servo;
Servo angular_servo;
int linear_servo_pin = 11;
int angular_servo_pin = 9;

int linear_vel = 20;
int angle = 90;

void cmd_vel_subscriber(const geometry_msgs::Twist& cmd_vel) {
  digitalWrite(green_led_pin,HIGH);
  
  linear_vel = cmd_vel.linear.x;
  angle = cmd_vel.angular.z;

  linear_vel = map(linear_vel, -30, 30, 1440, 1540);
  angle = map(angle, -30, 30, 1220, 1820);
  
  linear_servo.writeMicroseconds(linear_vel);
  angular_servo.writeMicroseconds(angle);
  if(linear_vel < 1300) {
    digitalWrite(buzzer_pin, LOW);
  }
  else if(linear_vel < 1450){
    digitalWrite(buzzer_pin,HIGH);
    digitalWrite(red_led_pin,HIGH);
  }
  else{
    digitalWrite(buzzer_pin,LOW);
    digitalWrite(red_led_pin,LOW);
  }

  digitalWrite(13, HIGH - digitalRead(13));
}

ros::Subscriber<geometry_msgs::Twist> sub("cmd_vel", cmd_vel_subscriber);
ros::Publisher pub("cmd_vel", &geo_msg);

void setup() {
  pinMode(13, OUTPUT);
  pinMode(buzzer_pin,OUTPUT);
  pinMode(green_led_pin,OUTPUT);
  pinMode(red_led_pin, OUTPUT);

  nh.initNode();
  nh.subscribe(sub);
  nh.advertise(pub);

  linear_servo.attach(linear_servo_pin);
  angular_servo.attach(angular_servo_pin);

  pinMode(read_angle_pin, INPUT);
  pinMode(read_linear_pin, INPUT);
}

void loop() {
  nh.spinOnce();
  delay(10);

  digitalWrite(green_led_pin,LOW);

  //publish하는 부분

  read_angle_value = pulseIn(read_angle_pin, HIGH);
  read_linear_value = pulseIn(read_linear_pin, HIGH);
  //들어오는 값은 1150 ~ 1470(중앙) ~ 1800

  geo_msg.linear.x = map(read_linear_value, 1100, 1900, -30, 30);
  geo_msg.angular.z = map(read_angle_value, 1100, 1900, 30, -30);
  pub.publish(&geo_msg);
}
