1.video_preview instructions
  (1) 1-way video preview
  (2) Cam pipe0 1080P rot enabled, rot90 flip
  (3) 2 UI planes test, one for planescale test, one for general plane test
  (4) you can open 2-way video preview for test
  (5) you can select video format for test
  (6) you can select display mode for test

  Parameter description:
  (1) -d <device>:select input device,
      2-way camera input is used like this "-d '/dev/video3' -d '/dev/video1'"
  (2) -n <bufs number>:select video input buffer number, must be >= 3
  (3) -r <horizontal_resolution vertical_resolution>:video resolution
  (4) -t <transform mode>:
      0:none; 1:hflip; 2:vflip; 4:90rot; 3:180rot 7:270rot; 5:90hflip; 6:90vflip
  (5) -f <format>:select format , eg:NV12 or RG16 etc.
  (6) -i <image mode>:0:stretch 1:trim
  (7) -s <multiples>:Show reduced multiples
  (8) -h,-H:Show help
  (9) -c <multiples>:The number of times to restart the camera test

  for example,
  (1) OV5640 1080P cam input
  (2) LVDS TM070JDHG30-00 1280x800@60hz screen output
  (3) EVB board used
  (4) test cmd: video_preview -d /dev/video3 -d /dev/video0 -t 3 -f NV12 -i 1

  Precautions:
  (1) If you need to output 1080P images, you need to modify the kernel and uboot
      at the same time.
