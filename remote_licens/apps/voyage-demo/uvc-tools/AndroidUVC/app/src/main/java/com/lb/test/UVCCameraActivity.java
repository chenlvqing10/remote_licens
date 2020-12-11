package com.lb.test;

import static android.content.pm.PackageManager.PERMISSION_GRANTED;
import static android.widget.Toast.LENGTH_SHORT;

import android.Manifest;
import android.content.Intent;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.graphics.Matrix;
import android.graphics.PixelFormat;
import android.graphics.RectF;
import android.hardware.usb.UsbDevice;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.HandlerThread;
import android.text.TextUtils;
import android.util.Log;
import android.view.TextureView;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.Spinner;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import com.lb.uvccamera.UVCCameraProxy;
import com.lb.uvccamera.bean.PicturePath;
import com.lb.uvccamera.callback.ConnectCallback;
import com.lombo.app.faceid.RegisterImgInfo;
import com.lombo.app.faceid.RegisterInfo;
import com.lombo.app.faceid.UVCConfigInfo;
import com.lombo.app.faceid.api;
import com.lombo.app.view.RectView;
import com.serenegiant.usb.Size;

import java.text.DecimalFormat;
import java.util.ArrayList;
import java.util.List;

public class UVCCameraActivity extends AppCompatActivity {
  private static final String TAG = "UVCCameraActivity";

  private static final String ADB_PATH = "/system/bin/adb";
  private static final String IMG_PATH = "/mnt/sdcard/face-register";

  private TextureView mTextureView;
  private RectView mSurfaceView;
  private ImageView mImageView1;
  private Spinner mSpinner;
  private UVCCameraProxy mUVCCamera;
  private int mPreviewWidth = 0;
  private int mPreviewHeight = 0;

  private boolean isFirst = true;
  private String path1;

  private EditText mEtUserName;
  private boolean mStopPreView = false;
  private Boolean mDrawRecg = true;
  private Switch mSwhLiving;

  private TextView mTvInfoVersion;
  private TextView mTvThreshold;
  private TextView mTvMaxFacePixel;
  private TextView mTvMinFacePixel;
  private TextView mTvMaxTrackFaces;
  private TextView mTvResolution;
  private UVCConfigInfo mUVCConfigInfo;

  private Handler mBackgroundHandler;

  private Handler getBackgroundHandler() {
    if (mBackgroundHandler == null) {
      HandlerThread thread = new HandlerThread("background");
      thread.start();
      mBackgroundHandler = new Handler(thread.getLooper());
    }
    return mBackgroundHandler;
  }

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    requestWindowFeature(Window.FEATURE_NO_TITLE);
    getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
            WindowManager.LayoutParams.FLAG_FULLSCREEN);
    setContentView(R.layout.activity_uvc_camera);

    initView();
    requestPermission();
    initUVCCamera();
  }

  @Override
  protected void onResume() {
    Log.i(TAG, "onResume: ");
    super.onResume();
    try {
      mUVCCamera.startPreview();
    } catch (Exception e) {
      e.printStackTrace();
    }
  }

  @Override
  protected void onPause() {
    Log.i(TAG, "onPause: ");
    super.onPause();
    try {
      mUVCCamera.stopPreview();
    } catch (Exception e) {
      e.printStackTrace();
    }
  }

  @Override
  protected void onDestroy() {
    super.onDestroy();
    if (mBackgroundHandler != null) {
      if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN_MR2) {
        mBackgroundHandler.getLooper().quitSafely();
      } else {
        mBackgroundHandler.getLooper().quit();
      }
      mBackgroundHandler = null;
    }
  }

  private void initView() {
    mTextureView = findViewById(R.id.textureView);
    mSurfaceView = findViewById(R.id.surfaceView);
    mSurfaceView.setZOrderOnTop(true);
    mSurfaceView.getHolder().setFormat(PixelFormat.TRANSLUCENT);
    mSurfaceView.setDataSrcType(RectView.DataSrcType.API);
//    mImageView1 = findViewById(R.id.imag1);
    mSpinner = findViewById(R.id.spinner);
    mSpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
      @Override
      public void onItemSelected(AdapterView<?> parent, View view, final int position, long id) {
        Log.i(TAG, "position-->" + position);
        if (isFirst) {
          isFirst = false;
          return;
        }
        mUVCCamera.stopPreview();
        List<Size> list = mUVCCamera.getSupportedPreviewSizes();
        if (!list.isEmpty()) {
          mUVCCamera.setPreviewSize(list.get(position).width, list.get(position).height);
          mTvResolution.setText(getResources().getString(R.string.resolution) + list.get(position).height);
          mUVCCamera.startPreview();
        }
      }

      @Override
      public void onNothingSelected(AdapterView<?> parent) {

      }
    });

    mEtUserName = findViewById(R.id.name);
    mSwhLiving = findViewById(R.id.swh_living);
    mSwhLiving.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
      @Override
      public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
        if (mSurfaceView != null) {
          mSurfaceView.setCheckLiving(isChecked);
        }
      }
    });
    mTvInfoVersion = findViewById(R.id.tv_firmware_version);
    mTvThreshold = findViewById(R.id.tv_threshold);
    mTvMaxFacePixel = findViewById(R.id.tv_max_face_pixel);
    mTvMinFacePixel = findViewById(R.id.tv_min_face_pixel);
    mTvMaxTrackFaces = findViewById(R.id.tv_max_track_faces);
    mTvResolution = findViewById(R.id.tv_resolution);
