1.video_capture instructions
  (1) camera video capture
  (2) you can choose the memory mode to test
  (3) you can choose different resolution to test
  (4) you can choose the number of caches to test
  (5) you can select the capture mode or preview mode at compile time
               capture mode: mm
               preview mode: mm ver=dis

2.Parameter description
  (1) -d <device>:select input device
  (2) -r <horizontal_resolution vertical_resolution>:video resolution
  (3) -n <bufs number>:select video input buffer number, must be >= 3
  (4) -m <memroy mode>:select memory mode number, DMA:0   MMAP:1
  (5) -f <framerate>:select framerate, eg:60
  (6) -e enumerate device resolutions and frame rates,eg：video_capture -d 0 -s
  (7) -s save frames (Save 20 frames by default)
  (8) -t 0:test mode, 1:repeatly restart test, Do not test by default
  (9) -h,-H:Show help

4.Sopport for UVC camera
  (1) MMAP only
  (2) test cmd: video_capture -d 0 -m 1

3.for example
  (1) OV5640 1080P cam input
  (2) test_0.yuv file output
  (3) EVB board used
  (4) test cmd: video_capture -d 0 -r 1920 1080 -n 3 -m 0
