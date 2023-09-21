# include "BT.h"
# include "gui.h"

int main()
{
    thread readDataThread(readData);
    thread writeDataThread(writeData);
    thread GUIThread(GUI);

    initPair();

    readDataThread.join();
    writeDataThread.join();
    GUIThread.join();

    return 0;
}
