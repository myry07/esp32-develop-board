config
```
[env:esp32s3]
platform = espressif32
board = esp32-s3-devkitc-1
framework = arduino
monitor_speed = 115200
board_build.arduino.partitions = default_16MB.csv
board_build.arduino.memory_type = qio_opi
build_flags = -DBOARD_HAS_PSRAM
board_upload.flash_size = 16MB
```