#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

const char auth[] = "0dae2c2438224a1fa685de875a339d75";

// Casa lisboa
//const char ssid[] = "Vodafone-04AA15";
//const char pass[] = "3a4Xpv2gy2";

// iPhone Hotspot
const char ssid[] = "AlmeidaSiPhone";
const char pass[] = "tartaruga";

// Leandro Hotspot
//const char ssid[] = "AndroidAP_1662";
//const char pass[] = "281a3c0bc213";

// HUAWEI HotSpot
// const char ssid[] = "HUAWEI-5776-7648";
// const char pass[] = "MNA99RF7";

#define BLYNK_GREEN     "#00FF00"
#define BLYNK_RED       "#FF0000"
#define BLYNK_WHITE     "#FFFFFF"
#define BLYNK_YELLOW    "#FFFF00"
#define BLYNK_BLUE      "#0000FF"

WidgetTerminal terminal(V1);
WidgetLED led1(V4);
WidgetLED led2(V5);
WidgetLED led3(V6);
WidgetLED led4(V7);
BLYNK_CONNECTED() {
  Blynk.syncVirtual(V11);
  Blynk.syncVirtual(V12);
}
bool button1, button2;
bool game4 = false, waiting_for_player_one = false, waiting_for_player_two = false;;
bool wainting4push = false;
int player1 = 0, player2 = 0;
int rand_led, rand_color;
int true_count, hipo_count;

//WIFI
WiFiServer server(80);

// Terminal
BLYNK_WRITE(V1)
{
  if (game4) {
    hipo_count = param.asInt();
    if (waiting_for_player_one) {
      if (true_count == hipo_count) {
        player1++;
      } else {
        player1--;
      }
      delay(1000);
      waiting_for_player_one = false;
      waiting_for_player_two = true;
      terminal.print("Player2 : ");
    } else if (waiting_for_player_two) {
      if (true_count == hipo_count) {
        player2++;
      }
      else {
        player2--;
      }
      delay(1000);
      update_score();
      waiting_for_player_two = false;
      game4 = false;
      update_score();
    }
  } else {
    leds_off();
    if (param.asInt() == 1) {
      terminal.println("Leds Race") ;
      terminal.flush();
      alternate_colors();
      mode_one();
      leds_race();
    } else if (param.asInt() == 2) {
      terminal.println("Danger Color") ;
      terminal.flush();
      mode_one();
      danger_color();
    } else if (param.asInt() == 3) {
      terminal.println("Time is Counting") ;
      terminal.flush();
      white_colors();
      mode_two();
      time_is_counting();
    } else if (param.asInt() == 4) {
      terminal.println("Memory Challenge") ;
      terminal.flush();
      alternate_colors();
      mode_three();
      true_count = memory_challenge();
    } else if (param.asInt() == 5) {
      terminal.println("2 Colors") ;
      terminal.flush();
      alternate_colors();
      mode_four();
      two_colors();
    } else {
      new_game();
    }
  }

  // Ensure everything is sent
  terminal.flush();
}

// BUTTONS
BLYNK_WRITE(V11)
{
  button1 = param.asInt();
  if (button2) {
    button1 = false;
  }
}
BLYNK_WRITE(V12)
{
  button2 = param.asInt();
  if (button1) {
    button2 = false;
  }
}

void setup()
{
  // Debug console
  Serial.begin(9600);

  init_wifi();

  new_game();
  update_score();
  init_buttons();
  leds_off();
  init_server();
}

void loop()
{
  website();
  Blynk.run();
  if (wainting4push) {
    frist2pushthebutton();
  }
}

void leds_off() {
  led1.off(); led2.off(); led3.off(); led4.off();
}

void update_score() {
  Blynk.virtualWrite(V9, invert_2(player1));
  Blynk.virtualWrite(V10, player2);
  delay(200);
  new_game();
}

