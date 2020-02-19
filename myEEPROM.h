#include <EEPROM.h>
#include <SPI.h>
#include "myTypes.h"
#include "quickSort.h"

String EEPROMToHTML(competitor_t * dataArray);
void writeToEEPROM(competitor_t * data);
void clearEEPROM();
void loadEEPROM(competitor_t *dataArary);
