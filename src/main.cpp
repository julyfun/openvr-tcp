#include <chrono>
#include <cstdio>
#include <iostream>
#include <string>
#include <thread>

#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/StreamSocket.h"
#include <openvr.h>


using namespace std;

void print_mat(float m[3][4]) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%f ", m[i][j]);
        }
        printf("\n");
    }
}

void poco() {
    try {
        Poco::Net::SocketAddress sa("localhost", 3001);
        Poco::Net::StreamSocket socket(sa);

        std::string message = "hello";
        socket.sendBytes(message.data(), message.size());

        std::cout << "Message sent: " << message << std::endl;
    } catch (Poco::Exception& exc) {
        std::cerr << "POCO Exception: " << exc.displayText() << std::endl;
    } catch (std::exception& exc) {
        std::cerr << "Standard Exception: " << exc.what() << std::endl;
    }
}

class Node {
public:
    void init_openvr() {
        vr::EVRInitError e_error = vr::VRInitError_None;
        this->vr_system = vr::VR_Init(&e_error, vr::VRApplication_Other);
        if (e_error != vr::VRInitError_None) {
            printf(
                "Unable to init VR runtime: %s",
                vr::VR_GetVRInitErrorAsEnglishDescription(e_error)
            );
            return;
        }
    }

    void update_pose() {
        char dev_class_char[vr::k_unMaxTrackedDeviceCount] = {};
        vr::VRCompositor()
            ->WaitGetPoses(tracked_device_pose, vr::k_unMaxTrackedDeviceCount, nullptr, 0);

        int m_iValidPoseCount = 0; // copy copy
        string m_strPoseClasses = "";
        for (size_t nDevice = 0; nDevice < vr::k_unMaxTrackedDeviceCount; ++nDevice) {
            if (tracked_device_pose[nDevice].bPoseIsValid) {
                m_iValidPoseCount++;
                // ConvertSteamVRMatrixToMatrix4(
                //      >tracked_device_pose[nDevice].mDeviceToAbsoluteTracking
                //  );
                // this->tracked_device_pose[nDevice].mDeviceToAbsoluteTracking.m;
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
                            print_mat(this->tracked_device_pose[nDevice].mDeviceToAbsoluteTracking.m
                            );
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
        printf("Valid poses: %d\n", m_iValidPoseCount);
        printf("PoseClasses: %s\n", m_strPoseClasses.c_str());
    };

private:
    vr::IVRSystem* vr_system;
    vr::TrackedDevicePose_t tracked_device_pose[vr::k_unMaxTrackedDeviceCount];
};

int main() {
    puts("running well");
    poco();
    // Node node;
    // node.init_openvr();
    // while (true) {
    //     node.update_pose();
    //     std::this_thread::sleep_for(std::chrono::duration<float>(1));
    // }
    return 0;
}
