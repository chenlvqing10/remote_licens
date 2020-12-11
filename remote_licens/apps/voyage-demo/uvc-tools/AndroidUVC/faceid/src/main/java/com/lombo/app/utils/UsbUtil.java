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

package com.lombo.app.utils;

import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.hardware.usb.UsbConstants;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbManager;
import android.util.Log;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import com.lombo.app.callback.UsbConnectedState;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.HashMap;
import java.util.Objects;

public class UsbUtil {
  private static final String TAG = UsbUtil.class.getSimpleName();

  private Context mContext;

  private UsbCameraReceiver mUsbCameraReceiver;

  private UsbManager mUsbManager;

  @Nullable
  private UsbConnectedState mConnectedState;

  public boolean mWaitMounted = false;
  private boolean mSendUpdate = false;
  private boolean mWaitUVCUpdate = false;

  public UsbUtil(@NonNull Context context) {
    this.mContext = context;
    this.mUsbManager = (UsbManager) context.getSystemService(context.USB_SERVICE);
  }

  /**
   * Check usb camera device. And request permission if usb camera device in the
   * DeviceList.
   */
  public void checkDevice() {
    UsbDevice usbDevice = getUsbCameraDevice();
    if (usbDevice != null) {
      requestPermission(usbDevice);
    }
  }

  /**
   * Gets the usb camera device.
   *
   * @return The usb Camera device {@link UsbDevice}. Can be {@code null} if no
   * usb camera device in the DeviceList.
   */
  @Nullable
  public UsbDevice getUsbCameraDevice() {
    HashMap<String, UsbDevice> deviceMap = mUsbManager.getDeviceList();
    if (deviceMap != null) {
      for (UsbDevice usbDevice : deviceMap.values()) {
        if (isUsbCamera(usbDevice)) {
          return usbDevice;
        }
      }
    }
    return null;
  }

  /**
   * Request permission of usb camera device.
   *
   * @param usbDevice The usb camera device {@link UsbDevice}.
   */
  private void requestPermission(UsbDevice usbDevice) {
    if (mUsbManager.hasPermission(usbDevice)) {
      connectUsbDevice(usbDevice);
    } else {
//      Log.i(TAG, "requestPermission: device: " + usbDevice);
      PendingIntent pendingIntent = PendingIntent.getBroadcast(mContext, 0,
              new Intent(ACTION_USB_DEVICE_PERMISSION), PendingIntent.FLAG_UPDATE_CURRENT);
      mUsbManager.requestPermission(usbDevice, pendingIntent);
    }
  }

  /**
   * Connect to the usb camera device.
   *
   * @param usbDevice The usb camera device {@link UsbDevice}.
   */
  private void connectUsbDevice(UsbDevice usbDevice) {
    // release old device before
    UsbConnectController.getInstance()
            .release();

    if (UsbConnectController.getInstance()
            .openUsbConnection(mUsbManager, usbDevice)) {
      // If is doing update, the ConnectedState.onConnected fun is not called.
      if (UsbConnectController.getInstance()
              .isUpdate()) {
        for (int i = 0; i < 2; i++) {
          try {
            Thread.sleep(2000);
            Log.w(TAG, "run: sleep 2000 end!");
          } catch (InterruptedException e) {
            e.printStackTrace();
          }
          if (UsbConnectController.getInstance()
                  .updateUVC()) {
            mSendUpdate = false;
            mWaitUVCUpdate = true;
            break;
          }
        }
      } else {
        if (mConnectedState != null) {
          mConnectedState.onConnected();
        }
      }
    }
  }

  /**
   * Released old device before and called to {@link UsbConnectedState#onConnected()}.
   */
  public void detachedUsbDevice() {
    UsbConnectController.getInstance().release();
    if (mConnectedState != null) {
      mConnectedState.onDetached();
    }
  }

  /**
   * Sets the UsbConnectedState Callback.
   *
   * @param state The {@link UsbConnectedState} to be set.
   */
  public void setConnectedState(UsbConnectedState state) {
    this.mConnectedState = state;
  }

  /**
   * Selects the usb camera device.
   *
   * @param usbDevice Device of DeviceList {@link UsbManager#getDeviceList()}.
   * @return {@code true} current device has video class {@link UsbConstants#USB_CLASS_VIDEO}.
   * {@code false} if current device is null, or there is no usb camera device.
   */
  public static boolean isUsbCamera(UsbDevice usbDevice) {
    if (usbDevice == null) {
      return false;
    }

    for (int i = 0; i < usbDevice.getInterfaceCount(); i++) {
      if (usbDevice.getInterface(i).getInterfaceClass() == UsbConstants.USB_CLASS_VIDEO) {
        return true;
      }
    }
    return false;
  }

  /**
   * Clear the Update flag. Called when update failed.
   */
  public void clearUpdateFlag() {
    mWaitMounted = false;
    mSendUpdate = false;
    mWaitUVCUpdate = false;
  }

  /**
   * Register Usb Camera Receiver.
   *
   * @see UsbCameraReceiver
   * @see #unregisterReceiver()
   */
  public void registerReceiver() {
    Log.i(TAG, "registerReceiver: usbReceiver");
    if (mUsbCameraReceiver == null) {
      IntentFilter filter = new IntentFilter();
      filter.addAction(UsbManager.ACTION_USB_DEVICE_ATTACHED);
      filter.addAction(UsbManager.ACTION_USB_DEVICE_DETACHED);
      filter.addAction(ACTION_USB_DEVICE_PERMISSION);
//      filter.addAction(Intent.ACTION_MEDIA_MOUNTED);
//      filter.addDataScheme("file");
      mUsbCameraReceiver = new UsbCameraReceiver();
      mContext.registerReceiver(mUsbCameraReceiver, filter);
    }
  }

