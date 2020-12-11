1.sample_face_det
  instructions:
  (1)support 800x1024 yuv420 image input.
  (2)detecting the face and obtaining the position of the face.
     and the coordinates of key points

  usage:
  (1)sample_face_det [y data] [uv data] [debug] [track]
  (2)result in /mnt/usb/det_result.txt

  for example:
  sample_face_det /mnt/usb/1_y.bin /mnt/usb/1_uv.bin 1 1

2.sample_det_batch
  instructions:
  (1)support 800x1024 yuv420 image input.
  (2)batch detction of faces, obtaining the position of the face and key points.

  usage:
  (1)cd to directory with yuv420 data
  (2)excute program : sample_det_batch
  (3)result in [current directory]/_loss/

3.sample_rec
  instructions:
  (1)support 800x1024 yuv420 image input.
  (2)face recognition based on the provided image and key points.

  usage:
  (1)sample_rec [y data] [uv data] [key point data]
  (2)result in /mnt/reg_result.bin