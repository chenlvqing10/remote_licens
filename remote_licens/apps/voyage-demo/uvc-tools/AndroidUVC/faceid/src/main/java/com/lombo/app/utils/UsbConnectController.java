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

import android.hardware.usb.UsbConstants;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbEndpoint;
import android.hardware.usb.UsbInterface;
import android.hardware.usb.UsbManager;
import android.util.Log;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import com.lombo.app.faceid.Person;
import com.lombo.app.faceid.RegisterImgInfo;
import com.lombo.app.faceid.UVCConfigInfo;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.List;
import java.util.Objects;

public class UsbConnectController {
  private static final String TAG = UsbConnectController.class.getSimpleName();

  private final Object mSync = new Object();
  private static UsbConnectController instance;

  private UsbDeviceConnection mUsbConnection;
  private UsbInterface mMassStoreIf;
  private UsbEndpoint mEndpointIn;
  private UsbEndpoint mEndpointOut;

  public boolean isClaimed() {
    return mClaimed;
  }

  private boolean mClaimed = false;
  private boolean mEndpointInFail = true;
  private boolean mEndpointOutFail = true;

  private static int CBW_TAG = 0x70;
  private static final int RESPONSE_RECV_DATA_LEN = 3000;
  private static final int PERSON_LIST_RECV_DATA_LEN = 2650;
  private int IMG_RECV_DATA_LEN_TEMP = 0;
  private int IMG_RECV_DATA_LEN_ALL = 0;
  private final int MAX_BULK_TRANSFER = 16384;

  private UsbConnectController() {
  }

  public static UsbConnectController getInstance() {
    if (instance == null) {
      synchronized (UsbConnectController.class) {
        if (instance == null) {
          instance = new UsbConnectController();
        }
      }
    }
    return instance;
  }

  /**
   * Open usb camera device connection.
   *
   * @param usbManager UsbManager
   * @param usbDevice usb camera device.
   * @return {@code true} {@link #mUsbConnection} is not empty. {@code false}
   * {@link #mUsbConnection} is empty.
   */
  public boolean openUsbConnection(@NonNull UsbManager usbManager, @NonNull UsbDevice usbDevice) {
    mUsbConnection = usbManager.openDevice(usbDevice);
    if (mUsbConnection != null) {
      initMassStoreIntf(usbDevice);
    }
    return mUsbConnection != null;
  }

  /**
   * Claim MassStore interface and init endpoint.
   *
   * @param usbDevice Use usb camera device and doing this when the device has
   *                  USB_CLASS_MASS_STORAGE interface.
   */
  private void initMassStoreIntf(UsbDevice usbDevice) {
//    Log.i(TAG, "initMassStoreIntf: ");
    for (int i = 0; i < usbDevice.getInterfaceCount(); i++) {
      UsbInterface intf = usbDevice.getInterface(i);
//      Log.i(TAG, "initMassStoreIntf: face class: " + intf.getInterfaceClass());

      if (intf.getInterfaceClass() != UsbConstants.USB_CLASS_MASS_STORAGE) {
        continue;
      }

      mMassStoreIf = intf;
//      Log.i(TAG, "initMassStoreIntf: intf endpoint count: " + intf.getEndpointCount());
      mClaimed = mUsbConnection.claimInterface(mMassStoreIf, true);
      Log.i(TAG, "initMassStoreIntf: claim: " + mClaimed);
      if (mClaimed) {
        for (int j = 0; j < intf.getEndpointCount(); j++) {
          UsbEndpoint usbEndpoint = intf.getEndpoint(j);
          if (usbEndpoint.getDirection() == UsbConstants.USB_DIR_IN) {
            mEndpointIn = usbEndpoint;
          } else if (usbEndpoint.getDirection() == UsbConstants.USB_DIR_OUT) {
            mEndpointOut = usbEndpoint;
          }
          mEndpointInFail = false;
          mEndpointOutFail = false;
        }
      }
    }
  }

  /**
   * Release old device before.
   */
  public void release() {
    Log.i(TAG, "release: ");
    if (mClaimed) {
      mUsbConnection.releaseInterface(mMassStoreIf);
      mUsbConnection.close();
      mEndpointIn = null;
      mEndpointOut = null;
      mMassStoreIf = null;
      mUsbConnection = null;
    }
    mClaimed = false;
    mEndpointInFail = true;
    mEndpointOutFail = true;
  }

  public static final String BULK_TRANSFER_SUCCESS = "BULK_TRANSFER_SUCCESS";
  public static final String BULK_TRANSFER_FAIL = "BULK_TRANSFER_FAIL";
  public static final int FAIL_RESULT_BULK_TRANSFER = -1;

