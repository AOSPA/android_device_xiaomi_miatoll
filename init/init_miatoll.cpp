/*
 *
 * Copyright (C) 2024 Paranoid Android
 * Copyright (C) 2020 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#include <android-base/properties.h>
#include <sys/sysinfo.h>

#include <cstdlib>
#include <cstring>
#include <vector>
#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>

#include "property_service.h"
#include "vendor_init.h"

using android::base::GetProperty;

// list of partitions to override props
std::vector<std::string> ro_props_default_source_order = {
    "", "odm.", "odm_dlkm.", "product.", "system.", "system_ext.", "vendor.", "vendor_dlkm.",
};

void property_override(char const prop[], char const value[], bool add = true) {
    auto pi = (prop_info *)__system_property_find(prop);

    if (pi != nullptr) {
        __system_property_update(pi, value, strlen(value));
    } else if (add) {
        __system_property_add(prop, strlen(prop), value, strlen(value));
    }
}

void set_ro_build_prop(const std::string &source, const std::string &prop,
                       const std::string &value, bool product = false) {
    std::string prop_name;

    if (product) {
        prop_name = "ro.product." + source + prop;
    } else {
        prop_name = "ro." + source + "build." + prop;
    }

    property_override(prop_name.c_str(), value.c_str(), true);
}

void set_device_props(const std::string fingerprint, const std::string description,
                      const std::string brand, const std::string device, const std::string model) {
    for (const auto &source : ro_props_default_source_order) {
        set_ro_build_prop(source, "fingerprint", fingerprint);
        set_ro_build_prop(source, "brand", brand, true);
        set_ro_build_prop(source, "device", device, true);
        set_ro_build_prop(source, "model", model, true);
    }

    property_override("ro.build.fingerprint", fingerprint.c_str());
    property_override("ro.build.description", description.c_str());
    property_override("bluetooth.device.default_name", model.c_str());
}

void load_device_properties() {
    // Detect variant and override properties
    std::string hwname = GetProperty("ro.boot.hwname", "");
    std::string region = GetProperty("ro.boot.hwc", "");
    std::string hwversion = GetProperty("ro.boot.hwversion", "");

    if (hwname == "curtana") {
        if (region == "Global_PA" || region == "Global_TWO" || region == "Japan") {
            set_device_props(
                "Redmi/curtana_global/curtana:12/RKQ1.211019.001/V14.0.4.0.SJWMIXM:user/release-keys",
                "qssi-user 12 SKQ1.211019.001 V14.0.4.0.SJWMIXM-release-keys", "Redmi", "curtana",
                "Redmi Note 9S");
        } else if (region == "India") {
            set_device_props(
                "Redmi/curtana/curtana:12/RKQ1.211019.001/V14.0.3.0.SJWINXM:user/release-keys",
                "qssi-user 12 SKQ1.211019.001 V14.0.3.0.SJWINXM release-keys", "Redmi", "curtana",
                "Redmi Note 9 Pro");
        }
    } else if (hwname == "excalibur") {
        set_device_props(
            "Redmi/excalibur_in/excalibur:12/RKQ1.211019.001/V14.0.2.0.SJXINXM:user/release-keys",
            "qssi-user 12 SKQ1.211019.001 V14.0.2.0.SJXINXM release-keys", "Redmi", "excalibur",
            "Redmi Note 9 Pro Max");
    } else if (hwname == "gram") {
        set_device_props(
            "POCO/gram_in/gram:12/RKQ1.211019.001/V14.0.5.0.SJPINXM:user/release-keys",
            "qssi-user 12 SKQ1.211019.001 V14.0.5.0.SJPINXM release-keys", "POCO", "gram",
            "POCO M2 Pro");
    } else if (hwname == "joyeuse") {
        set_device_props(
            "Redmi/joyeuse_global/joyeuse:12/RKQ1.211019.001/V14.0.3.0.SJZMIXM:user/release-keys",
            "qssi-user 12 SKQ1.211019.001 V14.0.3.0.SJZMIXM release-keys", "Redmi", "joyeuse",
            "Redmi Note 9 Pro");
    }
    property_override("vendor.boot.hwversion", hwversion.c_str());
    property_override("ro.boot.product.hardware.sku", hwname.c_str());
}

void load_dalvik_properties() {
    char const *heapstartsize;
    char const *heapgrowthlimit;
    char const *heapsize;
    char const *heapminfree;
    char const *heapmaxfree;
    char const *heaptargetutilization;
    struct sysinfo sys;

    sysinfo(&sys);

    if (sys.totalram >= 5ull * 1024 * 1024 * 1024) {
        // from - phone-xhdpi-6144-dalvik-heap.mk
        heapstartsize = "16m";
        heapgrowthlimit = "256m";
        heapsize = "512m";
        heaptargetutilization = "0.5";
        heapminfree = "8m";
        heapmaxfree = "32m";
    } else if (sys.totalram >= 3ull * 1024 * 1024 * 1024) {
        // from - phone-xhdpi-4096-dalvik-heap.mk
        heapstartsize = "8m";
        heapgrowthlimit = "192m";
        heapsize = "512m";
        heaptargetutilization = "0.6";
        heapminfree = "8m";
        heapmaxfree = "16m";
    } else {
        return;
    }

    property_override("dalvik.vm.heapstartsize", heapstartsize);
    property_override("dalvik.vm.heapgrowthlimit", heapgrowthlimit);
    property_override("dalvik.vm.heapsize", heapsize);
    property_override("dalvik.vm.heaptargetutilization", heaptargetutilization);
    property_override("dalvik.vm.heapminfree", heapminfree);
    property_override("dalvik.vm.heapmaxfree", heapmaxfree);
}

void load_audio_properties() {
    property_override("persist.vendor.audio.voicecall.speaker.stereo", "false");
}

void vendor_load_properties() {
    load_dalvik_properties();
    load_device_properties();
    load_audio_properties();
}
