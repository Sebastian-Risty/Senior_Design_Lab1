# include "BT.h"
# include "gui.h"

int main()
{
    while (!initPair()) {}

    thread readDataThread(readData);
    thread writeDataThread(writeData);
    thread GUIThread(GUI);


    readDataThread.join();
    writeDataThread.join();
    GUIThread.join();

    return 0;
}
