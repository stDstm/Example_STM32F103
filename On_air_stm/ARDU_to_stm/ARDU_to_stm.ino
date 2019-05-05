#include <SPI.h>
#include <SD.h>

#define Usart_stm32 Serial1
#define WRITE_ADDR 0x08000000 // адрес для заливки прошивки
#define SIZE_WRITE 256
#define FIRMWARE "usart3.bin"

#define BOOT_PIN 5
#define RESET_PIN 6

void setup() 
{
  Serial.begin(57600);
  Usart_stm32.begin(57600, SERIAL_8E1);

  pinMode(BOOT_PIN, OUTPUT); // boot_0 
  digitalWrite(BOOT_PIN, LOW);
  pinMode(RESET_PIN, OUTPUT); // reset
  digitalWrite(RESET_PIN, LOW); 

  if(!SD.begin(53)) Serial.println(F("Card failed - ERROR"));
  else Serial.println(F("Card initialized - OK"));
}


void loop() 
{
  if(Serial.available()) 
  {
    uint8_t res = Serial.read();

    /////////////// первый запрос для установки скорости (в ответ ждёт 0x79 - ACK-байт)///////////////
    if(res == 'B') 
    {
      entr_bootloader();
    }

    ////////////////////////////////// boot_off_and_reset /////////////////////////////////////
    else if(res == 'R') 
    {
      boot_off_and_reset();
    }  
    
    ////////////////////////////////// Erase Memory /////////////////////////////////////
    else if(res == 'E') 
    {
      erase_memory();
    }   

    ///////////////////////////////////// Write_Memory ////////////////////////////////////////
    else if(res == 'W') 
    {
      write_memory();
    }
    
    ///////////////////////////////////// on_boot() ////////////////////////////////////////
    else if(res == 'h') 
    {
      on_boot();
    }

    ///////////////////////////////////// off_boot() ////////////////////////////////////////
    else if(res == 'l') 
    {
      off_boot();
    }

    ///////////////////////////////////// on_reset() ////////////////////////////////////////
    else if(res == 'r') 
    {
      on_reset();
    }     
  }
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
  if(ack_byte() == 0) Serial.println(F("Bootloader - OK"));  // Log
  else Serial.println(F("Bootloader - ERROR"));  // Log  
}

//////////////////////////// write_memory /////////////////////////////////////
void write_memory()
{
  if(erase_memory() == 0)
  {
     File df = SD.open(FIRMWARE);
     
     if(df) 
     {
       uint32_t size_file = df.size();
      
       Serial.print(F("Size file "));
       Serial.println(size_file);
     
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
             //write_memory(write_buff, len);file.position()

             //Serial.print(F("seek_len "));
             //Serial.println(seek_len);
             //Serial.println(df.position());
              
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
               Serial.println(F("Block not Write Memory - ERROR"));
               break;
             }
             
             if(size_file == df.position())
             {
               Serial.println(F("End Write Memory2 - OK"));
               boot_off_and_reset();
               break;
             }                 
           }
           else 
           {
             Serial.println(F("Address Write Memory - ERROR")); // Log
             break;
           }
         }
         else 
         {
           Serial.println(F("Cmd cod Write Memory - ERROR")); // Log
           break;
         }             
       } // end while
       
       df.close();
     }
     else Serial.println(F("Not file - ERROR"));          
  } 
  else Serial.println(F("Not erase Write Memory - ERROR"));    
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
      Serial.println(F("Erase Memory - OK")); // Log
      return 0;
    }
    else Serial.println(F("Cmd cod Erase Memory - ERROR")); // Log             
  }
  else Serial.println(F("Cmd start Erase Memory - ERROR")); // Log

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
  Serial.println(F("Boot off and reset")); // Log
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
}