//    File file = new File(ADB_PATH);
//    if (!file.exists()) {
//      Log.w(TAG, "initView: no adb!!!");
//      findViewById(R.id.bt_export_register_img).setVisibility(View.GONE);
//    }

    PackageManager pm = getPackageManager();
    try {
      PackageInfo packageInfo = pm.getPackageInfo(getPackageName(), 0);
      if (packageInfo != null) {
        ((TextView) findViewById(R.id.tv_app_version)).setText(getResources().getString(R.string.app_version)
                + packageInfo.versionName);
      }
    } catch (PackageManager.NameNotFoundException e) {
      e.printStackTrace();
    }
  }

  private void requestPermission() {
    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
      Log.i(TAG, "requestPermission: SDK: " + Build.VERSION.SDK_INT);
      String[] permissions = new String[]{Manifest.permission.CAMERA, Manifest.permission.READ_EXTERNAL_STORAGE};
      requestPermissions(permissions, 0);
    }
  }

  @Override
  public void onRequestPermissionsResult(int requestCode, String[]  permissions, int[] grantResults) {
    if (permissions == null) {
      Log.w(TAG, "onRequestPermissionsResult: permissions is null!");
      return;
    }

    for (int i = 0; i < permissions.length; i++) {
      if (permissions[i].equals(Manifest.permission.CAMERA)) {
        if (grantResults[i] == PERMISSION_GRANTED) {
          Log.i(TAG, "onRequestPermissionsResult: grant camera permission!");
          getBackgroundHandler().post(new Runnable() {
            @Override
            public void run() {
              try {
                if (mUVCCamera != null) {
                  mUVCCamera.startPreview();
                }
              } catch (Exception e) {
                e.printStackTrace();
              }
            }
          });
        }
      }
    }
  }

  private void initUVCCamera() {
    mUVCCamera = new UVCCameraProxy(this);
    // 已有默认配置，不需要可以不设置
    mUVCCamera.getConfig()
            .isDebug(true)
            .setPicturePath(PicturePath.SDCARD)
            .setDirName("face-register")
            .setProductId(0)
            .setVendorId(0);
    mUVCCamera.setPreviewTexture(mTextureView);

    mUVCCamera.setConnectCallback(new ConnectCallback() {
      @Override
      public void onAttached(UsbDevice usbDevice) {
        mUVCCamera.requestPermission(usbDevice);
      }

      @Override
      public void onGranted(UsbDevice usbDevice, boolean granted) {
        if (granted) {
          mUVCCamera.connectDevice(usbDevice);
        }
      }

      @Override
      public void onConnected(UsbDevice usbDevice) {

        getBackgroundHandler().post(new Runnable() {
          @Override
          public void run() {
            mUVCCamera.openCamera();

            for (int i = 0; i < 4; i++) {
              if ((mUVCConfigInfo = api.getInstance()
                      .getUVCConfigInfo()) != null) {
                mSurfaceView.onResume();
                break;
              }
            }
            runOnUiThread(new Runnable() {
              @Override
              public void run() {
                updateUVCInfo();
                updateSwhLiving();
              }
            });
          }
        });
      }

      @Override
      public void onCameraOpened() {
        showAllPreviewSizes();
        Log.i(TAG, "onCameraOpened: previewHeight: " + mPreviewHeight);
        runOnUiThread(new Runnable() {
          @Override
          public void run() {
            mTvResolution.setText(getResources().getString(R.string.resolution) + mPreviewHeight);
          }
        });
        mUVCCamera.setPreviewSize(mPreviewWidth, mPreviewHeight);
        mUVCCamera.startPreview();
      }

      @Override
      public void onDetached(UsbDevice usbDevice) {
        mUVCCamera.closeCamera();
        mUVCConfigInfo = null;
      }
    });

    // 暂不需要预览回调数据
//    mUVCCamera.setPreviewCallback(new PreviewCallback() {
//      @Override
//      public void onPreviewFrame(byte[] yuv) {
//      }
//    });

    // 暂不需要拍照功能
//    mUVCCamera.setPhotographCallback(new PhotographCallback() {
//      @Override
//      public void onPhotographClick() {
//        mUVCCamera.takePicture();
//                mUVCCamera.takePicture("test.jpg");
//      }
//    });

//    mUVCCamera.setPictureTakenCallback(new PictureCallback() {
//      @Override
//      public void onPictureTaken(String path) {
//        path1 = path;
//        mImageView1.setImageURI(null);
//        mImageView1.setImageURI(Uri.parse(path));
//      }
//    });
  }

  private void showAllPreviewSizes() {
    isFirst = true;
    final int select = 2;
    List<Size> previewList = mUVCCamera.getSupportedPreviewSizes();
    Log.i(TAG, "showAllPreviewSizes: " + previewList);
    List<String> previewStrs = new ArrayList<>();
    for (Size size : previewList) {
      previewStrs.add(size.width + " * " + size.height);
      if (mPreviewHeight < size.height) {
        mPreviewHeight = size.height;
        mPreviewWidth = size.width;
      }
    }
    final ArrayAdapter adapter = new ArrayAdapter(UVCCameraActivity.this, android.R.layout.simple_spinner_item, previewStrs);
    adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
    runOnUiThread(new Runnable() {
      @Override
      public void run() {
        mSpinner.setAdapter(adapter);
        mSpinner.setSelection(select);
      }
    });
  }

  private void updateUVCInfo() {
    if (mUVCConfigInfo != null) {
      mTvInfoVersion.setText(getResources().getString(R.string.firmware_version)
              + mUVCConfigInfo.getVersion()
      );
      mTvThreshold.setText(getResources().getString(R.string.threshold)
              + mUVCConfigInfo.getRecg_threshold());
      mTvMinFacePixel.setText(getResources().getString(R.string.min_face_pixel)
              + mUVCConfigInfo.getMin_face_pixel());
      mTvMaxFacePixel.setText(getResources().getString(R.string.max_face_pixel)
              + mUVCConfigInfo.getMax_face_pixel());
      mTvMaxTrackFaces.setText(getResources().getString(R.string.max_track_faces)
              + mUVCConfigInfo.getMax_track_faces());
    }
  }

  private void updateSwhLiving() {
    if (mUVCConfigInfo != null) {
      if (mUVCConfigInfo.getLiving_detect() == 1) {
        mSwhLiving.setChecked(true);
        mSwhLiving.setVisibility(View.VISIBLE);
      } else {
        mSwhLiving.setChecked(false);
        mSwhLiving.setVisibility(View.GONE);
      }
    }
  }

  private boolean invalidEtUserName() {
    if (mEtUserName != null) {
      if (TextUtils.isEmpty(mEtUserName.getText().toString())) {
        Toast.makeText(this, "请输入姓名！", Toast.LENGTH_LONG).show();
        return true;
      }

      Log.i(TAG, "invalidEtUserName: length: " + mEtUserName.getText().toString().length());
      if (mEtUserName.getText().toString().length() > 12) {
        Toast.makeText(this, "姓名长度不能超过12！", Toast.LENGTH_LONG).show();
        return true;
      }
      return false;
    }
    Log.w(TAG, "invalidEtUserName: mEtUserName is null!");
    return true;
  }

  public void onClick(View v) {
    switch (v.getId()) {
      case R.id.btn1:
        mStopPreView = false;
        try {
          mUVCCamera.startPreview();
          if (mDrawRecg) {
            mSurfaceView.setDrawingRect(true);
          }
        } catch (Exception e) {
          e.printStackTrace();
        }
        break;

      case R.id.btn2:
        mStopPreView = true;
        try {
          mUVCCamera.stopPreview();
          if (mDrawRecg) {
            mSurfaceView.setDrawingRect(false);
          }
        } catch (Exception e) {
          e.printStackTrace();
        }
        break;

      case R.id.btn3:
        if (!mUVCCamera.isCameraOpen()) {
          Toast.makeText(this, "请打开摄像头预览！", LENGTH_SHORT).show();
          return;
        }
        if (invalidEtUserName()) {
          return;
        }
        getBackgroundHandler().post(new Runnable() {
          @Override
          public void run() {
            int temp = -1;
            // 尝试四次发送
            for (int i = 0; i < 4; i++) {
              if ((temp = api.getInstance()
                      .registerFaceName(mEtUserName.getText().toString())) != -1) {
                break;
              }
            }
            final int ret = temp;

            String msg;
            if (ret == 0) {
              msg = "注册 " + mEtUserName.getText().toString() + " 成功!";
              Toast.makeText(getApplicationContext(), msg, LENGTH_SHORT).show();

//              if (mUVCCamera.isCameraOpen()) {
//                mUVCCamera.getUVCCamera().setFrameCallback(new IFrameCallback() {
//                  @Override
//                  public void onFrame(ByteBuffer frame) {
//                    final int length = frame.remaining();
//                    final byte[] data = new byte[length];
//                    frame.get(data);
//                    getBackgroundHandler().post(new Runnable() {
//                      @Override
//                      public void run() {
//                        Log.i(TAG, "img data length: " + length);
//                        Bitmap bitmap = ImageUtil.yuv2Bitmap(data, mPreviewWidth, mPreviewHeight, 0);
//                        if (api.getInstance().saveRegisterImg(mEtUserName.getText().toString(),
//                                bitmap)) {
//                          Toast.makeText(UVCCameraActivity.this, "注册图片在"
//                                          + IMG_PATH + "路径下",
//                                  Toast.LENGTH_SHORT).show();
//                        } else {
//                          Toast.makeText(UVCCameraActivity.this, "保存图片失败！",
//                                  Toast.LENGTH_SHORT).show();
//                        }
//                      }
//                    });
//                    mUVCCamera.getUVCCamera().setFrameCallback(null, 0);
//                  }
//                }, UVCCamera.PIXEL_FORMAT_YUV420SP);
//              }

            } else if (ret == -1) {
              msg = "注册 " + mEtUserName.getText().toString() + " 失败!"
                      + "发送数据失败";
              Toast.makeText(getApplicationContext(), msg, LENGTH_SHORT).show();
            } else {
              msg = "注册 " + mEtUserName.getText().toString() + " 失败!"
                      + RegisterInfo.getRegisterInfo(ret);
              Toast.makeText(getApplicationContext(), msg, LENGTH_SHORT).show();
            }
          }
        });
        break;

      case R.id.btn4:
        if (invalidEtUserName()) {
          return;
        }
        getBackgroundHandler().post(new Runnable() {
          @Override
          public void run() {
            int temp = -1;
            // 尝试四次发送
            for (int i = 0; i < 4; i++) {
              if ((temp = api.getInstance()
                      .unregisterFaceName(mEtUserName.getText().toString())) != -1) {
//                temp = 0;
                break;
              }
            }
            final int ret = temp;

            String msg;
            if (ret == 0) {
              msg = "注销 " + mEtUserName.getText().toString() + " 成功!";
              Toast.makeText(getApplicationContext(), msg, LENGTH_SHORT).show();

              api.getInstance()
                      .deleteRegisterImg(mEtUserName.getText().toString());
            } else if (ret == -1){
              msg = "注销 " + mEtUserName.getText().toString() + " 失败!"
                      + "发送数据失败";
              Toast.makeText(getApplicationContext(), msg, LENGTH_SHORT).show();
            } else {
              msg = "注册 " + mEtUserName.getText().toString() + " 失败!"
                      + RegisterInfo.getUnregisterInfo(ret);
              Toast.makeText(getApplicationContext(), msg, LENGTH_SHORT).show();
            }
          }
        });
        break;

      case R.id.btn5:
        if (mDrawRecg = !mDrawRecg) {
          Toast.makeText(this, "打开人脸框", LENGTH_SHORT).show();
          if (!mStopPreView) {
            mSurfaceView.setDrawingRect(true);
          }
        } else {
          Toast.makeText(this, "关闭人脸框", LENGTH_SHORT).show();
          mSurfaceView.setDrawingRect(false);
        }
        break;

      case R.id.btn_threshold_add:
        if (mUVCConfigInfo == null) {
          Toast.makeText(this, "设备信息获取失败，请重新连接！", LENGTH_SHORT).show();
          return;
        }
        getBackgroundHandler().post(new Runnable() {
          @Override
          public void run() {
            final float threshold = Float.parseFloat(mUVCConfigInfo.getRecg_threshold());
            if (threshold >= 0.99) {
              Toast.makeText(UVCCameraActivity.this, "阈值必须在0~1之间！", LENGTH_SHORT).show();
              return;
            }
            final DecimalFormat df = new DecimalFormat("0.00");
            final String addThreshold = df.format(threshold + 0.01f);
            mUVCConfigInfo.setRecg_threshold(addThreshold);
            int temp = -1;
            // 尝试四次发送
            for (int i = 0; i < 4; i++) {
              if (api.getInstance()
                      .setUVCConfig(mUVCConfigInfo) == 0) {
                temp = 0;
                break;
              }
            }
            final int result = temp;
            runOnUiThread(new Runnable() {
              @Override
              public void run() {
                if (result == 0) {
                  Toast.makeText(getApplicationContext(), "增加阈值成功！", LENGTH_SHORT).show();
                  updateUVCInfo();
                } else {
                  mUVCConfigInfo.setRecg_threshold(df.format(threshold));
                  Toast.makeText(getApplicationContext(), "增加阈值失败！", LENGTH_SHORT).show();
                }
              }
            });
          }
        });
        break;

      case R.id.btn_threshold_delete:
        if (mUVCConfigInfo == null) {
          Toast.makeText(this, "设备信息获取失败，请重新连接！", LENGTH_SHORT).show();
          return;
        }
        getBackgroundHandler().post(new Runnable() {
          @Override
          public void run() {
            final float threshold_ = Float.parseFloat(mUVCConfigInfo.getRecg_threshold());
            if (threshold_ <= 0.01) {
              Toast.makeText(UVCCameraActivity.this, "阈值必须在0~1之间！", LENGTH_SHORT).show();
              return;
            }
            final DecimalFormat df_ = new DecimalFormat("0.00");
            final String deleteThreshold = df_.format(threshold_ - 0.01f);
            mUVCConfigInfo.setRecg_threshold(deleteThreshold);
            int temp = -1;
            // 尝试四次发送
            for (int i = 0; i < 4; i++) {
              if (api.getInstance()
                      .setUVCConfig(mUVCConfigInfo) == 0) {
                temp = 0;
                break;
              }
            }
            final int result = temp;
            runOnUiThread(new Runnable() {
              @Override
              public void run() {
                if (result == 0) {
                  Toast.makeText(getApplicationContext(), "減小阈值成功！", LENGTH_SHORT).show();
                  updateUVCInfo();
                } else {
                  mUVCConfigInfo.setRecg_threshold(df_.format(threshold_));
                  Toast.makeText(getApplicationContext(), "減小阈值失败！", LENGTH_SHORT).show();
                }
              }
            });

          }
        });
        break;

      case R.id.btn_face_add:
        if (mUVCConfigInfo == null) {
          Toast.makeText(this, "设备信息获取失败，请重新连接！", LENGTH_SHORT).show();
          return;
        }

        if (mUVCConfigInfo.getMax_face_pixel() <= mUVCConfigInfo.getMin_face_pixel() + 10) {
          Toast.makeText(this, "最小识别人脸像素不能超过最大识别人脸像素！", LENGTH_SHORT).show();
          return;
        }

        getBackgroundHandler().post(new Runnable() {
          @Override
          public void run() {
            final int facePixel = mUVCConfigInfo.getMin_face_pixel();
            if (facePixel + 10 >= 512) {
              Toast.makeText(UVCCameraActivity.this, "最小识别人脸像素不能超过512！", LENGTH_SHORT).show();
              return;
            }
            mUVCConfigInfo.setMin_face_pixel(facePixel + 10);
            int temp = -1;
            // 尝试四次发送
            for (int i = 0; i < 4; i++) {
              if (api.getInstance()
                      .setUVCConfig(mUVCConfigInfo) == 0) {
                temp = 0;
                break;
              }
            }
            final int result = temp;
            runOnUiThread(new Runnable() {
              @Override
              public void run() {
                if (result == 0) {
                  Toast.makeText(getApplicationContext(), "增加最小识别人脸像素成功！", LENGTH_SHORT).show();
                  updateUVCInfo();
                } else {
                  mUVCConfigInfo.setMin_face_pixel(facePixel);
                  Toast.makeText(getApplicationContext(), "增加最小识别人脸像素失败！", LENGTH_SHORT).show();
                }
              }
            });
          }
        });
        break;

      case R.id.btn_face_delete:
        if (mUVCConfigInfo == null) {
          Toast.makeText(this, "设备信息获取失败，请重新连接！", LENGTH_SHORT).show();
          return;
        }

        getBackgroundHandler().post(new Runnable() {
          @Override
          public void run() {
            final int facePixel_ = mUVCConfigInfo.getMin_face_pixel();
            if (facePixel_ - 10 < 80) {
              Toast.makeText(UVCCameraActivity.this, "最小识别人脸像素不能小于80！", LENGTH_SHORT).show();
              return;
            }
            mUVCConfigInfo.setMin_face_pixel(facePixel_ - 10);
            int temp = -1;
            // 尝试四次发送
            for (int i = 0; i < 4; i++) {
              if (api.getInstance()
                      .setUVCConfig(mUVCConfigInfo) == 0) {
                temp = 0;
                break;
              }
            }
            final int result = temp;
            runOnUiThread(new Runnable() {
              @Override
              public void run() {
                if (result == 0) {
                  Toast.makeText(getApplicationContext(), "减少最小识别人脸像素成功！", LENGTH_SHORT).show();
                  updateUVCInfo();
                } else {
                  mUVCConfigInfo.setMin_face_pixel(facePixel_);
                  Toast.makeText(getApplicationContext(), "减少最小识别人脸像素失败！", LENGTH_SHORT).show();
                }
              }
            });

          }
        });
        break;

      case R.id.btn_face_max_add:
        if (mUVCConfigInfo == null) {
          Toast.makeText(this, "设备信息获取失败，请重新连接！", LENGTH_SHORT).show();
          return;
        }

        getBackgroundHandler().post(new Runnable() {
          @Override
          public void run() {
            final int facePixel = mUVCConfigInfo.getMax_face_pixel();
            if (facePixel + 10 > 512) {
              Toast.makeText(UVCCameraActivity.this, "最大识别人脸像素不能超过512！", LENGTH_SHORT).show();
              return;
            }
            mUVCConfigInfo.setMax_face_pixel(facePixel + 10);
            int temp = -1;
            // 尝试四次发送
            for (int i = 0; i < 4; i++) {
              if (api.getInstance()
                      .setUVCConfig(mUVCConfigInfo) == 0) {
                temp = 0;
                break;
              }
            }
            final int result = temp;
            runOnUiThread(new Runnable() {
              @Override
              public void run() {
                if (result == 0) {
                  Toast.makeText(getApplicationContext(), "增加最大识别人脸像素成功！", LENGTH_SHORT).show();
                  updateUVCInfo();
                } else {
                  mUVCConfigInfo.setMax_face_pixel(facePixel);
                  Toast.makeText(getApplicationContext(), "增加最大识别人脸像素失败！", LENGTH_SHORT).show();
                }
              }
            });
          }
        });
        break;

      case R.id.btn_face_max_delete:
        if (mUVCConfigInfo == null) {
          Toast.makeText(this, "设备信息获取失败，请重新连接！", LENGTH_SHORT).show();
          return;
        }

        if (mUVCConfigInfo.getMax_face_pixel() - 10 <= mUVCConfigInfo.getMin_face_pixel()) {
          Toast.makeText(this, "最大识别人脸像素不能小于最小识别人脸像素！", LENGTH_SHORT).show();
          return;
        }

        getBackgroundHandler().post(new Runnable() {
          @Override
          public void run() {
            final int facePixel_ = mUVCConfigInfo.getMax_face_pixel();
            if (facePixel_ - 10 <= 80) {
              Toast.makeText(UVCCameraActivity.this, "最大识别人脸像素不能小于80！", LENGTH_SHORT).show();
              return;
            }
            mUVCConfigInfo.setMax_face_pixel(facePixel_ - 10);
            int temp = -1;
            // 尝试四次发送
            for (int i = 0; i < 4; i++) {
              if (api.getInstance()
                      .setUVCConfig(mUVCConfigInfo) == 0) {
                temp = 0;
                break;
              }
            }
            final int result = temp;
            runOnUiThread(new Runnable() {
              @Override
              public void run() {
                if (result == 0) {
                  Toast.makeText(getApplicationContext(), "减少最大识别人脸像素成功！", LENGTH_SHORT).show();
                  updateUVCInfo();
                } else {
                  mUVCConfigInfo.setMax_face_pixel(facePixel_);
                  Toast.makeText(getApplicationContext(), "减少最大识别人脸像素失败！", LENGTH_SHORT).show();
                }
              }
            });

          }
        });
        break;

      case R.id.btn_num_add:
        if (mUVCConfigInfo == null) {
          Toast.makeText(this, "设备信息获取失败，请重新连接！", LENGTH_SHORT).show();
          return;
        }

        getBackgroundHandler().post(new Runnable() {
          @Override
          public void run() {
            final int faceNum = mUVCConfigInfo.getMax_track_faces();
            if (faceNum >= 5) {
              Toast.makeText(UVCCameraActivity.this, "最多识别人脸数不能超过5！", LENGTH_SHORT).show();
              return;
            }
            mUVCConfigInfo.setMax_track_faces(faceNum + 1);
            int temp = -1;
            // 尝试四次发送
            for (int i = 0; i < 4; i++) {
              if (api.getInstance()
                      .setUVCConfig(mUVCConfigInfo) == 0) {
                temp = 0;
                break;
              }
            }
            final int result = temp;
            runOnUiThread(new Runnable() {
              @Override
              public void run() {
                if (result == 0) {
                  Toast.makeText(getApplicationContext(), "增加最多识别人脸数成功！", LENGTH_SHORT).show();
                  updateUVCInfo();
                } else {
                  mUVCConfigInfo.setMax_track_faces(faceNum);
                  Toast.makeText(getApplicationContext(), "增加最多识别人脸数失败！", LENGTH_SHORT).show();
                }
              }
            });

          }
        });
        break;

      case R.id.btn_num_delete:
        if (mUVCConfigInfo == null) {
          Toast.makeText(this, "设备信息获取失败，请重新连接！", LENGTH_SHORT).show();
          return;
        }

        getBackgroundHandler().post(new Runnable() {
          @Override
          public void run() {
            final int faceNum_ = mUVCConfigInfo.getMax_track_faces();
            if (faceNum_ <= 1) {
              Toast.makeText(UVCCameraActivity.this, "最多识别人脸数不能小于1！", LENGTH_SHORT).show();
              return;
            }
            mUVCConfigInfo.setMax_track_faces(faceNum_ - 1);
            int temp = -1;
            // 尝试四次发送
            for (int i = 0; i < 4; i++) {
              if (api.getInstance()
                      .setUVCConfig(mUVCConfigInfo) == 0) {
                temp = 0;
                break;
              }
            }
            final int result = temp;
            runOnUiThread(new Runnable() {
              @Override
              public void run() {
                if (result == 0) {
                  Toast.makeText(getApplicationContext(), "减少最多识别人脸数成功！", LENGTH_SHORT).show();
                  updateUVCInfo();
                } else {
                  mUVCConfigInfo.setMax_track_faces(faceNum_);
                  Toast.makeText(getApplicationContext(), "减少最多识别人脸数失败！", LENGTH_SHORT).show();
                }
              }
            });
          }
        });
        break;

      case R.id.bt_unregister_all:
        getBackgroundHandler().post(new Runnable() {
          @Override
          public void run() {
            int temp = -1;
            for (int i = 0; i < 4; i++) {
              if ((temp = api.getInstance()
                      .unregisterAllUser()) == 0) {
                break;
              }
            }
            final int ret = temp;

            String msg;
            if (ret == 0) {
              msg = "注销所有用户成功!";
              Toast.makeText(getApplicationContext(), msg, LENGTH_SHORT).show();
              api.getInstance().deleteAllRegisteredPictures();
            } else if (ret == -1) {
              msg = "注销所有用户失败!"
                      + "发送数据失败";
              Toast.makeText(getApplicationContext(), msg, LENGTH_SHORT).show();
            } else {
              msg = "注销失败!"
                      + RegisterInfo.getUnregisterInfo(ret);
              Toast.makeText(getApplicationContext(), msg, LENGTH_SHORT).show();
            }
          }
        });
        break;

      case R.id.bt_export_register_img:
        Log.i(TAG, "onClick: img");

        getBackgroundHandler().post(new Runnable() {
          @Override
          public void run() {
            RegisterImgInfo registerImgInfo = null;
            for (int i = 0; i < 4; i++) {
              if ((registerImgInfo = api.getInstance()
                      .getLastRegisterImgInfo()) != null) {
                break;
              }
            }

            if (registerImgInfo == null) {
              Toast.makeText(UVCCameraActivity.this, "获取人脸图片大小失败！",
                      LENGTH_SHORT).show();
              return;
            }

            if (registerImgInfo.getLatest_reg_img_size() == -1) {
              Toast.makeText(UVCCameraActivity.this, "当前无注册信息！",
                      LENGTH_SHORT).show();
              return;
            }

            String msg;
            if (api.getInstance()
                    .getRegisterImgByAdb(registerImgInfo.getLatest_reg_img_path() +
                            "/" +
                            registerImgInfo.getLatest_reg_img_name())) {
              msg = "导出图片成功!";
              Toast.makeText(getApplicationContext(), msg, LENGTH_SHORT).show();
            } else {
              msg = "导出图片失败!";
              Toast.makeText(getApplicationContext(), msg, LENGTH_SHORT).show();
            }
          }
        });
        break;

      case R.id.btn_exit:
        finish();
        break;

      default:
        break;
    }
  }

  public void onUpdate(View v) {}

  private void jump2ImageActivity(String path) {
    Intent intent = new Intent(this, ImageActivity.class);
    intent.putExtra("path", path);
    startActivity(intent);
  }

  private void configureTransform() {
    Log.i(TAG, "configureTransform: ");
    Matrix matrix = new Matrix();

    // lombo: change pixel
    RectF previewRect = new RectF(0, 0, mPreviewWidth, mPreviewHeight);
    RectF surfaceDimensions;
    surfaceDimensions = new RectF(0, 0, 1280, 800);
    matrix.setRectToRect(previewRect, surfaceDimensions, Matrix.ScaleToFit.FILL);

    mTextureView.setTransform(matrix);
  }
}