  /**
   * Unregister Usb Camera Receiver.
   *
   * @see #registerReceiver()
   * @see UsbCameraReceiver
   */
  public void unregisterReceiver() {
    Log.i(TAG, "unregisterReceiver: ");
    if (mUsbCameraReceiver != null) {
      mContext.unregisterReceiver(mUsbCameraReceiver);
      mUsbCameraReceiver = null;
    }
  }

  private static final String ACTION_USB_DEVICE_PERMISSION = "ACTION_USB_DEVICE_PERMISSION";
  /**
   * Receive broadcast when usb camera is attached, detached, and granted permission.
   *
   * @see #registerReceiver()
   * @see #unregisterReceiver()
   */
  private class UsbCameraReceiver extends BroadcastReceiver {
    private final String TAG = UsbCameraReceiver.class.getSimpleName();

    @Override
    public void onReceive(Context context, Intent intent) {
      String action = intent.getAction();
      if (action == null) {
        return;
      }
      Log.i(TAG, "onReceive: action: " + action);
      UsbDevice usbDevice = intent.getParcelableExtra(UsbManager.EXTRA_DEVICE);
      if (!isUsbCamera(usbDevice)) {
        Log.e(TAG, "onReceive: is not uvc camera device");
        return;
      }

      switch (action) {
        // Request permission When usb camera is attached.
        case UsbManager.ACTION_USB_DEVICE_ATTACHED:
          UsbUtil.this.requestPermission(usbDevice);
          break;

        case ACTION_USB_DEVICE_PERMISSION:
          boolean granted = intent.getBooleanExtra(UsbManager.EXTRA_PERMISSION_GRANTED,
                  false);
          Log.d(TAG, "onReceive: usb device permission: " + granted);
          if (granted) {
            if (mWaitUVCUpdate) {
              mWaitUVCUpdate = false;
              UsbConnectController.getInstance()
                      .setUpdate(false);
            }
            UsbUtil.this.connectUsbDevice(usbDevice);
          }
          break;

        case UsbManager.ACTION_USB_DEVICE_DETACHED:
          UsbUtil.this.detachedUsbDevice();
          if (mWaitMounted || mSendUpdate) {
            if (mConnectedState != null) {
              mConnectedState.onUpdateFailed();
            }
          }
          break;

        default:
          break;
      }
    }
  }

  private UsbReceiver mUsbReceiver;
  /**
   * Register Usb Receiver.
   *
   * @see #unregisterUsbReceiver()
   * @see UsbReceiver
   */
  public void registerUsbReceiver() {
    Log.i(TAG, "registerUsbReceiver: ");
    if (mUsbReceiver == null) {
      Log.i(TAG, "registerUsbReceiver: registerReceiver successful");
      IntentFilter intentFilter = new IntentFilter();
      intentFilter.addAction(Intent.ACTION_MEDIA_MOUNTED);
      intentFilter.addDataScheme("file");
      mUsbReceiver = new UsbReceiver();
      mContext.registerReceiver(mUsbReceiver, intentFilter);
    }
  }

  /**
   * Unregister Usb Receiver.
   *
   * @see #registerUsbReceiver()
   * @see UsbReceiver
   */
  public void unregisterUsbReceiver() {
    Log.i(TAG, "unregisterUsbReceiver: ");
    if (mUsbReceiver != null) {
      Log.i(TAG, "unregisterUsbReceiver: unregisterReceiver successful!");
      mContext.unregisterReceiver(mUsbReceiver);
      mUsbReceiver = null;
    }
  }

  public static final String updateFilePath = "/sdcard/face-update/package.efw";
  /**
   * Receive broadcast when media mounted.
   *
   * @see #registerUsbReceiver()
   * @see #unregisterUsbReceiver()
   */
  private class UsbReceiver extends BroadcastReceiver {
    private final String TAG = UsbReceiver.class.getSimpleName();
    @Override
    public void onReceive(Context context, final Intent intent) {
      unregisterUsbReceiver();
      Log.d(TAG, "onReceive: action: " + intent.getAction());
      if (Objects.equals(intent.getAction(), Intent.ACTION_MEDIA_MOUNTED)) {
        new Thread(new Runnable() {
          @Override
          public void run() {
            String sdCardPath = intent.getDataString();
            Log.i(TAG, "onReceive: sdCardPath: " + sdCardPath);
            InputStream input = null;
            OutputStream output = null;

            try {
              mWaitMounted = false;
              File srcFile = new File(updateFilePath);
              File desFile = new File(sdCardPath.substring(6) + "/package.efw");
              Log.i(TAG, "onReceive: srcFile length: " + srcFile.length());

              input = new FileInputStream(srcFile);
              output = new FileOutputStream(desFile);

              byte[] buffer = new byte[1024];
              int byteRead;
              Log.i(TAG, "onReceive: start read");
              while ((byteRead = input.read(buffer)) > 0) {
                output.write(buffer, 0, byteRead);
                output.flush();
              }
              input.close();
              output.close();
              Log.w(TAG, "onReceive: end read");

              mSendUpdate = true;
              // wait for finished
              try {
                Thread.sleep(10000);
                Log.w(TAG, "run: sleep 10000 end!");
              } catch (InterruptedException e) {
                e.printStackTrace();
              }
              checkDevice();

            }  catch (IOException e) {
              e.printStackTrace();

              try {
                if (input != null) {
                  input.close();
                }
                if (output != null) {
                  output.close();
                }
              } catch (IOException ioe) {
                ioe.printStackTrace();
              }

              if (mConnectedState != null) {
                mConnectedState.onUpdateFailed();
              }
            }
          }
        }).start();
      }
    }
  }
}
