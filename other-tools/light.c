#include "stdio.h"
#include <limits.h>

#include <cutils/properties.h>

#include "hardware/hardware.h"
#include "hardware/lights.h"



struct light_device_t *ldev;
struct light_state_t state;

hw_module_t *light;
hw_module_methods_t *methods;

int main(int argc, char** argv) {

  int ret;
  char prop[PATH_MAX];

  ret = property_get("ro.hardware", prop, NULL);
  printf("prop %d %s\n", ret, prop);


  ret = hw_get_module("lights", (hw_module_t const**)&light);

  printf("load %d %x\n", ret, light);

  methods = light->methods;

  ret = methods->open(light, "backlight", (struct hw_device_t **)&ldev);

  printf("open %d %x\n", ret, ldev);

  state.color = atoi(argv[1]);
  state.brightnessMode = BRIGHTNESS_MODE_USER;

  ldev->set_light(ldev, &state);

//  ldev->set_light(ldev, state);

}