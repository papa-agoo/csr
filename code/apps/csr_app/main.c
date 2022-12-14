////////////////////////////////////////////////////////////////////////////////////////////////////

#include "application.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

result_e main()
{
    result_e result = application_init();
    check_result(result, "could not init application");

    application_run();
    application_quit();

error:
    return result;
}
