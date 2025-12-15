#include <iostream>
#include <math.h>
#include "Application.h"

int main(int argc, char* args[])
{
    Application app;

    // grid: width, height, cell size (pixels)
    app.Setup(160, 90, 8);

    while(app.IsRunning()) 
    {
        app.Input();
        app.Update();
        app.Render();
    }

    app.Destroy();

    return 0;
}

