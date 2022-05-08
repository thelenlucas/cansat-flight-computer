#include <EEPROM.h>

int MET_ADRESS = 1;

//Startup function
void setup() {
  int mission_elapsed_time = EEPROM.read(MET_ADRESS);
}

void Backup_Data(int MET, int GMT_ENCODED) {
  EEP
}
