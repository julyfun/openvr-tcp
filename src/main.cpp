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
            printf("%6.2f ", m[i][j]);
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

void poco_v2() {
    using namespace Poco::Net;
    try {
        SocketAddress address("172.24.81.239", 3001);
        StreamSocket socket(address);
        float data[12] = { 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0 };
        socket.sendBytes(data, sizeof(data));
        cout << "pocov2 sent ok" << endl;
    } catch (Poco::Exception& exc) {
        std::cerr << "POCO Exception: " << exc.displayText() << std::endl;
    } catch (std::exception& exc) {
        std::cerr << "Standard Exception: " << exc.what() << std::endl;
    }
}

class Node {
private:
    vr::IVRSystem* vr_system;
    vr::TrackedDevicePose_t tracked_device_pose_buffer[vr::k_unMaxTrackedDeviceCount];
    std::vector<int> trackers_idx;

    Poco::Net::StreamSocket socket;

public:
    void init_socket() {
        Poco::Net::SocketAddress sa("172.24.81.239", 3001);
        this->socket.connect(sa);
    }
    void send_floats() {
        this->vr_system->GetDeviceToAbsoluteTrackingPose(
            vr::TrackingUniverseRawAndUncalibrated,
            0,
            this->tracked_device_pose_buffer,
            vr::k_unMaxTrackedDeviceCount
        );
        // send idx cnt
        const int cnt = this->trackers_idx.size();
        const int bytes_sent = this->socket.sendBytes(&cnt, sizeof(cnt));
        printf("sent %d devices\n", cnt);
        for (const auto i: this->trackers_idx) {
            const auto pose_mat = this->tracked_device_pose_buffer[i].mDeviceToAbsoluteTracking;
            float data[12] = { pose_mat.m[0][0], pose_mat.m[0][1], pose_mat.m[0][2],
                               pose_mat.m[0][3], pose_mat.m[1][0], pose_mat.m[1][1],
                               pose_mat.m[1][2], pose_mat.m[1][3], pose_mat.m[2][0],
                               pose_mat.m[2][1], pose_mat.m[2][2], pose_mat.m[2][3] };

            int bytes_sent = this->socket.sendBytes(data, sizeof(data));
            for (int i = 0; i < 12; i++) {
                printf("%6.2f ", data[i]);
            }
            puts("");
        }
    }

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

        this->vr_system->GetDeviceToAbsoluteTrackingPose(
            vr::TrackingUniverseRawAndUncalibrated,
            0,
            this->tracked_device_pose_buffer,
            vr::k_unMaxTrackedDeviceCount
        );

        // find GenericTracker's idx
        for (size_t i = 0; i < vr::k_unMaxTrackedDeviceCount; i++) {
            // valid
            if (tracked_device_pose_buffer[i].bPoseIsValid) {
                // 获得 GenericTracker
                if (vr_system->GetTrackedDeviceClass(i) == vr::TrackedDeviceClass_GenericTracker) {
                    this->trackers_idx.push_back(i);
                    cout << "Found GenericTracker at " << i << endl;
                }
            }
        }
    }

    void check_device_pose_and_type() {
        char dev_class_char[vr::k_unMaxTrackedDeviceCount] = {};
        // 使用这个会出现能连接的假象，Tracker 有时能找得到有时不能
        // vr::VRCompositor()
        //     ->WaitGetPoses(tracked_device_pose, vr::k_unMaxTrackedDeviceCount, nullptr, 0);
        this->vr_system->GetDeviceToAbsoluteTrackingPose(
            vr::TrackingUniverseRawAndUncalibrated,
            0,
            this->tracked_device_pose_buffer,
            vr::k_unMaxTrackedDeviceCount
        );
        int m_iValidPoseCount = 0; // copy copy
        string m_strPoseClasses = "";
        for (size_t nDevice = 0; nDevice < vr::k_unMaxTrackedDeviceCount; ++nDevice) {
            if (tracked_device_pose_buffer[nDevice].bPoseIsValid) {
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
                            print_mat(
                                tracked_device_pose_buffer[nDevice].mDeviceToAbsoluteTracking.m
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
        printf("%zu\n", strlen(m_strPoseClasses.c_str()));
        printf("PoseClasses: %s\n", m_strPoseClasses.c_str());
    };
};

int main() {
    puts("running well");
    // poco();
    Node node;
    node.init_openvr();
    node.init_socket();
    while (true) {
        puts("---");
        node.send_floats();
        std::this_thread::sleep_for(std::chrono::duration<float>(0.02));
    }
    return 0;
}