  /**
   * @param sendData send data to device.
   * @return success BULK_TRANSFER_SUCCESS, fail BULK_TRANSFER_FAIL.
   */
  private String sendBulkTransfer(String sendData) {
    if (mEndpointOut == null) {
      return BULK_TRANSFER_FAIL;
    }

    byte[] cmd = new byte[]{
            (byte) 0x55, (byte) 0x53, (byte) 0x42, (byte) 0x43, //[0:3] Signature
            (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00, //[4:7] Tag
            (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00, //[8:11] DataTransferLength
            (byte) 0x00, //[12] Flag , 0x80 IN; 0x00 OUT dir
            (byte) 0x00, //[13] Lun
            (byte) 0x10, //[14] Length of the CDB
            //[15:30] CDB, [21:22] DataTransferLength in CDB
            (byte) 0xf6, (byte) 0x04, (byte) 0x00, (byte) 0x00,
            (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00,
            (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00,
            (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00,
    };

    byte[] data = sendData.getBytes();
    cmd[4] = (byte) (CBW_TAG & 0xff);
    cmd[5] = (byte) (CBW_TAG >> 8 & 0xff);
    cmd[6] = (byte) (CBW_TAG >> 16 & 0xff);
    cmd[7] = (byte) (CBW_TAG >> 24 & 0xff);
    CBW_TAG++;
    cmd[8] = (byte) (data.length & 0xff);
    cmd[9] = (byte) (data.length >> 8 & 0xff);
    cmd[21] = cmd[8];
    cmd[22] = cmd[9];

    synchronized (mSync) {
      int result = mUsbConnection.bulkTransfer(mEndpointOut, cmd, cmd.length, 1000);
      if (result < 0) {
        bulkTransferFail(mEndpointOut);
        return BULK_TRANSFER_FAIL;
      }

      result = mUsbConnection.bulkTransfer(mEndpointOut, data, data.length, 1000);
      if (result < 0) {
        bulkTransferFail(mEndpointOut);
        return BULK_TRANSFER_FAIL;
      }

      byte[] csw = new byte[64];
      result = mUsbConnection.bulkTransfer(mEndpointIn, csw, csw.length, 1000);
      if (result < 0) {
        bulkTransferFail(mEndpointIn);
        return BULK_TRANSFER_FAIL;
      }

      if (csw[4] == cmd[4] && csw[5] == cmd[5] && csw[6] == cmd[6] && csw[7] == cmd[7]) {
        return BULK_TRANSFER_SUCCESS;
      }
    }
    return BULK_TRANSFER_FAIL;
  }

  public enum RECEIVE_TYPE {RESPONSE, OTHER, IMG_DATA}
  /**
   * @param receiveType Select type of receive.
   * @return success receive data from device, fail return BULK_TRANSFER_FAIL.
   */
  private String receiveBulkTransfer(RECEIVE_TYPE receiveType) {
    if(mEndpointIn == null) {
      return BULK_TRANSFER_FAIL;
    }

    int dataLen = 0;

    byte[] cmd = new byte[]{
            (byte) 0x55, (byte) 0x53, (byte) 0x42, (byte) 0x43,  //[0:3] Signature
            (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00,  //[4:7] Tag
            (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00,  //[8:11] DataTransferLength
            (byte) 0x80,  //[12] Flag , 0x80 IN; 0x00 OUT dir
            (byte) 0x00,  //[13] Lun
            (byte) 0x10,  //[14] Length of the CDB
            //[15:30] CDB, [21:22] DataTransferLength in CDB
            (byte) 0xf6, (byte) 0x04, (byte) 0x00, (byte) 0x00,
            (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00,
            (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00,
            (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00,
    };

    cmd[4] = (byte) (CBW_TAG & 0xff);
    cmd[5] = (byte) (CBW_TAG >> 8 & 0xff);
    cmd[6] = (byte) (CBW_TAG >> 16 & 0xff);
    cmd[7] = (byte) (CBW_TAG >> 24 & 0xff);
    CBW_TAG++;

    if (receiveType == RECEIVE_TYPE.RESPONSE) {
      cmd[8] = (byte) (RESPONSE_RECV_DATA_LEN & 0xff);
      cmd[9] = (byte) (RESPONSE_RECV_DATA_LEN >> 8 & 0xff);
      dataLen = RESPONSE_RECV_DATA_LEN;
    } else if (receiveType == RECEIVE_TYPE.OTHER) {
      cmd[8] = (byte) (PERSON_LIST_RECV_DATA_LEN & 0xff);
      cmd[9] = (byte) (PERSON_LIST_RECV_DATA_LEN >> 8 & 0xff);
      dataLen = PERSON_LIST_RECV_DATA_LEN;
    } else if (receiveType == RECEIVE_TYPE.IMG_DATA) {
      cmd[8] = (byte) (IMG_RECV_DATA_LEN_TEMP & 0xff);
      cmd[9] = (byte) (IMG_RECV_DATA_LEN_TEMP >> 8 & 0xff);
//      dataLen = IMG_RECV_DATA_LEN_ALL + PERSON_LIST_RECV_DATA_LEN;
      dataLen = IMG_RECV_DATA_LEN_TEMP;
    }

    cmd[21] = cmd[8];
    cmd[22] = cmd[9];

    if (receiveType == RECEIVE_TYPE.RESPONSE) {
      cmd[25] = 1;
    } else if (receiveType == RECEIVE_TYPE.IMG_DATA) {
      cmd[25] = 2;
    }

    synchronized (mSync) {
      int result = mUsbConnection.bulkTransfer(mEndpointOut, cmd, cmd.length, 1000);
      if (result < 0) {
        bulkTransferFail(mEndpointOut);
        return BULK_TRANSFER_FAIL;
      }

      byte[] data = new byte[dataLen];

      result = mUsbConnection.bulkTransfer(mEndpointIn, data, data.length, 1000);
      if (result < 0) {
        bulkTransferFail(mEndpointIn);
        return BULK_TRANSFER_FAIL;
      }

      byte[] csw = new byte[64];
      result = mUsbConnection.bulkTransfer(mEndpointIn, csw, csw.length, 1000);
      if (result < 0) {
        bulkTransferFail(mEndpointIn);
        return BULK_TRANSFER_FAIL;
      }

      if (csw[4] == cmd[4] && csw[5] == cmd[5] && csw[6] == cmd[6] && csw[7] == cmd[7]) {
        return new String(data);
      }
    }

    return BULK_TRANSFER_FAIL;
  }

//  private byte[] receiveImgData() {
//    if(mEndpointIn == null) {
//      return null;
//    }
//
//    int dataLen;
//
//    byte[] cmd = new byte[]{
//            (byte) 0x55, (byte) 0x53, (byte) 0x42, (byte) 0x43,  //[0:3] Signature
//            (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00,  //[4:7] Tag
//            (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00,  //[8:11] DataTransferLength
//            (byte) 0x80,  //[12] Flag , 0x80 IN; 0x00 OUT dir
//            (byte) 0x00,  //[13] Lun
//            (byte) 0x10,  //[14] Length of the CDB
//            //[15:30] CDB, [21:22] DataTransferLength in CDB
//            (byte) 0xf6, (byte) 0x04, (byte) 0x00, (byte) 0x00,
//            (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00,
//            (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00,
//            (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00,
//    };
//
//    cmd[4] = (byte) (CBW_TAG & 0xff);
//    cmd[5] = (byte) (CBW_TAG >> 8 & 0xff);
//    cmd[6] = (byte) (CBW_TAG >> 16 & 0xff);
//    cmd[7] = (byte) (CBW_TAG >> 24 & 0xff);
//    CBW_TAG++;
//
//    cmd[8] = (byte) (IMG_RECV_DATA_LEN_TEMP & 0xff);
//    cmd[9] = (byte) (IMG_RECV_DATA_LEN_TEMP >> 8 & 0xff);
//    dataLen = IMG_RECV_DATA_LEN_TEMP;
//
//    cmd[21] = cmd[8];
//    cmd[22] = cmd[9];
//
//    cmd[25] = 2;
//
//    synchronized (mSync) {
//      int result = mUsbConnection.bulkTransfer(mEndpointOut, cmd, cmd.length, 1000);
//      if (result < 0) {
//        bulkTransferFail(mEndpointOut);
//        return null;
//      }
//
//      byte[] data = new byte[dataLen];
//
//      result = mUsbConnection.bulkTransfer(mEndpointIn, data, data.length, 1000);
//      if (result < 0) {
//        bulkTransferFail(mEndpointIn);
//        return null;
//      }
//
//      byte[] csw = new byte[64];
//      result = mUsbConnection.bulkTransfer(mEndpointIn, csw, csw.length, 1000);
//      if (result < 0) {
//        bulkTransferFail(mEndpointIn);
//        return null;
//      }
//
//      if (csw[4] == cmd[4] && csw[5] == cmd[5] && csw[6] == cmd[6] && csw[7] == cmd[7]) {
//        return data;
//      }
//    }
//
//    return null;
//  }

  private void bulkTransferFail(UsbEndpoint endpoint) {
    if (endpoint == mEndpointOut) {
      if (!mEndpointOutFail) {
        Log.i(TAG, "bulkTransferFail: mEndpointOutFail");
        mEndpointOutFail = true;
      }
    } else if (endpoint == mEndpointIn) {
      if (!mEndpointInFail) {
        Log.i(TAG, "bulkTransferFail: mEndpointInFail");
        mEndpointInFail = true;
      }
    }
  }

  private static final int RECV_WAIT_TIEM = 200;
  private static final int RECV_CYCLES_NUM = 20;

  /**
   * Register one user by name.
   *
   * @param name The name will be registered.
   * @return success return register status from device,
   * fail return BULK_TRANSFER_FAIL.
   */
  public int registerFaceName(String name) {
    if (!mClaimed) {
      Log.i(TAG, "registerFaceName: no device claimed");
      return FAIL_RESULT_BULK_TRANSFER;
    }

    JSONObject root = new JSONObject();
    String cmd_id = getCmdId();

    try {
      JSONObject header = new JSONObject();
      header.put("type", 0);

      root.put("cmd_id", cmd_id);

      JSONObject register_info = new JSONObject();
      register_info.put("name", name);

      root.put("header", header);
      root.put("register_info", register_info);
    } catch (JSONException e) {
      e.printStackTrace();
    }

    String sendResult = sendBulkTransfer(root.toString());

    if (sendResult.equals(BULK_TRANSFER_SUCCESS)) {
      for (int i = 0; i < RECV_CYCLES_NUM; i++) {
        try {
          Thread.sleep(RECV_WAIT_TIEM);
        } catch (InterruptedException e) {
          e.printStackTrace();
        }

        String receiveResult = receiveBulkTransfer(RECEIVE_TYPE.RESPONSE);
//        Log.d(TAG, receiveResult);
        if (receiveResult.equals(BULK_TRANSFER_FAIL)) {
          continue;
        }

        try {
          JSONObject receiveJson = new JSONObject(receiveResult);

          int type = receiveJson.getJSONObject("header").getInt("type");
          if (type != 0) {
            Log.w(TAG, "registerFaceName: receive error type is " + type);
            continue;
          }

          String id = receiveJson.getString("cmd_id");
          if (!id.equals(cmd_id)) {
            Log.w(TAG, "registerFaceName: send cmd id is: " + cmd_id + ", receive is: " + id);
            continue;
          }

          return receiveJson.getJSONObject("register_info").getInt("reg_status");

        } catch (JSONException e) {
          e.printStackTrace();
        }
      }
    } else {
      Log.e(TAG, "registerFaceName: sendBulkTransfer fail!");
    }
    return FAIL_RESULT_BULK_TRANSFER;
  }

  /**
   * Unregister one user by name.
   *
   * @param name The name will be unregistered.
   * @return success return unregister status from device,
   * fail return BULK_TRANSFER_FAIL.
   */
  public int unregisterFaceName(String name) {
    if (!mClaimed) {
      Log.i(TAG, "unregisterFaceName: no device claimed");
      return FAIL_RESULT_BULK_TRANSFER;
    }

    String cmd_id = getCmdId();
    JSONObject root = new JSONObject();
    try {
      JSONObject header = new JSONObject();
      header.put("type", 1);

      root.put("cmd_id", cmd_id);

      JSONObject unregister_info = new JSONObject();
      unregister_info.put("name", name);

      root.put("header", header);
      root.put("unregister_info", unregister_info);
    } catch (JSONException e) {
      e.printStackTrace();
    }

    String resultSend = sendBulkTransfer(root.toString());

    if (resultSend.equals(BULK_TRANSFER_SUCCESS)) {
      for (int i = 0; i < RECV_CYCLES_NUM; i++) {
        try {
          Thread.sleep(RECV_WAIT_TIEM);
        } catch (InterruptedException e) {
          e.printStackTrace();
        }
        String receiveResult = receiveBulkTransfer(RECEIVE_TYPE.RESPONSE);
//                Log.d(TAG, receiveResult);
        if (receiveResult.equals(BULK_TRANSFER_FAIL)) {
          continue;
        }

        try {
          JSONObject receiveJson = new JSONObject(receiveResult);

          int type = receiveJson.getJSONObject("header").getInt("type");
          if (type != 1) {
            Log.w(TAG, "unregisterFaceName: receive error type is " + type);
            continue;
          }

          String id = receiveJson.getString("cmd_id");
          if (!id.equals(cmd_id)) {
            Log.w(TAG, "unregisterFaceName: send cmd id is: " + cmd_id + ", receive is: " + id);
            continue;
          }

          return receiveJson.getJSONObject("unregister_info").getInt("unreg_status");

        } catch (JSONException e) {
          e.printStackTrace();
        }

      }
    } else {
      Log.e(TAG, "unregisterFaceName: sendBulkTransfer Fail!");
    }

    return FAIL_RESULT_BULK_TRANSFER;
  }

  /**
   * Unregister all users.
   *
   * @return success return unregister all user status from device,
   * fail return BULK_TRANSFER_FAIL.
   */
  public int unregisterAllUser() {
    if (!mClaimed) {
      Log.i(TAG, "unregisterFaceName: no device claimed");
      return FAIL_RESULT_BULK_TRANSFER;
    }

    String cmd_id = getCmdId();
    JSONObject root = new JSONObject();
    try {
      JSONObject header = new JSONObject();
      header.put("type", 1);

      root.put("cmd_id", cmd_id);

      JSONObject unregister_info = new JSONObject();
      unregister_info.put("delete_all", "");

      root.put("header", header);
      root.put("unregister_info", unregister_info);
    } catch (JSONException e) {
      e.printStackTrace();
    }

    String resultSend = sendBulkTransfer(root.toString());

    if (resultSend.equals(BULK_TRANSFER_SUCCESS)) {
      for (int i = 0; i < RECV_CYCLES_NUM; i++) {
        try {
          Thread.sleep(RECV_WAIT_TIEM);
        } catch (InterruptedException e) {
          e.printStackTrace();
        }
        String receiveResult = receiveBulkTransfer(RECEIVE_TYPE.RESPONSE);
//        Log.d(TAG, receiveResult);
        if (receiveResult.equals(BULK_TRANSFER_FAIL)) {
          continue;
        }

        try {
          JSONObject receiveJson = new JSONObject(receiveResult);

          int type = receiveJson.getJSONObject("header").getInt("type");
          if (type != 1) {
            Log.w(TAG, "unregisterAllUser: receive error type is " + type);
            continue;
          }

          String id = receiveJson.getString("cmd_id");
          if (!id.equals(cmd_id)) {
            Log.w(TAG, "unregisterAllUser: send cmd id is: " + cmd_id + ", receive is: " + id);
            continue;
          }
          Log.i(TAG, "unregisterAllUser: result: "
                  + receiveJson.getJSONObject("unregister_info").toString());

          return receiveJson.getJSONObject("unregister_info").getInt("unreg_status");

        } catch (JSONException e) {
          e.printStackTrace();
        }

      }
    } else {
      Log.e(TAG, "unregisterFaceName: sendBulkTransfer Fail!");
    }

    return FAIL_RESULT_BULK_TRANSFER;
  }

  /**
   * Gets the current person list.
   *
   * @return The current person list.
   */
  public List<Person> getPersonList() {
    List<Person> list = new ArrayList<>();
    if (!mClaimed) {
      return list;
    }

    String receiveResult = receiveBulkTransfer(RECEIVE_TYPE.OTHER);
//    Log.d(TAG, receiveResult);
    if (receiveResult.equals(BULK_TRANSFER_FAIL)) {
      return list;
    }

    try {
      JSONObject receiveJson = new JSONObject(receiveResult);

      int type = receiveJson.getJSONObject("header").getInt("type");
      if (type != 2) {
        Log.w(TAG, "getPersonList: receive error type is " + type);
//        Log.d(TAG, receiveResult.substring(0,100));
        return list;
      }

      int face_num = receiveJson.getInt("face_num");
//      Log.i(TAG, "getPersonList: face num: " + face_num);

      JSONArray face_info = receiveJson.getJSONArray("face_info");

      for (int j = 0; j < face_num; j++) {
        Person person = new Person();
        int[] rect = new int[4];

        rect[0] = face_info.getJSONObject(j).getInt("x0");
        rect[1] = face_info.getJSONObject(j).getInt("y0");
        rect[2] = face_info.getJSONObject(j).getInt("x1");
        rect[3] = face_info.getJSONObject(j).getInt("y1");
        person.setRect(rect);

        person.setName(face_info.getJSONObject(j).getString("name"));
        person.setLiving(face_info.getJSONObject(j).getInt("living"));
        person.setRecg(face_info.getJSONObject(j).getInt("recg"));
        person.setMale(face_info.getJSONObject(j).getInt("male"));

        String[] head_pose = new String[3];
        head_pose[0] = face_info.getJSONObject(j).getString("head_pose[0]");
        head_pose[1] = face_info.getJSONObject(j).getString("head_pose[1]");
        head_pose[2] = face_info.getJSONObject(j).getString("head_pose[2]");
        person.setHead_pose(head_pose);

        String[] kpts = new String[10];
        kpts[0] = face_info.getJSONObject(j).getString("kpts[0]");
        kpts[1] = face_info.getJSONObject(j).getString("kpts[1]");
        kpts[2] = face_info.getJSONObject(j).getString("kpts[2]");
        kpts[3] = face_info.getJSONObject(j).getString("kpts[3]");
        kpts[4] = face_info.getJSONObject(j).getString("kpts[4]");
        kpts[5] = face_info.getJSONObject(j).getString("kpts[5]");
        kpts[6] = face_info.getJSONObject(j).getString("kpts[6]");
        kpts[7] = face_info.getJSONObject(j).getString("kpts[7]");
        kpts[8] = face_info.getJSONObject(j).getString("kpts[8]");
        kpts[9] = face_info.getJSONObject(j).getString("kpts[9]");
        person.setKpts(kpts);

        list.add(person);
//        Log.d(TAG, person.toString());
      }

    } catch (JSONException e) {
//            e.printStackTrace();
    } catch (NullPointerException e) {
//            e.printStackTrace();
    }

    return list;
  }

  /**
   * Gets uvcConfigInfo {@link UVCConfigInfo}.
   *
   * @return success return current uvcConfigInfo, fail return null.
   */
  @Nullable
  public UVCConfigInfo getUVCConfigInfo() {
    if (!mClaimed) {
      Log.i(TAG, "UVCConfigInfo: no device claimed");
      return null;
    }

    JSONObject root = new JSONObject();
    String cmd_id = getCmdId();

    try {
      JSONObject header = new JSONObject();
      header.put("type", 3);
      root.put("header", header);

      root.put("cmd_id", cmd_id);

      root.put("get_init_info", 1);
    } catch (JSONException e) {
      e.printStackTrace();
    }

    String resultSend = sendBulkTransfer(root.toString());

    if (resultSend.equals(BULK_TRANSFER_SUCCESS)) {
      for (int i = 0; i < RECV_CYCLES_NUM; i++) {
        try {
          Thread.sleep(RECV_WAIT_TIEM);
        } catch (InterruptedException e) {
          e.printStackTrace();
        }

        String receiveResult = receiveBulkTransfer(RECEIVE_TYPE.RESPONSE);
//                Log.d(TAG, receiveResult);
        if (receiveResult.equals(BULK_TRANSFER_FAIL)) {
          continue;
        }

        try {
          JSONObject receiveJson = new JSONObject(receiveResult);
          int type = receiveJson.getJSONObject("header").getInt("type");
          if (type != 3) {
            Log.w(TAG, "getUVCConfigInfo: receive error type is " + type);
            continue;
          }

          String id = receiveJson.getString("cmd_id");
          if (!id.equals(cmd_id)) {
            Log.w(TAG, "getUVCConfigInfo: send cmd id is: " + cmd_id + ", receive is: " + id);
            continue;
          }

          JSONObject infoJson = receiveJson.getJSONObject("get_init_info");

          UVCConfigInfo uvcConfigInfo = new UVCConfigInfo();
          uvcConfigInfo.setProduct_name(infoJson.getString("product_name"));
          uvcConfigInfo.setVersion(infoJson.getString("version"));
          uvcConfigInfo.setLiving_detect(infoJson.getInt("living_detect"));
          uvcConfigInfo.setMax_track_faces(infoJson.getInt("max_track_faces"));
          uvcConfigInfo.setMax_face_pixel(infoJson.getInt("max_face_pixel"));
          uvcConfigInfo.setMin_face_pixel(infoJson.getInt("min_face_pixel"));
          uvcConfigInfo.setRecg_threshold(infoJson.getString("recg_threshold"));
          Log.i(TAG, "getUVCConfigInfo: uvcConfigInfo: " + uvcConfigInfo.toString());
          return uvcConfigInfo;

        } catch (JSONException e) {
          e.printStackTrace();
        }
      }
    } else {
      Log.e(TAG, "getUVCConfigInfo: sendBulkTransfer Fail!");
    }
    return null;
  }

  /**
   * Sets uvcConfigInfo {@link UVCConfigInfo}.
   *
   * @param uvcConfigInfo The desired uvcConfigInfo.
   * @return success return 0, fail return BULK_TRANSFER_FAIL, besides return
   * the parameter setting status from device.
   */
  public int setUVCConfig(UVCConfigInfo uvcConfigInfo) {
    if (!mClaimed) {
      Log.i(TAG, "setUVCConfig: no device claimed");
      return FAIL_RESULT_BULK_TRANSFER;
    }

    JSONObject root = new JSONObject();
    String cmd_id = getCmdId();

    try {
      JSONObject header = new JSONObject();
      header.put("type", 3);
      root.put("header", header);

      root.put("cmd_id", cmd_id);

      JSONObject recg_para_info = new JSONObject();
      recg_para_info.put("living_detect", uvcConfigInfo.getLiving_detect());
      recg_para_info.put("max_track_faces", uvcConfigInfo.getMax_track_faces());
      recg_para_info.put("max_face_pixel", uvcConfigInfo.getMax_face_pixel());
      recg_para_info.put("min_face_pixel", uvcConfigInfo.getMin_face_pixel());
      recg_para_info.put("recg_threshold", uvcConfigInfo.getRecg_threshold());
      root.put("recg_para_info",recg_para_info);
    } catch (JSONException e) {
      e.printStackTrace();
    }
    String sendResult = sendBulkTransfer(root.toString());

    if (sendResult.equals(BULK_TRANSFER_SUCCESS)) {
      for (int i = 0; i < RECV_CYCLES_NUM; i++) {
        try {
          Thread.sleep(RECV_WAIT_TIEM);
        } catch (InterruptedException e) {
          e.printStackTrace();
        }

        String receiveResult = receiveBulkTransfer(RECEIVE_TYPE.RESPONSE);
//          Log.d(TAG, receiveResult);
        if (receiveResult.equals(BULK_TRANSFER_FAIL)) {
          continue;
        }

        try {
          JSONObject receiveJson = new JSONObject(receiveResult);
          int type = receiveJson.getJSONObject("header").getInt("type");
          if (type != 3) {
            Log.w(TAG, "getUVCConfigInfo: receive error type is " + type);
            continue;
          }

          String id = receiveJson.getString("cmd_id");
          if (!id.equals(cmd_id)) {
            Log.w(TAG, "getUVCConfigInfo: send cmd id is: " + cmd_id + ", receive is: " + id);
            continue;
          }

//          Log.d(TAG, receiveResult);

          JSONObject infoJson = receiveJson.getJSONObject("get_init_info");
          Log.i(TAG, "setUVCConfig: resultJson: " + infoJson.toString());
          if (uvcConfigInfo.getMax_track_faces() == infoJson.getInt("max_track_faces")
                  && uvcConfigInfo.getMax_face_pixel() == infoJson.getInt("max_face_pixel")
                  && uvcConfigInfo.getMin_face_pixel() == infoJson.getInt("min_face_pixel")
                  && uvcConfigInfo.getRecg_threshold().equals(infoJson.getString("recg_threshold"))) {
            Log.d(TAG, "setUVCConfig: success!");
            return 0;
          } else {
            return infoJson.getInt("set_para_status");
          }
        } catch (JSONException e) {
          e.printStackTrace();
        }
      }
    } else {
      Log.e(TAG, "setUVCConfig: sendBulkTransfer Fail!");
    }
    return FAIL_RESULT_BULK_TRANSFER;
  }

  /**
   * @deprecated no used
   */
  public RegisterImgInfo getLastRegisterImgInfo() {
    if (!mClaimed) {
      Log.i(TAG, "getLastRegisterImgInfo: no device claimed");
      return null;
    }

    JSONObject root = new JSONObject();
    String cmd_id = getCmdId();

    try {
      JSONObject header = new JSONObject();
      header.put("type", 4);
      root.put("header", header);

      root.put("cmd_id", cmd_id);

//      JSONObject export_register_img = new JSONObject();
//      export_register_img.put("get_img_size", 1);
//      root.put("export_register_img", export_register_img);

    } catch (JSONException e) {
      e.printStackTrace();
    }

    String resultSend = sendBulkTransfer(root.toString());

    if (resultSend.equals(BULK_TRANSFER_SUCCESS)) {
      for (int i = 0; i < RECV_CYCLES_NUM; i++) {
        try {
          Thread.sleep(RECV_WAIT_TIEM);
        } catch (InterruptedException e) {
          e.printStackTrace();
        }

        String receiveResult = receiveBulkTransfer(RECEIVE_TYPE.RESPONSE);
//        Log.d(TAG, receiveResult);
        if (receiveResult.equals(BULK_TRANSFER_FAIL)) {
          continue;
        }

        try {
          JSONObject receiveJson = new JSONObject(receiveResult);
          int type = receiveJson.getJSONObject("header").getInt("type");
          if (type != 4) {
            Log.w(TAG, "getLastRegisterImgInfo: receive error type is " + type);
            continue;
          }

          String id = receiveJson.getString("cmd_id");
          if (!id.equals(cmd_id)) {
            Log.w(TAG, "getLastRegisterImgInfo: send cmd id is: " + cmd_id + ", receive is: " + id);
            continue;
          }

          JSONObject infoJson = receiveJson.getJSONObject("export_register_img");

          RegisterImgInfo registerImgInfo = new RegisterImgInfo();

//          Log.d(TAG, receiveResult);
          if (infoJson.has("have_not_reg_person")) {
            registerImgInfo.setLatest_reg_img_size(-1);
            return registerImgInfo;
          }

          registerImgInfo.setLatest_reg_img_path(infoJson.getString("latest_reg_img_path"));
          registerImgInfo.setLatest_reg_img_name(infoJson.getString("latest_reg_img_name"));
          registerImgInfo.setLatest_reg_img_size(infoJson.getInt("latest_reg_img_size"));
          Log.i(TAG, "getLastRegisterImgData: registerImgInfo: " + registerImgInfo.toString());

          IMG_RECV_DATA_LEN_ALL = registerImgInfo.getLatest_reg_img_size();
          return registerImgInfo;
        } catch (JSONException e) {
          e.printStackTrace();
        }
      }
    } else {
      Log.e(TAG, "getLastRegisterImgInfo: sendBulkTransfer Fail!");
    }
    return null;
  }

  /**
   * @deprecated no used
   */
  public boolean getRegisterImgByAdb(String srcImg) {
//    String cmd = "adb pull /data/face_register/19700101/Li-large.jpg /mnt/sdcard/face-register/";
    boolean result = false;
    if (!checkDesPath()) {
      Log.e(TAG, "getRegisterImgByAdb: no des path");
      return false;
    }

    String cmd = "adb pull " + srcImg + " " + DES_PATH;
    BufferedReader reader = null;
    Process process = null;
    try {
      Log.i(TAG, "getRegisterImg: cmd: " + cmd);
      process = Runtime.getRuntime().exec(cmd);
      // 原本想靠getInputStream判断结果，但error时需要获取process.getErrorStream()
//      reader = new BufferedReader(new InputStreamReader(process.getInputStream()));
//      String line = reader.readLine();
//      while (line != null) {
//        Log.i(TAG, "getRegisterImg: line: " + line);
//        if (line.contains("bytes reader")) {
//          result = true;
//        }
//        line = reader.readLine();
//      }

      int value = process.waitFor();
      Log.i(TAG, "getRegisterImgByAdb: value: " + value);
      result = value == 0;

    } catch (IOException e) {
      e.printStackTrace();
    } catch (InterruptedException e) {
      e.printStackTrace();
    } finally {
      if (reader != null) {
        try {
          reader.close();
        } catch (IOException e) {
          e.printStackTrace();
        }
      }

      if (process != null) {
        process.destroy();
      }
    }
    return result;
  }

  public static final String DES_PATH = "/mnt/sdcard/face-register/";
  public static boolean checkDesPath() {
    File file = new File(DES_PATH);
    if (!file.exists()) {
      Log.i(TAG, "checkDesPath: make dir!");
      return file.mkdir();
    }
    return true;
  }

  /**
   * Called when picture taken.
   *
   * @param img  Image name.
   * @param data Image data.
   *
   * @return {@code true} success to save image. {@code false} path does not exist
   * {@link #DES_PATH}, or throw IOException.
   */
  public boolean saveRegisterImg(String img, byte[] data) {
    if (!checkDesPath()) {
      return false;
    }

    File file = new File(DES_PATH, img + ".jpg");
    try (OutputStream os = new FileOutputStream(file)) {
      os.write(data);
      os.close();
      return true;
    } catch (IOException e) {
      Log.w(TAG, "Cannot write to " + file, e);
      return false;
    }
  }

  /**
   * Called when delete register picture.
   *
   * @param img Deleted file name.
   */
  public boolean deleteRegisterImg(String img) {
    File file = new File(DES_PATH, img + ".jpg");
    if (file.exists() && file.isFile()) {
      return file.delete();
    }
    return true;
  }

  /**
   * Called when delete all registers picture.
   */
  public boolean deleteAllRegisteredPictures() {
    File rootFile = new File(DES_PATH);
    if (!rootFile.exists() || !rootFile.isDirectory()) {
      Log.w(TAG, "deleteAllRegisteredPictures: DES_PATH is null");
      return true;
    }

    File[] files = rootFile.listFiles();
    if (files != null) {
      for (File file : files) {
        deleteFile(file);
      }
    }
    return true;
  }

  public void deleteFile(File file) {
    if (file.isDirectory()) {
      for (File f : Objects.requireNonNull(file.listFiles())) {
        deleteFile(f);
      }
    }
    file.delete();
  }

  /**
   * Gets update flag.
   *
   * @return The current {@link #mUpdate}.
   */
  public boolean isUpdate() {
    return mUpdate;
  }

  /**
   * Sets update flag.
   *
   * @param update The {@link #mUpdate} to be set.
   */
  public void setUpdate(boolean update) {
    this.mUpdate = update;
  }

  private boolean mUpdate = false;
  /**
   * Sends update command.
   *
   * @return {@code true} success to send update command. {@code false} if no device
   * claimed, or if sendBulkTransfer fail.
   */
  public boolean updateUVC() {
    if (!mClaimed) {
      Log.i(TAG, "updateUVC: no device claimed");
      return false;
    }

    JSONObject root = new JSONObject();
    // String cmd_id = getCmdId();

    try {
      JSONObject header = new JSONObject();
      header.put("type", 5);
      root.put("header", header);

      // root.put("cmd_id", cmd_id);
      root.put("recv_ota_file_success", 1);

    } catch (JSONException e) {
      e.printStackTrace();
    }

    String resultSend = sendBulkTransfer(root.toString());

    if (resultSend.equals(BULK_TRANSFER_SUCCESS)) {
      Log.i(TAG, "updateUVC: success!");
      return true;
    } else {
      Log.e(TAG, "updateUVC: sendBulkTransfer Fail!");
      return false;
    }
  }

//  public byte[] getLastRegisterImgData() {
//    if (!mClaimed) {
//      Log.i(TAG, "getLastRegisterImgData: no device claimed");
//      return null;
//    }
//
//    Log.i(TAG, "getLastRegisterImgData: all img size: " + IMG_RECV_DATA_LEN_ALL);
//
//    byte[] result = new byte[0];
//
//    while (IMG_RECV_DATA_LEN_ALL > 0) {
//      if (IMG_RECV_DATA_LEN_ALL > MAX_BULK_TRANSFER) {
//        IMG_RECV_DATA_LEN_TEMP = MAX_BULK_TRANSFER;
//        IMG_RECV_DATA_LEN_ALL -= MAX_BULK_TRANSFER;
//      } else {
//        IMG_RECV_DATA_LEN_TEMP = IMG_RECV_DATA_LEN_ALL;
//        IMG_RECV_DATA_LEN_ALL = 0;
//      }
//
//      Log.i(TAG, "getLastRegisterImgData: IMG_RECV_DATA_LEN_TEMP: " + IMG_RECV_DATA_LEN_TEMP);
//
//      JSONObject root = new JSONObject();
//      String cmd_id = getCmdId();
//
//      try {
//        JSONObject header = new JSONObject();
//        header.put("type", 4);
//        root.put("header", header);
//
//        root.put("cmd_id", cmd_id);
//
//        JSONObject export_register_img = new JSONObject();
//        export_register_img.put("read_img_size", IMG_RECV_DATA_LEN_TEMP);
//        root.put("export_register_img", export_register_img);
//
//      } catch (JSONException e) {
//        e.printStackTrace();
//      }
//
//      String resultSend = sendBulkTransfer(root.toString());
//
//      if (resultSend.equals(BULK_TRANSFER_SUCCESS)) {
//
//        for (int i = 0; i < RECV_CYCLES_NUM; i++) {
//          try {
//            Thread.sleep(RECV_WAIT_TIEM);
//          } catch (InterruptedException e) {
//            e.printStackTrace();
//          }
//
//          byte[] receiveResult = receiveImgData();
////                Log.d(TAG, receiveResult);
//          if (receiveResult == null) {
//            Log.i(TAG, "getLastRegisterImgData: BULK_TRANSFER_FAIL");
//            continue;
//          }
//
//          try {
//            JSONObject receiveJson = new JSONObject(new String(receiveResult));
//            if (receiveJson.has("header")) {
//              Log.e(TAG, "getLastRegisterImgData: has header type");
//              continue;
//            }
//          } catch (JSONException e) {
////            e.printStackTrace();
//          }
//
//          result = byteArrayMerge(result, receiveResult);
//          Log.i(TAG, "getLastRegisterImgData: result size: " + result.length);
//          break;
//        }
//
//
//      } else {
//        Log.e(TAG, "getLastRegisterImgData: sendBulkTransfer Fail!");
//      }
//    }
//
//    if (result.length > 0) {
//      return result;
//    }
//    getImgDataSuccess();
//    return null;
//  }

//  public boolean getImgDataSuccess() {
//    if (!mClaimed) {
//      Log.i(TAG, "getImgDataSuccess: no device claimed");
//      return false;
//    }
//
//    JSONObject root = new JSONObject();
//    String cmd_id = getCmdId();
//
//    try {
//      JSONObject header = new JSONObject();
//      header.put("type", 4);
//      root.put("header", header);
//
//      root.put("cmd_id", cmd_id);
//
//      JSONObject export_register_img = new JSONObject();
//      export_register_img.put("read_success", 1);
//      root.put("export_register_img", export_register_img);
//
//    } catch (JSONException e) {
//      e.printStackTrace();
//    }
//
//    String resultSend = sendBulkTransfer(root.toString());
//
//    return resultSend.equals(BULK_TRANSFER_SUCCESS);
//  }

  @NonNull
  private static String getCmdId() {
    long time = System.currentTimeMillis();
    return String.valueOf(time);
  }

  private static byte[] byteArrayMerge(byte[] source1, byte[] source2) {
    byte[] result = new byte[source1.length + source2.length];
    System.arraycopy(source1, 0, result, 0, source1.length);
    System.arraycopy(source2, 0, result, source1.length, source2.length);
    return result;
  }

}
