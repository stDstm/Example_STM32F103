#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <FS.h>

#define WRITE_ADDR 0x08000000 // адрес для заливки прошивки
#define SIZE_WRITE 256
#define FIRMWARE "/esptostm.bin" // перед именем должен стоять слеш

#define BOOT_PIN D5
#define RESET_PIN D6

#define Usart_stm32 Serial
//#define DBG_OUTPUT_PORT Serial1

const char* ssid = "istarik";
const char* password = "SsghDfhfhfhfs";
const char* host = "esptostm32"; // адрес - http://esptostm32.local/

ESP8266WebServer server(80);
File fsUploadFile;

String getContentType(String filename) 
{
  if (server.hasArg("download")) 
  {
    return "application/octet-stream";
  } 
  else if (filename.endsWith(".htm")) 
  {
    return "text/html";
  } 
  else if (filename.endsWith(".html")) 
  {
    return "text/html";
  } 
  else if (filename.endsWith(".css")) 
  {
    return "text/css";
  } 
  else if (filename.endsWith(".js")) 
  {
    return "application/javascript";
  } 

  return "text/plain";
}

bool handleFileRead(String path) 
{
  //DBG_OUTPUT_PORT.println("handleFileRead: " + path);
  
  if (path.endsWith("/")) 
  {
    path += "edit.htm";
  }
  
  String contentType = getContentType(path);
  
  if(SPIFFS.exists(path)) 
  {   
    File file = SPIFFS.open(path, "r");
    server.streamFile(file, contentType);
    file.close();
    return true;
  }
  return false;
}

void handleFileUpload() 
{
  if(server.uri() != "/edit") return;

  HTTPUpload& upload = server.upload();

  if(upload.status == UPLOAD_FILE_START) 
  {
    String filename = upload.filename;
    
    if(!filename.startsWith("/")) filename = "/" + filename;
    
    //DBG_OUTPUT_PORT.print("handleFileUpload Name: "); 
    //DBG_OUTPUT_PORT.println(filename);
    
    fsUploadFile = SPIFFS.open(filename, "w");
    filename = String();
    
  } 
  else if(upload.status == UPLOAD_FILE_WRITE) 
  {
    if(fsUploadFile) fsUploadFile.write(upload.buf, upload.currentSize);
  } 
  else if(upload.status == UPLOAD_FILE_END) 
  {
    if(fsUploadFile) fsUploadFile.close();
    
    //DBG_OUTPUT_PORT.print("handleFileUpload Size: "); 
    //DBG_OUTPUT_PORT.println(upload.totalSize);
  }
}

void handleFileDelete() 
{
  if(server.args() == 0) return server.send(500, "text/plain", "BAD ARGS");
  
  String path = server.arg(0);
  
  //DBG_OUTPUT_PORT.println("handleFileDelete: " + path);
  
  if(path == "/") return server.send(500, "text/plain", "BAD PATH");
  
  if(!SPIFFS.exists(path)) return server.send(404, "text/plain", "FileNotFound");
  
  SPIFFS.remove(path);
  server.send(200, "text/plain", "");
  path = String();
}

void handleFileList() 
{
  if (!server.hasArg("dir")) 
  {
    server.send(500, "text/plain", "BAD ARGS");
    return;
  }

  String path = server.arg("dir");
  //DBG_OUTPUT_PORT.println("handleFileList: " + path);
  Dir dir = SPIFFS.openDir(path);
  path = String();

  String output = "[";
  while (dir.next()) 
  {
    File entry = dir.openFile("r");
    
    if(output != "[") output += ',';
    
    bool isDir = false;
    output += "{\"type\":\"";
    output += (isDir) ? "dir" : "file";
    output += "\",\"name\":\"";
    output += String(entry.name()).substring(1);
    output += "\"}";
    entry.close();
  }

  output += "]";
  server.send(200, "text/json", output);
}

///////////////////////////////////// Entr_bootloader //////////////////////////////////////////
void handleEntr_bootloader() 
{
  //DBG_OUTPUT_PORT.printf("Entr_bootloader\n");
  entr_bootloader();
}

///////////////////////////////////// Write //////////////////////////////////////////
void handleWrite() 
{
  //DBG_OUTPUT_PORT.printf("Write\n");
  write_memory();
}

///////////////////////////////////// Reset //////////////////////////////////////////
void handleReset() 
{
  //DBG_OUTPUT_PORT.printf("Reset\n");
  on_reset();
  server.send(200, "text/plain", "Reset - OK");  
}

///////////////////////////////////// On_boot //////////////////////////////////////////
void handleOn_boot() 
{
  //DBG_OUTPUT_PORT.printf("On_boot\n");
  on_boot();
  server.send(200, "text/plain", "On boot - OK");
}

///////////////////////////////////// Off_boot //////////////////////////////////////////
void handleOff_boot() 
{
  //DBG_OUTPUT_PORT.printf("Off_boot\n");
  off_boot();
  server.send(200, "text/plain", "Off boot - OK");
}

///////////////////////////////////// Event //////////////////////////////////////////
void handleEvent() 
{
  // что-то делаем
  server.send(200, "text/plain", "Event - OK");
}