void new_game() {
  terminal.clear();
  terminal.print("Use this URL to get info about the game: ");
  terminal.print("http://");
  terminal.print(WiFi.localIP());
  terminal.println("/");
  terminal.println("Insert a number between 1 an 5.") ;
  terminal.flush();
}

void frist2pushthebutton() {

  if (!(button1 || button2)) {
    return;
  }
  if (button1) {
    Blynk.setProperty(V11, "color", BLYNK_GREEN);
    Blynk.setProperty(V12, "color", BLYNK_RED);
    player1++;
  } else {
    Blynk.setProperty(V11, "color", BLYNK_RED);
    Blynk.setProperty(V12, "color", BLYNK_GREEN);
    player2++;
  }
  leds_off();
  delay(1000);
  Blynk.setProperty(V11, "color", BLYNK_WHITE);
  Blynk.setProperty(V12, "color", BLYNK_WHITE);
  wainting4push = false;
  update_score();
}

void alternate_colors() {
  Blynk.setProperty(V4, "color", BLYNK_RED);
  Blynk.setProperty(V5, "color", BLYNK_YELLOW);
  Blynk.setProperty(V6, "color", BLYNK_GREEN);
  Blynk.setProperty(V7, "color", BLYNK_BLUE);
}

void white_colors() {
  Blynk.setProperty(V4, "color", BLYNK_WHITE);
  Blynk.setProperty(V5, "color", BLYNK_WHITE);
  Blynk.setProperty(V6, "color", BLYNK_WHITE);
  Blynk.setProperty(V7, "color", BLYNK_WHITE);
}

void leds_race() {

  led1.on();
  delay(random(1000, 5000));
  led1.off();
  int rand_number = random(0, 4);
  if (rand_number != 0) {
    led2.on();
    delay(random(100, 2500));
    led2.off();
  }
  led3.on();
  Blynk.virtualWrite(V11, LOW);
  wainting4push = true;
}

void danger_color() {

  int number_of_times_to_blink = random(15, 25);
  delay(500);

  const char* color_vector[4];
  color_vector[0] = BLYNK_RED;
  color_vector[1] = BLYNK_WHITE;
  color_vector[2] = BLYNK_YELLOW;
  color_vector[3] = BLYNK_BLUE;

  for (int i = 1 ; i <= number_of_times_to_blink ; i++) {
    delay(100);
    rand_led = random(1, 5);
    rand_color = random(1, 5);
    if (i == number_of_times_to_blink) {
      color_vector[rand_color] = BLYNK_GREEN;
    }
    switch (rand_led) {
      case (1):
        Blynk.setProperty(V4, "color", color_vector[rand_color]);
        led1.on();
        delay(500);
        if (i != number_of_times_to_blink) {
          led1.off();
        }
        break;
      case (2):
        Blynk.setProperty(V5, "color", color_vector[rand_color]);
        led2.on();
        delay(500);
        if (i != number_of_times_to_blink) {
          led2.off();
        }
        break;
      case (3):
        Blynk.setProperty(V6, "color", color_vector[rand_color]);
        led3.on();
        delay(500);
        if (i != number_of_times_to_blink) {
          led3.off();
        }
        break;
      case (4):
        Blynk.setProperty(V7, "color", color_vector[rand_color]);
        led4.on();
        delay(500);
        if (i != number_of_times_to_blink) {
          led4.off();
        }
        break;
    }
  }
  wainting4push = true;
}

void time_is_counting() {

  int number = random(10, 16);
  int sec[4] = { -1};

  for (int i = number; i >= 1 ; i--) {
    for (int j = 1; j <= 6 && number - i <= 6; j++, i --) {
      get_decimal(i, sec);
      if (sec[0]) {
        led1.on();
      }
      if (sec[1]) {
        led2.on();
      }
      if (sec[2]) {
        led3.on();
      }
      if (sec[3]) {
        led4.on();
      }
      delay(1000);
      leds_off();
    }
    delay(1000);
  }
  wainting4push = true;
}

