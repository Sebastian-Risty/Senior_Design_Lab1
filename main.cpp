# include "BT.h"
# include "gui.h"

int main()
{
    while (!initPair()) {}

    thread readDataThread(readData);
    thread GUIThread(GUI);

    readDataThread.join();
    GUIThread.join();

    return 0;
}
