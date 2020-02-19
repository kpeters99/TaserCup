#include "myEEPROM.h"

int numCont = 0;

String convertToString(char* a, int size) 
{ 
    int i; 
    String s = ""; 
    for (i = 0; i < size; i++) { 
        s = s + a[i]; 
    } 
    return s; 
} 

void clearEEPROM(){
  competitor_t data;
  int dataAddr = 0;
  EEPROM.begin(4095);
  while (!(EEPROM.get(dataAddr, data).fname != "")){
    for (int i = dataAddr ; i < sizeof(data) ; i++) {
      EEPROM.write(i, 0);
    }
    dataAddr += sizeof(data);
  }
  delay(200);
  EEPROM.commit();
  EEPROM.end();
}

void writeToEEPROM(competitor_t * data){
  int dataAddr = 0;
  EEPROM.begin(4095);
  dataAddr = numCont*sizeof(*data);
  EEPROM.put(dataAddr, *data);
  numCont++;
  delay(200);
  EEPROM.commit();
  EEPROM.end();
}

void loadEEPROM(competitor_t * dataArray){
  int dataAddr = 0;
  numCont = 0;
  competitor_t data = EEPROM.get(dataAddr, data);
  EEPROM.begin(4095);
  while (!(data.fname != "")){
    dataArray[dataAddr/sizeof(data)] = data;
    dataAddr += sizeof(data);
    data = EEPROM.get(dataAddr, data);
    numCont++;
  }
  EEPROM.end();
}

String EEPROMToHTML(competitor_t * dataArray){
  String msg = "<table style='width:100%'>";
  char buf[16];
  msg += "<tr>";
  msg +=   "<th>Name</th>";
  msg +=   "<th>InputTime</th>";
  msg += "</tr>";
  
  doQuickSort(dataArray, 1, numCont);

  for(int i = 0; i<numCont; i++){
    ltoa(dataArray[i].inputTime,buf,10);
    msg += "<tr>";
    msg += "<td>" + convertToString(dataArray[i].fname, 16) + "<\td>";
    msg += "<td>" + convertToString(buf, 16) + "<\td>";
    msg += "<\tr>";
  }

  msg += "<\t>";
  return msg;
  
}
