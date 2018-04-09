#include "lorawan_client.h"
#define ECHO(str) if(echo){Serial.print(str);}
#define ECHOLN(str) if(echo){Serial.println(str);}

LoRaWANClient::LoRaWANClient() {
  ss.begin(9600);
  ss.setTimeout(SERIAL_WAIT_TIME);
}

bool LoRaWANClient::connect(bool force_reconnect){
  int waitTime=INIT_WAIT_TIME;
  String cmd;
  bool echo=DEBUG;

  ss.listen();

  sendCmd("\n", "",NULL, false);
  while (ss.available() > 0) {
    char ch = ss.read();
    Serial.print(ch);
  }

  if(!force_reconnect) {
    ECHOLN("Checking if already joined or not ... ");
    if (sendCmd("lorawan get_join_status", "joined", NULL, DEBUG, waitTime)) {
      ECHOLN("already joined.");
      return true;
    }
    ECHOLN("unjoined.");
  } else {
      //
      // LoRa module status clear
      //
      Serial.println("Initializing LoRa module Flash");
      if (!sendCmd("mod factory_reset", "Ok", NULL, DEBUG, waitTime)) {
        Serial.println("'factory_reset' Request Failed");
        return false;
      }
      if (!sendCmd("mod set_echo off", "Ok", NULL, DEBUG, waitTime)) {
        Serial.println("'set_echo off' Request Failed");
        return false;
      }
      if (!sendCmd("mod save", "Ok", NULL, DEBUG, waitTime)) {
        Serial.println("'save' Request Failed");
        return false;
      }
      if (!sendCmd("mod reset", "", NULL, DEBUG, waitTime)) {
        Serial.println("'reset' Request Failed");
        return false;
      }
  }

  //
  // LoRa module various value get
  //

  if (!sendCmd("mod get_hw_model", "", NULL, true, waitTime)) {
    Serial.println("Request Failed");
    return false;
  }
  if (!sendCmd("mod get_ver", "", NULL, true, waitTime)) {
    Serial.println("Request Failed");
    return false;
  }
  if (!sendCmd("lorawan get_deveui", "", NULL, true, waitTime)) {
    Serial.println("Request Failed");
    return false;
  }

  // LoRa module join to Network Server by OTAA
  //
  int retry=0;
  while (!sendCmd("lorawan join otaa", "accepted", NULL, DEBUG, JOIN_RETRY_INTERVAL)) {
    retry++;
    /*
    delay(2200);
    if (sendCmd("lorawan get_join_status", "joined", NULL, DEBUG, waitTime)) {
      return true;
    }
    */
    Serial.print("'lorawan join otaa' retried (");
    Serial.print(retry);
    Serial.print("/");
    if(!force_reconnect){
        Serial.print(JOIN_RETRY_MAX);
        Serial.println(")");
        if(retry >= JOIN_RETRY_MAX ){
          Serial.println("Exceeded JOIN_RETRY_MAX attempts.");
          return false;
        }
    } else Serial.print("-)");
  }
  return true;
}

bool LoRaWANClient::sleep(void){
  int waitTime = INIT_WAIT_TIME;
  if (!sendCmd("mod sleep 0 1 0", "", NULL, DEBUG, waitTime)) {
    Serial.println("'sleep' Request Failed");
    delay(16);
    return false;
  }
  return true;
}

bool LoRaWANClient::deep_sleep(void){
  int waitTime=INIT_WAIT_TIME;
  if (!sendCmd("mod sleep 1 1 0", "", NULL, DEBUG, waitTime)) {
    Serial.println("'deep_sleep' Request Failed");
    delay(16);
    return false;
  }
  return true;
}

bool LoRaWANClient::sendCmd(String cmd, String waitStr, CALLBACK p, bool echo, int waitTime){
  unsigned long tim;
  String str;

  ECHO("sendCmd: ");
  ECHOLN(cmd);
  ss.listen();
  ss.print(cmd);
  ss.print('\r');
  delay(100);
  tim = millis() + waitTime - 100;
  while (millis() < tim) {
    if(ss.available() > 0) {
      char ch = ss.read();
      ECHO(ch);
      str += String(ch);
      checkRx(&str, p);
      if (commandCompleted(cmd, str)) {
        break;
      }
    }
  }
  if (waitStr == NULL) return true;
  if (str.indexOf(waitStr) >= 0) return true;

  return false;
}

