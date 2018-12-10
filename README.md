# SmallFileSystem
An EEPROM File system for Arduino (SFS)<br>
With this library you can store files in Arduino's EEPROM. It's simple:<br>
First, ```#include<sfs.h>```
<br>
Storing file: 
```cpp
SFS.PutFile("file.txt", "I am a simple file!");
```
Reading file:<br>
```cpp
File f = SFS.Open("file.txt"); // Open a file
  if (!f.exists())
  {
    Serial.println("Invalid file.");
    //The file doesn't exists
    return;
  }
  Serial.println(f.Name); // Prints the file name
  while (f.available())
  {
    Serial.print((char)f.read()); // Read file char by char
  }
```
Deleting a file:<br>
```cpp
SFS.Erase("file.txt"); // Erase/Delete file.txt
```
Formating EEPROM: <br>
```cpp
SFS.Format(); // Format EEPROM
```