//////////////////////////////////////////////////////////////////////////////////////////


void setup(void) 
{
  pinMode(BOOT_PIN, OUTPUT); // boot_0 
  digitalWrite(BOOT_PIN, LOW);
  pinMode(RESET_PIN, OUTPUT); // reset
  digitalWrite(RESET_PIN, LOW);

  Usart_stm32.begin(57600, SERIAL_8E1);
  
  //DBG_OUTPUT_PORT.begin(57600);
  //DBG_OUTPUT_PORT.print("\n");
  //DBG_OUTPUT_PORT.setDebugOutput(true);
  
  /*SPIFFS.begin();
  {
    Dir dir = SPIFFS.openDir("/");
    
    while (dir.next()) 
    {
      String fileName = dir.fileName();
      uint16_t fileSize = dir.fileSize();
      DBG_OUTPUT_PORT.printf("FS File: %s, size: %d\n", fileName.c_str(), fileSize);
    }
    
    DBG_OUTPUT_PORT.printf("\n");
  }

  //WIFI INIT
  //DBG_OUTPUT_PORT.printf("Connecting to %s\n", ssid);
  
  if(String(WiFi.SSID()) != String(ssid)) 
  {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
  }*/

  SPIFFS.begin();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  while(WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    //DBG_OUTPUT_PORT.print(".");
  }
  
  //DBG_OUTPUT_PORT.println("");
  //DBG_OUTPUT_PORT.print("Connected! IP address: ");
  //DBG_OUTPUT_PORT.println(WiFi.localIP());

  MDNS.begin(host);
  //DBG_OUTPUT_PORT.print("Open http://");
  //DBG_OUTPUT_PORT.print(host);
  //DBG_OUTPUT_PORT.println(".local/edit to see the file browser");

  //SERVER INIT
  server.on("/list", HTTP_GET, handleFileList);
  server.on("/enter_boot", HTTP_GET, handleEntr_bootloader);
  server.on("/write", HTTP_GET, handleWrite);
  server.on("/reset", HTTP_GET, handleReset);
  server.on("/on_boot", HTTP_GET, handleOn_boot);
  server.on("/off_boot", HTTP_GET, handleOff_boot);
  server.on("/event", HTTP_GET, handleEvent);
  server.on("/edit", HTTP_DELETE, handleFileDelete);
  
  server.on("/edit", HTTP_GET, []() 
  {
    if(!handleFileRead("/edit.htm")) server.send(404, "text/plain", "FileNotFound");
  });
  
  server.on("/edit", HTTP_POST, []() 
  {
    server.send(200, "text/plain", "");
  }, handleFileUpload);

  server.onNotFound([]() 
  {
    if(!handleFileRead(server.uri())) server.send(404, "text/plain", "FileNotFound");
  });

  server.begin();
  //DBG_OUTPUT_PORT.println("HTTP server started");
}

void loop(void) 
{
  server.handleClient();
}

//////////////////////////// entr_bootloader /////////////////////////////////////
void entr_bootloader()
{
  on_off_boot(HIGH); // подтягиваем BOOT_0 к плюсу
  delay(500);
  on_reset(); // нажимаем ресет
  delay(200);

  while(Usart_stm32.available()) { Usart_stm32.read(); } // в приёмном буфере может быть мусор

  Usart_stm32.write(0x7F); // первый запрос (для определения скорости) 
  if(ack_byte() == 0) 
  {
    //DBG_OUTPUT_PORT.println(F("Bootloader - OK"));  // Log
    server.send(200, "text/plain", "Bootloader - OK");
  }
  else 
  {
    //DBG_OUTPUT_PORT.println(F("Bootloader - ERROR"));  // Log  
    server.send(200, "text/plain", "Bootloader - ERROR");
  }
}

