////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/graphics/ui/imgui/cimgui.h>
#include <csr/graphics/ui/imgui/cimgui_pimpl.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

struct string cimgui_platform_get_name()
{
    return make_string("NULL");
}

result_e cimgui_platform_startup()
{
    // display size doesn't matter at all :)
    igGetIO()->DisplaySize = ((ImVec2){640, 480});

    return RC_SUCCESS;
}

void cimgui_platform_shutdown() {}
void cimgui_platform_new_frame() {}
