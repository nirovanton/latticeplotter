#include <iostream>
#include <cstdlib>

#include "simulate.h"
#include "output.h"

using namespace std;

int main(int argc, char *argv[])
{
    Simulate *application;
    
    try
    {
        application = new Simulate(argc, argv);
        application->Run();
    }
    catch (SimulateArgumentError e)
    {
        if (e.GetMessage().length() > 0) ERROR(e.GetMessage());
        cout << e.GetDescription() << endl;
        return EXIT_FAILURE;
    }
    catch (SimulateError e)
    {
        if (e.GetMessage().length() > 0) ERROR(e.GetMessage());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

