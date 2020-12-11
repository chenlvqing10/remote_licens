/*
 * Copyright (C) 2019 lombo Tech CO.,LTD
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.lb.test;

import static android.content.pm.PackageManager.PERMISSION_GRANTED;
import static android.widget.Toast.LENGTH_SHORT;

import android.Manifest;
import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.content.DialogInterface;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.graphics.PixelFormat;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.HandlerThread;
import android.text.TextUtils;
import android.util.Log;
import android.view.View;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import com.lombo.app.callback.UsbConnectedState;
import com.lombo.app.cameraview.CameraView;
import com.lombo.app.faceid.RegisterImgInfo;
import com.lombo.app.faceid.RegisterInfo;
import com.lombo.app.faceid.UVCConfigInfo;
import com.lombo.app.utils.UsbConnectController;
import com.lombo.app.utils.UsbUtil;
import com.lombo.app.view.RectView;

import java.io.File;
import java.text.DecimalFormat;

public class AndroidCameraActivity extends AppCompatActivity {
  private static final String TAG = AndroidCameraActivity.class.getSimpleName();

  private static final String IMG_PATH = "/mnt/sdcard/face-register";
  private static final String ADB_PATH = "/system/bin/adb";

  private RectView mSurfaceView;
  private CameraView mCameraView;
  private int mPreResolution = 0;
  private CameraView.Callback mCallback = new CameraView.Callback() {

    @Override
    public void onCameraOpened(CameraView cameraView) {
      Log.d(TAG, "onCameraOpened");
    }

    @Override
    public void onCameraClosed(CameraView cameraView) {
      Log.d(TAG, "onCameraClosed");
    }

    @Override
    public void onPreResolutionChanged(int preResolution) {
      Log.i(TAG, "onPreResolutionChanged: preResolution: " + preResolution);
      mPreResolution = preResolution;
      runOnUiThread(new Runnable() {
        @Override
        public void run() {
          mTvResolution.setText(getResources().getString(R.string.resolution) + mPreResolution);
        }
      });
    }

    @Override
    public void onPictureTaken(CameraView cameraView, final byte[] data) {
      Log.d(TAG, "onPictureTaken: " + data.length);
      getBackgroundHandler().post(new Runnable() {
        @Override
        public void run() {
          if (UsbConnectController.getInstance().saveRegisterImg(mEtUserName.getText().toString(),
                  data)) {
            Toast.makeText(AndroidCameraActivity.this, "注册图片在"
                            + IMG_PATH + "路径下",
                    Toast.LENGTH_SHORT).show();
          } else {
            Toast.makeText(AndroidCameraActivity.this, "保存图片失败！",
                    Toast.LENGTH_SHORT).show();
          }
        }
      });
    }
  };

  private EditText mEtUserName;
  private boolean mStopPreView = false;
  private boolean mDrawRecg = true;
  private Switch mSwhLiving;

  private TextView mTvInfoVersion;
  private TextView mTvThreshold;
  private TextView mTvMaxFacePixel;
  private TextView mTvMinFacePixel;
  private TextView mTvMaxTrackFaces;
  private TextView mTvResolution;

  private UsbUtil mUsbUtil;
  private UVCConfigInfo mUVCConfigInfo;

  private Handler mBackgroundHandler;

  /**
   * Gets background thread looper.
   *
   * @return Looper of background thread {@link #mBackgroundHandler}.
   */
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
    setContentView(R.layout.activity_android_camera);
    initView();
    requestPermission();
    initUsbUtil();
  }

  @Override
  protected void onResume() {
    Log.i(TAG, "onResume: ");
    super.onResume();
    getBackgroundHandler().post(new Runnable() {
      @Override
      public void run() {
        if (UsbConnectController.getInstance().isClaimed()) {
          if (!mCameraView.isCameraOpened()) {
            try {
              mCameraView.start();
            } catch (Exception e) {
              e.printStackTrace();
            }
          }
        }
        mSurfaceView.onResume();
      }
    });
  }

  @Override
  protected void onPause() {
//    Log.v(TAG, "onPause: ");
    super.onPause();
    getBackgroundHandler().post(new Runnable() {
      @Override
      public void run() {
        try {
          if (mCameraView.isCameraOpened()) {
            mCameraView.stop();
          }
          mSurfaceView.onPause();
        } catch (Exception e) {
          e.printStackTrace();
        }
      }
    });
  }

  @Override
  protected void onDestroy() {
    Log.i(TAG, "onDestroy: ");
    super.onDestroy();
    if (mUsbUtil != null) {
      mUsbUtil.unregisterReceiver();
      mUsbUtil.unregisterUsbReceiver();
      mUsbUtil.detachedUsbDevice();
    }
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
    mSurfaceView = findViewById(R.id.surfaceView);
    mSurfaceView.setZOrderOnTop(true);
    mSurfaceView.getHolder().setFormat(PixelFormat.TRANSLUCENT);

    mCameraView = findViewById(R.id.cameraView);
    if (mCameraView != null) {
      mCameraView.addCallback(mCallback);
    }

    mEtUserName = findViewById(R.id.name);
    mSwhLiving = findViewById(R.id.swh_living);
    mSwhLiving.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
      @Override
      public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
        Log.i(TAG, "onCheckedChanged: isChecked: " +isChecked);
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
                if (mCameraView != null) {
                  mCameraView.start();
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

  private void initUsbUtil() {
    mUsbUtil = new UsbUtil(this);
    mUsbUtil.registerReceiver();
    mUsbUtil.setConnectedState(new UsbConnectedState() {
      @Override
      public void onConnected() {
        Log.i(TAG, "onConnected: ");
        getBackgroundHandler().post(new Runnable() {
          @Override
          public void run() {
            if (mProgressDialog != null && mProgressDialog.isShowing()) {
              mProgressDialog.dismiss();
            }

            if (mCameraView != null && !mCameraView.isCameraOpened()) {
              try {
                mCameraView.start();
              } catch (Exception e) {
                e.printStackTrace();
              }
            }

            for (int i = 0; i < 2; i++) {
              if ((mUVCConfigInfo = UsbConnectController.getInstance()
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
      public void onDetached() {
        getBackgroundHandler().post(new Runnable() {
          @Override
          public void run() {
            if (mCameraView.isCameraOpened()) {
              Log.i(TAG, "onDetached: close camera!");
              try {
                mCameraView.stop();
              } catch (Exception e) {
                e.printStackTrace();
              }
            }
            mUVCConfigInfo = null;
          }
        });
      }

      @Override
      public void onUpdateFailed() {
        getBackgroundHandler().post(new Runnable() {
          @Override
          public void run() {
            Log.i(TAG, "onUpdateFailed: ");
            if (UsbConnectController.getInstance().isUpdate()) {
              UsbConnectController.getInstance()
                      .setUpdate(false);

              if (mUsbUtil != null) {
                mUsbUtil.unregisterUsbReceiver();
                mUsbUtil.clearUpdateFlag();
                mUsbUtil.checkDevice();
              }

              if (mProgressDialog != null && mProgressDialog.isShowing()) {
                mProgressDialog.dismiss();
              }

              AlertDialog.Builder dialog = new AlertDialog.Builder(AndroidCameraActivity.this);
              dialog.setTitle("升级失败，请检查UVC设备是否正常!");
              dialog.setCancelable(false);
              dialog.setNegativeButton("确定", new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {
                  dialog.dismiss();
                }
              });
              dialog.show();
            }
          }
        });
      }
    });
    mUsbUtil.checkDevice();
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

  /**
   * @return {@code true} if name is empty, or if the length of name exceed 12. {@code false}
   * if name is not empty and the length of name less than or equal to 12.
   */
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
        getBackgroundHandler().post(new Runnable() {
          @Override
          public void run() {
            mStopPreView = false;
            try {
              if (!mCameraView.isCameraOpened()) {
                mCameraView.start();
              }
              if (mDrawRecg) {
                mSurfaceView.setDrawingRect(true);
              }
            } catch (Exception e) {
              e.printStackTrace();
            }
          }
        });
        break;

      case R.id.btn2:
        getBackgroundHandler().post(new Runnable() {
          @Override
          public void run() {
            mStopPreView = true;
            try {
              if (mCameraView.isCameraOpened()) {
                mCameraView.stop();
              }
              if (mDrawRecg) {
                mSurfaceView.setDrawingRect(false);
              }
            } catch (Exception e) {
              e.printStackTrace();
            }
          }
        });
        break;

      case R.id.btn3:
        if (!mCameraView.isCameraOpened()) {
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
            // try send 2 times cmd for make sure succeed
            for (int i = 0; i < 2; i++) {
              if ((temp = UsbConnectController.getInstance()
                      .registerFaceName(mEtUserName.getText().toString())) != -1) {
                break;
              }
            }
            final int ret = temp;
            Log.i(TAG, "run: register result: " + temp);
            String msg;
            if (ret == 0) {
              msg = "注册 " + mEtUserName.getText().toString() + " 成功!";
              Toast.makeText(getApplicationContext(), msg, LENGTH_SHORT).show();
              if (mCameraView != null) {
                mCameraView.takePicture();
              }
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
            // try send 2 times cmd for make sure succeed
            for (int i = 0; i < 2; i++) {
              if ((temp = UsbConnectController.getInstance()
                      .unregisterFaceName(mEtUserName.getText().toString())) != -1) {
                Log.i(TAG, "run: temp: " + temp);
//                temp = 0;
                break;
              }
            }
            Log.i(TAG, "run: unregister result: " + temp);
            final int ret = temp;

            String msg;
            if (ret == 0) {
              msg = "注销 " + mEtUserName.getText().toString() + " 成功!";
              Toast.makeText(getApplicationContext(), msg, LENGTH_SHORT).show();
              UsbConnectController.getInstance()
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
        getBackgroundHandler().post(new Runnable() {
          @Override
          public void run() {
            if (mUVCConfigInfo == null) {
              Toast.makeText(AndroidCameraActivity.this, "设备信息获取失败，请重新连接！", LENGTH_SHORT).show();
              return;
            }
            final float threshold = Float.parseFloat(mUVCConfigInfo.getRecg_threshold());
            if (threshold >= 0.99) {
              Toast.makeText(AndroidCameraActivity.this, "阈值必须在0~1之间！", LENGTH_SHORT).show();
              return;
            }
            final DecimalFormat df = new DecimalFormat("0.00");
            final String addThreshold = df.format(threshold + 0.01f);
            mUVCConfigInfo.setRecg_threshold(addThreshold);
            int temp = -1;
            // try send 2 times cmd for make sure succeed
            for (int i = 0; i < 2; i++) {
              if (UsbConnectController.getInstance()
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
                  if (mUVCConfigInfo != null) {
                    mUVCConfigInfo.setRecg_threshold(df.format(threshold));
                  }
                  Toast.makeText(getApplicationContext(), "增加阈值失败！", LENGTH_SHORT).show();
                }
              }
            });
          }
        });
        break;

      case R.id.btn_threshold_delete:
        getBackgroundHandler().post(new Runnable() {
          @Override
          public void run() {
            if (mUVCConfigInfo == null) {
              Toast.makeText(AndroidCameraActivity.this, "设备信息获取失败，请重新连接！", LENGTH_SHORT).show();
              return;
            }
            final float threshold_ = Float.parseFloat(mUVCConfigInfo.getRecg_threshold());
            if (threshold_ <= 0.01) {
              Toast.makeText(AndroidCameraActivity.this, "阈值必须在0~1之间！", LENGTH_SHORT).show();
              return;
            }
            final DecimalFormat df_ = new DecimalFormat("0.00");
            final String deleteThreshold = df_.format(threshold_ - 0.01f);
            mUVCConfigInfo.setRecg_threshold(deleteThreshold);
            int temp = -1;
            // try send 2 times cmd for make sure succeed
            for (int i = 0; i < 2; i++) {
              if (UsbConnectController.getInstance()
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
                  if (mUVCConfigInfo != null) {
                    mUVCConfigInfo.setRecg_threshold(df_.format(threshold_));
                  }
                  Toast.makeText(getApplicationContext(), "減小阈值失败！", LENGTH_SHORT).show();
                }
              }
            });

          }
        });
        break;

      case R.id.btn_face_add:
        getBackgroundHandler().post(new Runnable() {
          @Override
          public void run() {
            if (mUVCConfigInfo == null) {
              Toast.makeText(AndroidCameraActivity.this, "设备信息获取失败，请重新连接！", LENGTH_SHORT).show();
              return;
            }

            if (mUVCConfigInfo.getMax_face_pixel() <= mUVCConfigInfo.getMin_face_pixel() + 10) {
              Toast.makeText(AndroidCameraActivity.this, "最小识别人脸像素不能超过最大识别人脸像素！", LENGTH_SHORT).show();
              return;
            }

            final int facePixel = mUVCConfigInfo.getMin_face_pixel();
            if (facePixel + 10 >= 512) {
              Toast.makeText(AndroidCameraActivity.this, "最小识别人脸像素不能超过512！", LENGTH_SHORT).show();
              return;
            }
            mUVCConfigInfo.setMin_face_pixel(facePixel + 10);
            int temp = -1;
            // try send 2 times cmd for make sure succeed
            for (int i = 0; i < 2; i++) {
              if (UsbConnectController.getInstance()
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
                  if (mUVCConfigInfo != null) {
                    mUVCConfigInfo.setMin_face_pixel(facePixel);
                  }
                  Toast.makeText(getApplicationContext(), "增加最小识别人脸像素失败！", LENGTH_SHORT).show();
                }
              }
            });
          }
        });
        break;

      case R.id.btn_face_delete:
        getBackgroundHandler().post(new Runnable() {
          @Override
          public void run() {
            if (mUVCConfigInfo == null) {
              Toast.makeText(AndroidCameraActivity.this, "设备信息获取失败，请重新连接！", LENGTH_SHORT).show();
              return;
            }

            final int facePixel_ = mUVCConfigInfo.getMin_face_pixel();
            if (facePixel_ - 10 < 80) {
              Toast.makeText(AndroidCameraActivity.this, "最小识别人脸像素不能小于80！", LENGTH_SHORT).show();
              return;
            }

            mUVCConfigInfo.setMin_face_pixel(facePixel_ - 10);
            int temp = -1;
            // try send 2 times cmd for make sure succeed
            for (int i = 0; i < 2; i++) {
              if (UsbConnectController.getInstance()
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
                  if (mUVCConfigInfo != null) {
                    mUVCConfigInfo.setMin_face_pixel(facePixel_);
                  }
                  Toast.makeText(getApplicationContext(), "减少最小识别人脸像素失败！", LENGTH_SHORT).show();
                }
              }
            });

          }
        });
        break;

      case R.id.btn_face_max_add:
        getBackgroundHandler().post(new Runnable() {
          @Override
          public void run() {
            if (mUVCConfigInfo == null) {
              Toast.makeText(AndroidCameraActivity.this, "设备信息获取失败，请重新连接！", LENGTH_SHORT).show();
              return;
            }

            final int facePixel = mUVCConfigInfo.getMax_face_pixel();
            if (facePixel + 10 > 512) {
              Toast.makeText(AndroidCameraActivity.this, "最大识别人脸像素不能超过512！", LENGTH_SHORT).show();
              return;
            }

            mUVCConfigInfo.setMax_face_pixel(facePixel + 10);
            int temp = -1;
            // try send 2 times cmd for make sure succeed
            for (int i = 0; i < 2; i++) {
              if (UsbConnectController.getInstance()
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
                  if (mUVCConfigInfo != null) {
                    mUVCConfigInfo.setMax_face_pixel(facePixel);
                  }
                  Toast.makeText(getApplicationContext(), "增加最大识别人脸像素失败！", LENGTH_SHORT).show();
                }
              }
            });
          }
        });
        break;

      case R.id.btn_face_max_delete:
        getBackgroundHandler().post(new Runnable() {
          @Override
          public void run() {
            if (mUVCConfigInfo == null) {
              Toast.makeText(AndroidCameraActivity.this, "设备信息获取失败，请重新连接！", LENGTH_SHORT).show();
              return;
            }

            if (mUVCConfigInfo.getMax_face_pixel() - 10 <= mUVCConfigInfo.getMin_face_pixel()) {
              Toast.makeText(AndroidCameraActivity.this, "最大识别人脸像素不能小于最小识别人脸像素！", LENGTH_SHORT).show();
              return;
            }

            final int facePixel_ = mUVCConfigInfo.getMax_face_pixel();
            if (facePixel_ - 10 <= 80) {
              Toast.makeText(AndroidCameraActivity.this, "最大识别人脸像素不能小于80！", LENGTH_SHORT).show();
              return;
            }
            mUVCConfigInfo.setMax_face_pixel(facePixel_ - 10);
            int temp = -1;
            // try send 2 times cmd for make sure succeed
            for (int i = 0; i < 2; i++) {
              if (UsbConnectController.getInstance()
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
                  if (mUVCConfigInfo != null) {
                    mUVCConfigInfo.setMax_face_pixel(facePixel_);
                  }
                  Toast.makeText(getApplicationContext(), "减少最大识别人脸像素失败！", LENGTH_SHORT).show();
                }
              }
            });

          }
        });
        break;

      case R.id.btn_num_add:
        getBackgroundHandler().post(new Runnable() {
          @Override
          public void run() {
            if (mUVCConfigInfo == null) {
              Toast.makeText(AndroidCameraActivity.this, "设备信息获取失败，请重新连接！", LENGTH_SHORT).show();
              return;
            }

            final int faceNum = mUVCConfigInfo.getMax_track_faces();
            if (faceNum >= 5) {
              Toast.makeText(AndroidCameraActivity.this, "最多识别人脸数不能超过5！", LENGTH_SHORT).show();
              return;
            }
            mUVCConfigInfo.setMax_track_faces(faceNum + 1);
            int temp = -1;
            // try send 2 times cmd for make sure succeed
            for (int i = 0; i < 2; i++) {
              if (UsbConnectController.getInstance()
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
                  if (mUVCConfigInfo != null) {
                    mUVCConfigInfo.setMax_track_faces(faceNum);
                  }
                  Toast.makeText(getApplicationContext(), "增加最多识别人脸数失败！", LENGTH_SHORT).show();
                }
              }
            });

          }
        });
        break;

      case R.id.btn_num_delete:
        getBackgroundHandler().post(new Runnable() {
          @Override
          public void run() {
            if (mUVCConfigInfo == null) {
              Toast.makeText(AndroidCameraActivity.this, "设备信息获取失败，请重新连接！", LENGTH_SHORT).show();
              return;
            }

            final int faceNum_ = mUVCConfigInfo.getMax_track_faces();
            if (faceNum_ <= 1) {
              Toast.makeText(AndroidCameraActivity.this, "最多识别人脸数不能小于1！", LENGTH_SHORT).show();
              return;
            }
            mUVCConfigInfo.setMax_track_faces(faceNum_ - 1);
            int temp = -1;
            // try send 2 times cmd for make sure succeed
            for (int i = 0; i < 2; i++) {
              if (UsbConnectController.getInstance()
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
                  if (mUVCConfigInfo != null) {
                    mUVCConfigInfo.setMax_track_faces(faceNum_);
                  }
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
            final int ret = UsbConnectController.getInstance()
                    .unregisterAllUser();

            String msg;
            if (ret == 0) {
              msg = "注销所有用户成功!";
              Toast.makeText(getApplicationContext(), msg, LENGTH_SHORT).show();
              UsbConnectController.getInstance()
                      .deleteAllRegisteredPictures();
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
        getBackgroundHandler().post(new Runnable() {
          @Override
          public void run() {
            RegisterImgInfo registerImgInfo = null;
            for (int i = 0; i < 2; i++) {
              if ((registerImgInfo = UsbConnectController.getInstance()
                      .getLastRegisterImgInfo()) != null) {
                break;
              }
            }

            if (registerImgInfo == null) {
              Toast.makeText(AndroidCameraActivity.this, "获取人脸图片大小失败！",
                      LENGTH_SHORT).show();
              return;
            }

            if (registerImgInfo.getLatest_reg_img_size() == -1) {
              Toast.makeText(AndroidCameraActivity.this, "当前无注册信息！",
                      LENGTH_SHORT).show();
              return;
            }

            String msg;
            if (UsbConnectController.getInstance()
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
//
      case R.id.btn_exit:
        finish();
        break;

//      case R.id.take_picture:
//        mCameraView.takePicture();
//        break;
    }
  }

  private ProgressDialog mProgressDialog;

  public void onUpdate(View v) {
    if (mUVCConfigInfo == null) {
      Toast.makeText(this, "设备信息获取失败，请重新连接！", LENGTH_SHORT).show();
      return;
    }

    AlertDialog.Builder dialog = new AlertDialog.Builder(this);
    dialog.setTitle("是否进行固件升级");
    dialog.setCancelable(false);
    dialog.setPositiveButton("确定", new DialogInterface.OnClickListener() {
      @Override
      public void onClick(DialogInterface dialog, int which) {
        dialog.dismiss();

        if (mProgressDialog == null) {
          Log.d(TAG, "onClick: init ProgressDialog!");
          mProgressDialog = new ProgressDialog(AndroidCameraActivity.this);
          mProgressDialog.setTitle("固件升级中...请勿操作！");
          mProgressDialog.setCancelable(true);
        }

        if (!UsbConnectController.getInstance().isUpdate()) {
          Log.d(TAG, "onClick: doing update!");
          if (!checkUpdatePkg()) {
            Toast.makeText(AndroidCameraActivity.this,
                    UsbUtil.updateFilePath + "文件缺失！", LENGTH_SHORT).show();
            return;
          }

          UsbConnectController.getInstance()
                  .setUpdate(true);

          if (!mProgressDialog.isShowing()) {
            mProgressDialog.show();
          }

          mSurfaceView.onPause();
          mCameraView.stop();
          mUsbUtil.mWaitMounted = true;
          mUsbUtil.detachedUsbDevice();
          mUsbUtil.registerUsbReceiver();
        } else {
          Log.d(TAG, "onClick: already doing updating");
        }
      }
    });

    dialog.setNegativeButton("取消", new DialogInterface.OnClickListener() {
      @Override
      public void onClick(DialogInterface dialog, int which) {
        dialog.dismiss();
      }
    });
    dialog.show();
  }

  public void onConnect(View v) {
    Log.i(TAG, "onConnect: ");
    mUsbUtil.checkDevice();

    try {
      Thread.sleep(2000);
      Log.w(TAG, "run: sleep end!");
    } catch (InterruptedException e) {
      e.printStackTrace();
    }

    UsbConnectController.getInstance()
            .updateUVC();
  }

  public void onDetach(View v) {
    Log.i(TAG, "onDetach: ");
    mSurfaceView.onPause();
    mUsbUtil.detachedUsbDevice();
  }

  private boolean checkUpdatePkg() {
    File file = new File(UsbUtil.updateFilePath);
    return file.exists() && file.isFile();
  }
}
