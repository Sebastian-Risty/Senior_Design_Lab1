# include "BT.h"
# include "gui.h"
# include "sms.h"

int main()
{
    thread GUIThread(GUI);
    initPair();
    thread readDataThread(readData);
    thread writeDataThread(writeData);
    
    readDataThread.join();
    writeDataThread.join();
    GUIThread.join();

    return 0;
}