void LoRaWANClient::checkRx(String* rsp, CALLBACK p) {
  if (!rsp || !p) {
    return;
  }

  int rxIdx = rsp->indexOf("> rx");
  if (rxIdx >= 0) {
    String str = rsp->substring(rxIdx + 5 /* length of "> rx " */);
    int crIdx = str.indexOf("\n");
    if (crIdx >= 0) {
      p(str.substring(str.indexOf(" ") + 1, crIdx));
      rsp->remove(0, rxIdx);
      rsp->remove(0, crIdx);
    }
  }
}

bool LoRaWANClient::commandCompleted(String cmd, String rsp) {
  if (cmd.indexOf("lorawan tx") < 0) {
    return rsp.indexOf("\n> ") >= 0;
  }
  return (rsp.indexOf("> tx_ok") >= 0 || rsp.indexOf("> err") >= 0);
}

bool LoRaWANClient::sendData(char *data, short port, CALLBACK p, bool echo){
  int i,j;
  char cmdLine[32];
  char payload[MAX_PAYLOAD_SIZE*2+1]="";
  char tmp[3]="";

  ECHO("sending '");
  ECHO(data);
  ECHO("' to port ");
  ECHOLN(port);

  for(i=0, j=0; i< MAX_PAYLOAD_SIZE ; i++, j+=2)
  {
    if(data[i] == 0x00)
    {
      payload[j] = 0x00;
      break;
    }
    sprintf(tmp,"%0x",data[i]);
    strcat(payload, tmp);
  }

  sprintf(cmdLine, "lorawan tx ucnf %d %s", port, payload);
  ECHOLN(cmdLine);

  if(sendCmd(cmdLine, "tx_ok", p, echo, NETWORK_WAIT_TIME))
  {
    ECHOLN(" ... sent.");
    return true;
  }
  else
  {
    ECHOLN(" ... failed.");
    Serial.println("'sendData(char*)' failed");
    return false;
  }
}

bool LoRaWANClient::sendData(unsigned long data, short port, CALLBACK p, bool echo){
  int i,j;
  char cmdLine[32];
  char tmp[3]="";

  ECHO("sending '");
  ECHO(data);
  ECHO("' to port ");
  ECHOLN(port);

  sprintf(cmdLine, "lorawan tx ucnf %d %08lx", port, data);
//  ECHOLN(cmdLine);

  if(sendCmd(cmdLine, "tx_ok", p, echo, NETWORK_WAIT_TIME))
  {
    ECHOLN(" ... sent.");
    return true;
  }
  else
  {
    ECHOLN(" ... failed.");
    Serial.println("'sendData(long)' failed");
    return false;
  }
}

bool LoRaWANClient::sendBinary(byte *data_pointer, int data_size, short port, CALLBACK p, bool echo){
  char cmdLine[MAX_PAYLOAD_SIZE*2+30], tmp[]="00";
  int i;
  byte *b;
  b=data_pointer;
  if (data_size > MAX_PAYLOAD_SIZE)
  {
    ECHO("ERROR: size of data (");
    ECHO(data_size);
    ECHOLN(" bytes) too big. ");
    return false;
  }

  sprintf(cmdLine, "lorawan tx ucnf %d ", port);
  for(i=0;i<data_size;i++,b++)
  {
    sprintf(tmp, "%02x", *b );
    strcat(cmdLine, tmp);
  }
  ECHOLN(cmdLine);
  if(sendCmd(cmdLine, "tx_ok", p, echo, NETWORK_WAIT_TIME))
  {
    ECHOLN(" ... sent.");
    return true;
  }
  else
  {
    ECHOLN(" ... failed.");
    Serial.println("'sendBinary' failed");
    return false;
  }

  return true;
}