void get_decimal(int n, int sec[4]) {

  if (n > 7) {
    sec[0] = 1;
    n = n - 8;
  } else {
    sec[0] = 0;
  }
  if (n > 3) {
    sec[1] = 1;
    n = n - 4;
  } else {
    sec[1] = 0;
  }
  if (n > 1) {
    sec[2] = 1;
    n = n - 2;
  } else {
    sec[2] = 0;
  }
  if (n > 0) {
    sec[3] = 1;
    n = n - 1;
  } else {
    sec[3] = 0;
  }
}

int memory_challenge() {

  int count[4] = {0, 0, 0, 0};

  const char* color_vector[4];
  color_vector[0] = BLYNK_RED;
  color_vector[1] = BLYNK_WHITE;
  color_vector[2] = BLYNK_YELLOW;
  color_vector[3] = BLYNK_BLUE;

  for (int i = 1 ; i <= 9 ; i++) {
    delay(500);
    rand_color = random(1, 5);
    switch (rand_color) {
      case (1):
        Blynk.setProperty(V4, "color", color_vector[rand_color - 1]);
        led1.on();
        delay(1000);
        led1.off();
        count[0]++;
        break;
      case (2):
        Blynk.setProperty(V5, "color", color_vector[rand_color - 1]);
        led2.on();
        delay(1000);
        led2.off();
        count[1]++;
        break;
      case (3):
        Blynk.setProperty(V6, "color", color_vector[rand_color - 1]);
        led3.on();
        delay(1000);
        led3.off();
        count[2]++;
        break;
      case (4):
        Blynk.setProperty(V7, "color", color_vector[rand_color - 1]);
        led4.on();
        delay(1000);
        led4.off();
        count[3]++;
        break;
    }
  }
  game4 = true;
  terminal.println("How many times did which color blink, write a number like 1234");
  terminal.print("Player1 : ");
  waiting_for_player_one = true;
  return 1000 * count[0] + 100 * count[1] + 10 * count[2] + count[3] ;
}


void two_colors() {
  delay(500);

  int number2 = random (20, 26), led = 4, aux = 0, rand1;

  const char* color_vector[5];
  color_vector[0] = BLYNK_RED;
  color_vector[1] = BLYNK_YELLOW;
  color_vector[2] = BLYNK_GREEN;
  color_vector[3] = BLYNK_BLUE;
  color_vector[4] = BLYNK_WHITE;
  int base[4] = {0, 1, 2, 3};

  led1.on();
  led2.on();
  led3.on();
  led4.on();

  for (int i = number2; i >= 1; i--) {

    delay(750);

    if (i == 1) {
      int last;
      do {
        last = random(1, 5);
      } while (last == rand1);
      led = base[rand1 - 1];
      rand1 = last;
    } else {
      rand1 = random(1, 5);
    }


    switch (rand1) {
      case (1):
        Blynk.setProperty(V4, "color", color_vector[led]);
        aux = led;
        led = base[0];
        base[0] = aux;
        break;
      case (2):
        Blynk.setProperty(V5, "color", color_vector[led]);
        aux = led;
        led = base[1];
        base[1] = aux;
        break;
      case (3):
        Blynk.setProperty(V6, "color", color_vector[led]);
        aux = led;
        led = base[2];
        base[2] = aux;
        break;
      case (4):
        Blynk.setProperty(V7, "color", color_vector[led]);
        aux = led;
        led = base[3];
        base[3] = aux;
        break;
    }

  }

  wainting4push = true;
}

void mode_one() {
  Blynk.setProperty(V11, "offLabel", "uǝǝɹפ uo ƎW ɥsnԀ");
  Blynk.setProperty(V12, "offLabel", "Push ME on Green");
}

void mode_two() {
  Blynk.setProperty(V11, "offLabel", "0000 uo ƎW ɥsnԀ");
  Blynk.setProperty(V12, "offLabel", "Push ME on 0000");
}

void mode_three() {
  Blynk.setProperty(V11, "offLabel", "ssǝlǝsn ɯ,I 'ʍoN");
  Blynk.setProperty(V12, "offLabel", "Now, I´m useless");
}

