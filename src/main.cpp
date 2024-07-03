#include <iostream>
#include <cstdio>
#include "openvr.h"

using namespace std;

void init_openvr() {
    vr::EVRInitError e_error = vr::VRInitError_None;
    vr::TrackedDevicePose_t tracked_device_pose[vr::k_unMaxTrackedDeviceCount];
    char dev_class_char[vr::k_unMaxTrackedDeviceCount];
    vr::IVRSystem* vr_system;

    if (e_error != vr::VRInitError_None) {
        printf(
            "Unable to init VR runtime: %s",
            vr::VR_GetVRInitErrorAsEnglishDescription(e_error)
        );
        return;
    }
    vr::VRCompositor()
        ->WaitGetPoses(tracked_device_pose, vr::k_unMaxTrackedDeviceCount, nullptr, 0);

    int m_iValidPoseCount = 0;
    string m_strPoseClasses = "";
    for (size_t nDevice = 0; nDevice < vr::k_unMaxTrackedDeviceCount; ++nDevice) {
        if (tracked_device_pose[nDevice].bPoseIsValid) {
            m_iValidPoseCount++;
            // m_rmat4DevicePose[nDevice] = ConvertSteamVRMatrixToMatrix4(
            //     this->tracked_device_pose[nDevice].mDeviceToAbsoluteTracking
            // );
            if (dev_class_char[nDevice] == 0) {
                switch (vr_system->GetTrackedDeviceClass(nDevice)) {
                    case vr::TrackedDeviceClass_Controller:
                        dev_class_char[nDevice] = 'C';
                        break;
                    case vr::TrackedDeviceClass_HMD:
                        dev_class_char[nDevice] = 'H';
                        break;
                    case vr::TrackedDeviceClass_Invalid:
                        dev_class_char[nDevice] = 'I';
                        break;
                    case vr::TrackedDeviceClass_GenericTracker:
                        dev_class_char[nDevice] = 'G';
                        break;
                    case vr::TrackedDeviceClass_TrackingReference:
                        dev_class_char[nDevice] = 'T';
                        break;
                    default:
                        dev_class_char[nDevice] = '?';
                        break;
                }
            }
            m_strPoseClasses += dev_class_char[nDevice];
        }
    }
    // num
    printf("Valid poses: %d", m_iValidPoseCount);
    printf("PoseClasses: %s", m_strPoseClasses.c_str());
}

int main() {
    int n;
    cin >> n;
    cout << "hello" << endl;
    return 0;
}
