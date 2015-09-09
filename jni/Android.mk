LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

include C:\Development\OpenCV-android-sdk\sdk\native\jni\OpenCV.mk

LOCAL_MODULE    := AR
LOCAL_SRC_FILES := ar.cpp src/sys.cpp src/KNN.cpp src/ArMoments.cpp src/Obj.cpp
LOCAL_LDLIBS +=  -llog -ldl

include $(BUILD_SHARED_LIBRARY)