void mode_four() {
  Blynk.setProperty(V11, "offLabel", "pǝʇɐǝdǝɹ sᴉ ɹoloɔ ɐ uǝɥʍ ƎW ssǝɹԀ");
  Blynk.setProperty(V12, "offLabel", "Press ME when a color is repeated");
}

String invert(int i) {
  switch (i) {
    case (0):
      return "0";
      break;
    case (1):
      return "Ɩ";
      break;
    case (2):
      return "ᄅ";
      break;
    case (3):
      return "Ɛ";
      break;
    case (4):
      return "ㄣ";
      break;
    case (5):
      return "ϛ";
      break;
    case (6):
      return "9";
      break;
    case (7):
      return "ㄥ";
      break;
    case (8):
      return "8";
      break;
    case (9):
      return "6";
      break;
  }
}

String invert_2(int a) {

  bool again = false;
  int value, n;
  bool negative = false;
  if (a < 0) {
    negative = true; a = -a;
  }
  String string_big = "", string_small = "";
  do {
    if (a > 9) {
      for (n = 10; n <= a ; n = 10 * n);
      value = (a * 10) / n;
      again = true;
    } else {
      value = a;
      again = false;
    }
    string_small = invert(value);
    string_big = string_small + string_big;
    a = a - (value * n) / 10;
  } while (again);
  if (negative) return string_big + "-";
  return string_big;
}

void init_buttons() {
  Blynk.setProperty(V11, "offLabel", "N∩Ⅎ ƎΛ∀H");
  Blynk.setProperty(V12, "offLabel", "HAVE FUN");
}

void init_server() {
  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.print("Use this URL to get info about the game: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
}

void init_wifi() {
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  Blynk.begin(auth, ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    Serial.flush();
  }
  Serial.println("");
  Serial.println("WiFi connected");
}

void website() {
  // Check if a client has connected
  WiFiClient client = server.available();

  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("");
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.print("<body style=background-color:grey>");

  client.print("<font color=black>");
  client.println("Welcome to the game");
  client.println("<br><hr><br>");
  client.println("Choose the mini-game by scrolling down the screen and type which one you want to play. Then scroll up to make the keyboard dissapear and fill the screen equally for both players!");
  client.println("<br><hr><br>");

  client.print("<font color=cyan>");
  client.println("1 -> Leds Race<br>") ;
  client.print("<font color=black>");
  client.print("Just like a ");
  client.print("<font color=lime>");
  client.print("traffic light");
  client.print("<font color=black>");
  client.println(". Push the button on GREEN and WIN.<br>") ;

  client.print("<font color=red>");
  client.println("2 -> Danger Color<br>") ;
  client.print("<font color=black>");
  client.println("Try not to be confused by the ");
  client.print("<font color=lime>");
  client.print("random lights");
  client.print("<font color=black>");
  client.println(". Push the button in GREEN if you wanna WIN.<br>") ;

  client.print("<font color=white>");
  client.println("3 -> Time is Counting<br>") ;
  client.print("<font color=black>");
  client.println("Decimal ");
  client.print("<font color=lime>");
  client.print("countdown");
  client.print("<font color=black>");
  client.println("! Push the button when countdown is over.<br>") ;

  client.print("<font color=yellow>");
  client.println("4 -> Memory Challenge<br>") ;
  client.print("<font color=black>");
  client.println("How many times did which color ");
  client.print("<font color=lime>");
  client.print("blink");
  client.print("<font color=black>");
  client.println("? For this mini-game is useless to press the button.<br>") ;

  client.print("<font color=blue>");
  client.println("5 -> 2 Colors<br>") ;
  client.print("<font color=black>");
  client.println("They feel alone, so when you see ");
  client.print("<font color=lime>");
  client.print("two leds with the same color");
  client.print("<font color=black>");
  client.println(", don't hesitate and press the button.<br>") ;
  client.println("<html>");
}

