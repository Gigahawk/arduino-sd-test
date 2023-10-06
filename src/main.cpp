#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

Sd2Card card;
SdVolume volume;
SdFile root;

const int chipSelect = 5;
const int bytesToWrite = 2048;
const char* fileName = "test.txt";

void prompt(const char* msg) {
  Serial.println(msg);
  while (true) {
    char c = Serial.read();
    if (c == '\n' || c == '\r')
      break;
  }
}

bool initializeCardSd2Card() {
  if (!card.init(SPI_HALF_SPEED, chipSelect))
  {
    Serial.println("initialization failed. Things to check:");
    Serial.println("* is a card inserted?");
    Serial.println("* is your wiring correct?");
    Serial.println("* did you change the chipSelect pin to match your shield or module?");
    return false;
  }
  else
  {
    Serial.println("Wiring is correct and a card is present.");
  }
  Serial.println();
  return true;
}

bool initializeCardSd() {
  if(!SD.begin(chipSelect)) {
    Serial.println("Initialization failed");
    return false;
  } else {
    Serial.println("Init OK");
  }
  Serial.println();
  return true;
}

bool printCardInfo()
{
  Serial.print("Card type:         ");
  switch (card.type())
  {
  case SD_CARD_TYPE_SD1:
    Serial.println("SD1");
    break;
  case SD_CARD_TYPE_SD2:
    Serial.println("SD2");
    break;
  case SD_CARD_TYPE_SDHC:
    Serial.println("SDHC");
    break;
  default:
    Serial.println("Unknown");
  }

  // Now we will try to open the 'volume'/'partition' - it should be FAT16 or FAT32
  if (!volume.init(card))
  {
    Serial.println("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card");
    return false;
  }

  Serial.print("Clusters:          ");
  Serial.println(volume.clusterCount());

  Serial.print("Blocks x Cluster:  ");
  Serial.println(volume.blocksPerCluster());

  Serial.print("Total Blocks:      ");
  Serial.println(volume.blocksPerCluster() * volume.clusterCount());

  Serial.println();

  // print the type and size of the first FAT-type volume
  uint32_t volumesize;
  Serial.print("Volume type is:    FAT");
  Serial.println(volume.fatType(), DEC);

  volumesize = volume.blocksPerCluster(); // clusters are collections of blocks
  volumesize *= volume.clusterCount(); // we'll have a lot of clusters
  volumesize /= 2; // SD card blocks are always 512 bytes (2 blocks are 1KB)
  Serial.print("Volume size (Kb):  ");
  Serial.println(volumesize);

  Serial.print("Volume size (Mb):  ");
  volumesize /= 1024;
  Serial.println(volumesize);

  Serial.print("Volume size (Gb):  ");
  Serial.println((float)volumesize / 1024.0);

  Serial.println("\nFiles found on the card (name, date and size in bytes): ");
  root.openRoot(volume);
  // list all files in the card with date and size
  root.ls(LS_R | LS_DATE | LS_SIZE);
  root.close();
  Serial.println();
  return true;
}

bool writeToCard() {
  randomSeed(millis());
  char c;
  Serial.print("Opening ");
  Serial.print(fileName);
  Serial.println(" for writing");
  File file = SD.open(fileName, O_READ | O_WRITE | O_CREAT | O_TRUNC);
  if (file) {
    Serial.println("File opened successfully");
  } else {
    Serial.println("Failed to open file");
    return false;
  }
  Serial.println("Writing data");
  for(int i=0; i < bytesToWrite; i++) {
    //c = (char)random(65, 91);
    c = (char)i;
    Serial.print(c);
    file.write((uint8_t)c);
  }
  file.write((uint8_t)0);
  file.close();
  Serial.println();
  Serial.println("Done writing");
  Serial.println();
  return true;
}

bool readFromCard() {
  Serial.print("Opening ");
  Serial.print(fileName);
  Serial.println(" for reading");
  File file = SD.open(fileName, FILE_READ);
  if (file) {
    Serial.println("File opened successfully");
  } else {
    Serial.println("Failed to open file");
    return false;
  }
  Serial.println("Reading data");
  while(file.available()) {
    Serial.write(file.read());
  }
  file.close();
  Serial.println();
  Serial.println("Done reading");
  Serial.println();
  return true;
}

void setup() {
  Serial.begin(9600);
  while (!Serial);
  prompt("Press enter to start test");
}

void loop() {
  Serial.println("Start of SD test");
  prompt("Press enter to begin card initialization with the Sd2Card lib");
  if(!initializeCardSd2Card())
    return;
  prompt("Press enter to begin card initialization with the SD lib");
  if(!initializeCardSd())
    return;
  prompt("Press enter to query for card info");
  if(!printCardInfo())
    return;
  prompt("Press enter to write a file to the card");
  if(!writeToCard())
    return;
  prompt("Press enter to read the file from the card");
  if(!readFromCard())
    return;
}
