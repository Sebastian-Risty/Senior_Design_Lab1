# include "BT.h"
# include "gui.h"

int main()
{
<<<<<<< Updated upstream
    while (!initPair()) {}
=======
    initPair();
>>>>>>> Stashed changes

    thread readDataThread(readData);
    thread writeDataThread(writeData);
    thread GUIThread(GUI);

<<<<<<< Updated upstream

=======
>>>>>>> Stashed changes
    readDataThread.join();
    writeDataThread.join();
    GUIThread.join();

    return 0;
}
