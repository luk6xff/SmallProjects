#include "mbed.h"
#include "hm11.h"
#include "crc16.h"
#include <cstring>

#define HM11_PIN_TX PTE22 //PTC4   // FRDM-KL25Z UART2 //PTE22  //YELLOW wire 
#define HM11_PIN_RX PTE23//PTC3   //                  //PTE23  //ORANGE wire


#define CRC_TEST_ENABLED_0
#define SIMPLE_DATA_TEST_ENABLED_0
#define MSG1_DATA_TEST_ENABLED

#define MSG1_BROADCAST_TEST_ENABLED


//DigitalOut myled(LED1);



//TEST ENVIRONMENT FOR OUR MODULAR DEVICE //
//BROADCAST MESSAGE
//e.g 0x42 0x4C 0x45 0x01 0x45 0x53 0x45 0x06 0x50 0x6C 0x64 0x1E 0x4C 0x42 0x43 0x05 0x4C 0x68 0x74 0x02
//       bleHeader        Sensor module code      Led module         LiPol module    User interface module
//broadcast_msg1[] - blemodule, environmental sensor  
const uint8_t broadcast_msg1[]={0x42, 0x4C, 0x45, 0x01, 0x45, 0x53, 0x45, 0x06, 0x50, 0x6C, 0x64, 0x1E, 0x4C, 0x42, 0x43, 0x05,0x4C, 0x68 ,0x74 ,0x02};
//7C 41 45 53 4E 00 00 00 00 00 00 00 00 00 16 02 63 13 EF FF A5 00 84 FE 5F 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 01 DA 00 DD 00 D0 00 22 04 86 01 2B 00 00 00 00 00 00 00 00 00 00 D1 0F
const uint8_t data_msg1[]={0x45, 0x53, 0x4E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16, 0x02, 0x63, 0x13, 0xEF, 0xFF, 0xA5, 0x00, 0x84, 0xFE, 0x5F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xDA, 0x00, 0xDD, 0x00, 0xD0, 0x00, 0x22, 0x04, 0x86, 0x01, 0x2B, 0x00, 0x00, 0x00, 0x00 ,0x00 ,0x00, 0x00, 0x00, 0x00, 0x00};

typedef enum TypeOfMessage{
        MSG_BROADCAST,
        MSG_DATA,
        MSG_UNKNOWN
}TypeOfMessage_t;

static const uint8_t *parseDataFrameToSend(const uint8_t* data_msg, uint8_t dataLength, TypeOfMessage_t msgType,uint8_t* wholeFrameLength){
    
    uint8_t lengthCrc= 2;
    uint8_t StartByteAndDataLength =2;
    *wholeFrameLength= dataLength+lengthCrc+StartByteAndDataLength+1; //+1 for NULL
    uint16_t crc = computeCRC16((const uint8_t *)data_msg, dataLength);
       
    uint8_t* ptr= new uint8_t[*wholeFrameLength];
    if(!ptr||msgType>=MSG_UNKNOWN)
        return NULL;
    ptr[0]='|';
    if(msgType==MSG_BROADCAST)
        ptr[1]=dataLength&0x7F;
    else if(msgType==MSG_DATA)
        ptr[1]=dataLength|0x80;
    memcpy(&ptr[2],data_msg,dataLength);
    ptr[*wholeFrameLength-3]=((crc>>8)&0xFF);
    ptr[*wholeFrameLength-2]=((crc)&0xFF);
    ptr[*wholeFrameLength-1]=NULL;
    return ptr;   
}

int main() {
    Serial usbDebug(USBTX, USBRX);
    usbDebug.printf("HELLO WORLD !");  
    uint8_t allDataLength= 0;

    HM11* hm11 = new HM11( HM11_PIN_TX, HM11_PIN_RX);
   // myled = 0;
    int counter =0;
    while(1) {
        //myled = 1;
        wait(0.5);
        usbDebug.printf("alive ");   
        wait(0.5);
        
#ifdef CRC_TEST_ENABLED
        const uint8_t crcBuf[]= {0x42, 0x4C, 0x45};
        uint8_t crcBufLen= sizeof(crcBuf)/sizeof(crcBuf[0]);
        usbDebug.printf("CRC_VAL:  %d\r\n",(int)computeCRC16(crcBuf, crcBufLen));
#endif

#ifdef SIMPLE_DATA_TEST_ENABLED
        char buf[2];
        snprintf(buf,2,"%d",counter++);
        if(counter>9)
            counter=0;
        hm11->sendDataToDevice(buf);
        wait(0.2);
        
         while(hm11->isRxDataAvailable())                    
               usbDebug.printf("data:  %c\r\n",hm11->getDataFromRx());
#endif          


#ifdef MSG1_DATA_TEST_ENABLED
         
        const uint8_t* dataPtr= parseDataFrameToSend(data_msg1,sizeof(data_msg1)/sizeof(data_msg1[0]),MSG_DATA,&allDataLength);
        hm11->sendDataToDevice(dataPtr,allDataLength);
        usbDebug.printf("dataLength:  %d\r\n",allDataLength);
        delete[] dataPtr;
        wait(1);

#endif    
 
#ifdef  MSG1_BROADCAST_TEST_ENABLED
        const uint8_t* dataPtrB= parseDataFrameToSend(broadcast_msg1,sizeof(broadcast_msg1)/sizeof(broadcast_msg1[0]),MSG_BROADCAST, &allDataLength);
        hm11->sendDataToDevice(dataPtrB, allDataLength);
        delete[] dataPtrB;
        wait(1);


#endif 
    }
}