//////////////////////////// write_memory /////////////////////////////////////
void write_memory()
{
  if(erase_memory() == 0)
  {
     File df = SPIFFS.open(FIRMWARE, "r");
     
     if(df) 
     {
       uint32_t size_file = df.size();
      
       //DBG_OUTPUT_PORT.print(F("Size file "));
       //DBG_OUTPUT_PORT.println(size_file);
     
       uint8_t cmd_array[2] = {0x31, 0xCE}; // код Write Memory 
       uint32_t count_addr = 0;
       uint16_t len = 0;
       uint32_t seek_len = 0;

       while(true)
       {      
         if(send_cmd(cmd_array) == 0)
         {
           uint8_t ret_adr = send_adress(WRITE_ADDR + count_addr);
           count_addr = count_addr + SIZE_WRITE;
           
           if(ret_adr == 0)
           {       
             uint8_t write_buff[SIZE_WRITE] = {0,};
             len = df.read(write_buff, SIZE_WRITE);
             seek_len++;
             df.seek(SIZE_WRITE * seek_len);  
                     
             uint8_t cs, buf[SIZE_WRITE + 2];
             uint16_t i, aligned_len;
              
             aligned_len = (len + 3) & ~3;
             cs = aligned_len - 1;
             buf[0] = aligned_len - 1;
              
             for(i = 0; i < len; i++) 
             {
               cs ^= write_buff[i];
               buf[i + 1] = write_buff[i];
             }
              
             for(i = len; i < aligned_len; i++) 
             {
               cs ^= 0xFF;
               buf[i + 1] = 0xFF;
             }
              
             buf[aligned_len + 1] = cs;
             Usart_stm32.write(buf, aligned_len + 2);
             uint8_t ab = ack_byte();
          
             if(ab != 0) 
             {
               //DBG_OUTPUT_PORT.println(F("Block not Write Memory - ERROR"));
               server.send(200, "text/plain", "Block not Write Memory - ERROR");
               break;
             }
             
             if(size_file == df.position())
             {
               Usart_stm32.begin(57600, SERIAL_8N1);
               while(Usart_stm32.available())
               {
                 Usart_stm32.read();
                 delay(1);
               } 

               boot_off_and_reset();
               String reciv = "<br/>";
             
               for(uint16_t i = 0; i < 2000; i++)
               {
                 if(Usart_stm32.available())
                 {
                   while(Usart_stm32.available())
                   {
                     char c = Usart_stm32.read();
                     reciv += c;
                     delay(1);
                   } 
                   break;      
                 } 
                 delay(1);  
               }

               server.send(200, "text/plain", "Done Write Memory - OK" + reciv); //DBG_OUTPUT_PORT.println(F("Done Write Memory - OK"));
               Usart_stm32.begin(57600, SERIAL_8E1);
               break;
             }                 
           }
           else 
           {
             server.send(200, "text/plain", "Address Write Memory - ERROR"); //DBG_OUTPUT_PORT.println(F("Address Write Memory - ERROR")); // Log
             break;
           }
         }
         else 
         {
           server.send(200, "text/plain", "Cmd cod Write Memory - ERROR"); //DBG_OUTPUT_PORT.println(F("Cmd cod Write Memory - ERROR")); // Log
           break;
         }             
       } // end while
       
       df.close();
     }
     else server.send(200, "text/plain", "Not file - ERROR"); //DBG_OUTPUT_PORT.println(F("Not file - ERROR"));          
  } 
  else server.send(200, "text/plain", "Not erase Write Memory - ERROR"); //DBG_OUTPUT_PORT.println(F("Not erase Write Memory - ERROR"));    
}

////////////////////////////// erase_memory ////////////////////////////////////
uint8_t erase_memory()
{
  uint8_t cmd_array[2] = {0x43, 0xBC}; // команда на стирание 

  if(send_cmd(cmd_array) == 0) 
  {
    uint8_t cmd_array[2] = {0xFF, 0x00}; // код стирания (полное) 

    if(send_cmd(cmd_array) == 0)
    {
      //DBG_OUTPUT_PORT.println(F("Erase Memory - OK")); // Log
      return 0;
    }
    //else DBG_OUTPUT_PORT.println(F("Cmd cod Erase Memory - ERROR")); // Log             
  }
  //else DBG_OUTPUT_PORT.println(F("Cmd start Erase Memory - ERROR")); // Log

  return 1;
}

////////////////////////////// send_cmd ////////////////////////////////////
uint8_t send_cmd(uint8_t *cmd_array)
{
  Usart_stm32.write(cmd_array, 2); 
  if(ack_byte() == 0) return 0;
  else return 1;
}

/////////////////////////////// ack_byte ////////////////////////////////////
uint8_t ack_byte()
{ 
  for(uint16_t i = 0; i < 500; i++)
  {
    if(Usart_stm32.available())
    {
      uint8_t res = Usart_stm32.read();
      if(res == 0x79) return 0;         
    } 
    delay(1);  
  }

  return 1;
}

///////////////////////////// send_adress ////////////////////////////////////
uint8_t send_adress(uint32_t addr)
{
  uint8_t buf[5] = {0,};  
  buf[0] = addr >> 24;
  buf[1] = (addr >> 16) & 0xFF;
  buf[2] = (addr >> 8) & 0xFF;
  buf[3] = addr & 0xFF;
  buf[4] = buf[0] ^ buf[1] ^ buf[2] ^ buf[3];
  
  Usart_stm32.write(buf, 5);
  if(ack_byte() == 0) return 0;
  else return 1;  
}


////////////////////////////// on_boot ///////////////////////////////////
void on_boot()
{
  digitalWrite(BOOT_PIN, HIGH); 
}

////////////////////////////// off_boot ///////////////////////////////////
void off_boot()
{
  digitalWrite(BOOT_PIN, LOW); 
  on_reset();
}  

////////////////////////////// on_reset //////////////////////////////////////
void on_reset()
{
  digitalWrite(RESET_PIN, HIGH); // reset
  delay(50);    
  digitalWrite(RESET_PIN, LOW); 
}

////////////////////////////// on_off_boot ///////////////////////////////////
void on_off_boot(uint8_t i)
{
  digitalWrite(BOOT_PIN, i); 
}

//////////////////////////// boot_off_and_reset //////////////////////////////
void boot_off_and_reset()
{
  on_off_boot(LOW);
  delay(500);
  on_reset(); 
  //DBG_OUTPUT_PORT.println(F("Boot off and reset")); // Log
}
















