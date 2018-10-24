Dear Tao Gong, 
 
In reply to your message regarding Service Request SR 1-947156663:
 
First of all, sorry for my so later reply.
 
I would recommend customer to refer attached Kinetis 50MHz UART module validation code, which includes UART RX/TX interrupt test code.
And I also recmmend customer try to use CodeWarrior Processor Expert tool, which provides example about using Serial_LDD to generate UART Block reception/transmission, with interrupt service.
Please check below Processor Expert help info:
Block reception/transmission, with interrupt service
The most of applications use a serial communication in the interrupt mode when an application is asynchronously notified by a driver about transmission/reception events.
 
The following example demonstrates a simple "Hello world" application. The program waits until 'e' character is received and then sends text "Hello world".
 
Required component setup :
 
Interrupt service/event : Enabled 
DataWidth : 8 bits 
Receiver : Enabled 
Transmitter : Enabled 
Enabled in init. code : yes 
Methods to enable : SendBlock, ReceiveBlock 
Events to enable : OnBlockReceived 
 
 
Content of ProcessorExpert.c:
 
volatile bool DataReceivedFlg = FALSE;
char OutData[] = "Hello world";
char InpData[10];
LDD_TError Error;
LDD_TDeviceData *MySerialPtr;
 
void main(void)
{
  . . .
  MySerialPtr = AS1_Init(NULL);                                     /* Initialization of AS1 component */
  for(;;) {
    Error = AS1_ReceiveBlock(MySerialPtr, InpData, 1U);             /* Start reception of one character */
    while (!DataReceivedFlg) {                                      /* Wait until 'e' character is received */
    }
    if (InpData[0] == 'e') {
      Error = AS1_SendBlock(MySerialPtr, OutData, sizeof(OutData)); /* Send block of characters */
    }
    DataReceivedFlg = FALSE;
  }
}
 
Content of Event.c:
 
extern volatile bool DataReceivedFlg;
 
void AS1_OnBlockReceived(LDD_TUserData *UserDataPtr)
{
  DataReceivedFlg = TRUE; /* Set DataReceivedFlg flag */
}
 
Wish it helps.
 
Should you need to contact us with regard to this message, please see the notes below. 
 
Best Regards,
Hui 
Technical Support 
Freescale Semiconductor
 