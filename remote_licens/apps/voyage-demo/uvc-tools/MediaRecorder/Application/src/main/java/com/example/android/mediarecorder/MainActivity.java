/*
 * Copyright (C) 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.example.android.mediarecorder;

import android.Manifest;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.pm.PackageManager;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.media.CamcorderProfile;
import android.media.MediaRecorder;
import android.os.AsyncTask;
import android.os.Bundle;
import android.util.Log;
import android.view.TextureView;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.core.app.ActivityCompat;

import com.example.android.common.media.CameraHelper;

import java.io.File;
import java.io.IOException;
import java.util.List;

/**
 * This activity uses the camera/camcorder as the A/V source for the {@link android.media.MediaRecorder} API.
 * A {@link android.view.TextureView} is used as the camera preview which limits the code to API 14+. This
 * can be easily replaced with a {@link android.view.SurfaceView} to run on older devices.
 */
public class MainActivity extends Activity implements ActivityCompat.OnRequestPermissionsResultCallback {

  private static final int MEDIA_RECORDER_REQUEST = 0;
  private static final String TAG = "Recorder";
  private final String[] requiredPermissions = {
          Manifest.permission.WRITE_EXTERNAL_STORAGE,
          Manifest.permission.RECORD_AUDIO,
          Manifest.permission.CAMERA,
  };
  private Camera mCamera;
  private TextureView mPreview;
  private int count = 0;
  private MediaRecorder mMediaRecorder;
  private File mOutputFile;
  private boolean isRecording = false;
  private Button captureButton;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.sample_main);

    mPreview = (TextureView) findViewById(R.id.surface_view);
    mPreview.setSurfaceTextureListener(new TextureView.SurfaceTextureListener() {
      @Override
      public void onSurfaceTextureAvailable(SurfaceTexture surface, int width, int height) {

      }

      @Override
      public void onSurfaceTextureSizeChanged(SurfaceTexture surface, int width, int height) {

      }

      @Override
      public boolean onSurfaceTextureDestroyed(SurfaceTexture surface) {
        return false;
      }

      @Override
      public void onSurfaceTextureUpdated(SurfaceTexture surface) {
//                Log.i(TAG, "onSurfaceTextureUpdated: count: " + count++);
      }
    });
    captureButton = (Button) findViewById(R.id.button_capture);
  }

  /**
   * The capture button controls all user interaction. When recording, the button click
   * stops recording, releases {@link android.media.MediaRecorder} and {@link android.hardware.Camera}. When not recording,
   * it prepares the {@link android.media.MediaRecorder} and starts recording.
   *
   * @param view the view generating the event.
   */
  public void onCaptureClick(View view) {

    if (areCameraPermissionGranted()) {
      startCapture();
    } else {
      requestCameraPermissions();
    }
  }

  private void startCapture() {

    if (isRecording) {
      // BEGIN_INCLUDE(stop_release_media_recorder)

      // stop recording and release camera
      try {
        mMediaRecorder.stop();  // stop the recording
      } catch (RuntimeException e) {
        // RuntimeException is thrown when stop() is called immediately after start().
        // In this case the output file is not properly constructed ans should be deleted.
        Log.d(TAG, "RuntimeException: stop() is called immediately after start()");
        //noinspection ResultOfMethodCallIgnored
        mOutputFile.delete();
        Toast.makeText(MainActivity.this, "录制失败！", Toast.LENGTH_SHORT).show();
      }
      releaseMediaRecorder(); // release the MediaRecorder object
      mCamera.lock();         // take camera access back from MediaRecorder

      // inform the user that recording has stopped
      setCaptureButtonText("Capture");
      isRecording = false;
      releaseCamera();

      // END_INCLUDE(stop_release_media_recorder)

      AlertDialog.Builder dialog = new AlertDialog.Builder(this);
      dialog.setTitle("文件录制成功!，保存在" + mOutputFile.getPath());
      dialog.setPositiveButton("确定", new DialogInterface.OnClickListener() {
        @Override
        public void onClick(DialogInterface dialog, int which) {
          dialog.dismiss();
        }
      });
      dialog.show();
    } else {

      // BEGIN_INCLUDE(prepare_start_media_recorder)

      new MediaPrepareTask().execute(null, null, null);

      // END_INCLUDE(prepare_start_media_recorder)

    }
  }

  private void setCaptureButtonText(String title) {
    captureButton.setText(title);
  }

  @Override
  protected void onPause() {
    super.onPause();
    // if we are using MediaRecorder, release it first
    releaseMediaRecorder();
    // release the camera immediately on pause event
    releaseCamera();
  }

  private void releaseMediaRecorder() {
    if (mMediaRecorder != null) {
      // clear recorder configuration
      mMediaRecorder.reset();
      // release the recorder object
      mMediaRecorder.release();
      mMediaRecorder = null;
      // Lock camera for later use i.e taking it back from MediaRecorder.
      // MediaRecorder doesn't need it anymore and we will release it if the activity pauses.
      mCamera.lock();
    }
  }

  private void releaseCamera() {
    if (mCamera != null) {
      // release the camera for other applications
      mCamera.release();
      mCamera = null;
    }
  }

  private boolean prepareVideoRecorder() {

    // BEGIN_INCLUDE (configure_preview)
    Log.i("camera", "Camera size: " + Camera.getNumberOfCameras());
    if (Camera.getNumberOfCameras() < 1) {
      Log.i(TAG, "prepareVideoRecorder: Camera number is 0!");
      runOnUiThread(new Runnable() {
        @Override
        public void run() {
          Toast.makeText(MainActivity.this, "当前未检测到可用的摄像头！",
                  Toast.LENGTH_SHORT).show();
        }
      });
      return false;
    }

    try {
      mCamera = Camera.open(0);
    } catch (Exception e) {
      e.printStackTrace();
      return false;
    }

    // We need to make sure that our preview and recording video size are supported by the
    // camera. Query camera to find all the sizes and choose the optimal size given the
    // dimensions of our preview surface.
    Camera.Parameters parameters = mCamera.getParameters();
    List<Camera.Size> mSupportedPreviewSizes = parameters.getSupportedPreviewSizes();
    List<Camera.Size> mSupportedVideoSizes = parameters.getSupportedVideoSizes();


//        Camera.Size optimalSize = CameraHelper.getOptimalVideoSize(mSupportedVideoSizes,
//                mSupportedPreviewSizes, mPreview.getWidth(), mPreview.getHeight());

    Camera.Size optimalSize = mSupportedVideoSizes.get(0);
    Log.i(TAG, "prepareVideoRecorder: optimalSize: " + optimalSize.height);

    // Use the same size for recording profile.
    CamcorderProfile profile = CamcorderProfile.get(0, CamcorderProfile.QUALITY_HIGH);
    profile.videoFrameWidth = optimalSize.width;
    profile.videoFrameHeight = optimalSize.height;

    // likewise for the camera object itself.
//        parameters.setPreviewSize(1920, 1080);
    parameters.setPreviewSize(profile.videoFrameWidth, profile.videoFrameHeight);
    mCamera.setParameters(parameters);
    try {
      // Requires API level 11+, For backward compatibility use {@link setPreviewDisplay}
      // with {@link SurfaceView}
      mCamera.setPreviewTexture(mPreview.getSurfaceTexture());
    } catch (IOException e) {
      Log.e(TAG, "Surface texture is unavailable or unsuitable" + e.getMessage());
      return false;
    }
    // END_INCLUDE (configure_preview)


    // BEGIN_INCLUDE (configure_media_recorder)
    mMediaRecorder = new MediaRecorder();

    // Step 1: Unlock and set camera to MediaRecorder
    mCamera.unlock();
    mMediaRecorder.setCamera(mCamera);
    mCamera.setPreviewCallback(new Camera.PreviewCallback() {
      @Override
      public void onPreviewFrame(byte[] data, Camera camera) {
        Log.i(TAG, "onPreviewFrame: ");
      }
    });

    // Step 2: Set sources
    mMediaRecorder.setAudioSource(MediaRecorder.AudioSource.DEFAULT);
    mMediaRecorder.setVideoSource(MediaRecorder.VideoSource.CAMERA);

    // Step 3: Set a CamcorderProfile (requires API Level 8 or higher)
    Log.i(TAG, "prepareVideoRecorder: fr: " + profile.videoFrameRate);
    mMediaRecorder.setProfile(profile);
//        mMediaRecorder.setVideoFrameRate(30);

    // Step 4: Set output file
    mOutputFile = CameraHelper.getOutputMediaFile(CameraHelper.MEDIA_TYPE_VIDEO);
    if (mOutputFile == null) {
      return false;
    }
    mMediaRecorder.setOutputFile(mOutputFile.getPath());
    // END_INCLUDE (configure_media_recorder)

    // Step 5: Prepare configured MediaRecorder
    try {
      mMediaRecorder.prepare();
    } catch (IllegalStateException e) {
      Log.d(TAG, "IllegalStateException preparing MediaRecorder: " + e.getMessage());
      releaseMediaRecorder();
      return false;
    } catch (IOException e) {
      Log.d(TAG, "IOException preparing MediaRecorder: " + e.getMessage());
      releaseMediaRecorder();
      return false;
    }
    return true;
  }

  private boolean areCameraPermissionGranted() {

    for (String permission : requiredPermissions) {
      if (!(ActivityCompat.checkSelfPermission(this, permission) ==
              PackageManager.PERMISSION_GRANTED)) {
        return false;
      }
    }
    return true;
  }

  private void requestCameraPermissions() {
    ActivityCompat.requestPermissions(
            this,
            requiredPermissions,
            MEDIA_RECORDER_REQUEST);
  }

  @Override
  public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions,
                                         @NonNull int[] grantResults) {

    if (MEDIA_RECORDER_REQUEST != requestCode) {
      super.onRequestPermissionsResult(requestCode, permissions, grantResults);
      return;
    }

    boolean areAllPermissionsGranted = true;
    for (int result : grantResults) {
      if (result != PackageManager.PERMISSION_GRANTED) {
        areAllPermissionsGranted = false;
        break;
      }
    }

    if (areAllPermissionsGranted) {
      startCapture();
    } else {
      // User denied one or more of the permissions, without these we cannot record
      // Show a toast to inform the user.
      Toast.makeText(getApplicationContext(),
              getString(R.string.need_camera_permissions),
              Toast.LENGTH_SHORT)
              .show();
    }
  }

  /**
   * Asynchronous task for preparing the {@link android.media.MediaRecorder} since it's a long blocking
   * operation.
   */
  class MediaPrepareTask extends AsyncTask<Void, Void, Boolean> {

    @Override
    protected Boolean doInBackground(Void... voids) {
      // initialize video camera
      if (prepareVideoRecorder()) {
        // Camera is available and unlocked, MediaRecorder is prepared,
        // now you can start recording
        mMediaRecorder.start();

        isRecording = true;
      } else {
        // prepare didn't work, release the camera
        releaseMediaRecorder();
        return false;
      }
      return true;
    }

    @Override
    protected void onPostExecute(Boolean result) {
      if (!result) {
        Log.e(TAG, "onPostExecute: recorder fail!");
        //MainActivity.this.finish();
      } else {
        // inform the user that recording has started
        setCaptureButtonText("Stop");
      }
    }
  }

}
