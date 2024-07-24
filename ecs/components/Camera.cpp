#include "Camera.h"
#include "core/Debug.h"


namespace pk
{
    void CameraWindowResizeEvent::func(int w, int h)
    {
        const float aspectRatio = (float)w / (float)h;

        _camRef.setProjMat2D(create_proj_mat_ortho(0, w, h, 0, 0.0f, 100.0f));
        _camRef.setProjMat3D(create_perspective_projection_matrix(aspectRatio, 1.3f, 0.1f, 1000.0f));

        Debug::log("Cam projections reconfigured!");
    }
}
