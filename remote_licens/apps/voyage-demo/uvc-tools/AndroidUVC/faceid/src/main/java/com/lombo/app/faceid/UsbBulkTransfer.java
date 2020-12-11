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

package com.lombo.app.faceid;

import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbEndpoint;
import android.hardware.usb.UsbInterface;
import android.util.Log;


public class UsbBulkTransfer {
  private static final String TAG = UsbBulkTransfer.class.getSimpleName();
  private static int cbwtag = 0x70;

  private static final int MAX_RECV_DATA_LEN = 2560;

  private UsbDevice mUsbDevice;
  private UsbInterface mMassStoreIntf;
  private UsbEndpoint mEndpointIn;
  private UsbEndpoint mEndpointOut;
  private UsbDeviceConnection mUsbConnection;
  private final Object mSync = new Object();

  public boolean init(UsbDevice device, UsbDeviceConnection connection) {
    mUsbDevice = device;
    mUsbConnection = connection;
    for (int i = 0; i < mUsbDevice.getInterfaceCount(); i++) {
      UsbInterface intf = mUsbDevice.getInterface(i);
      if (intf.getEndpointCount() == 2) {
        mMassStoreIntf = intf;
        mUsbConnection.claimInterface(mMassStoreIntf, true);
        mEndpointIn = intf.getEndpoint(0);
        mEndpointOut = intf.getEndpoint(1);
        return true;
      }
    }
    return false;
  }

  public boolean deinit() {
    return mUsbConnection.releaseInterface(mMassStoreIntf);
  }

  public int send(String dataSend) {

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

    byte[] data = dataSend.getBytes();
    cmd[4] = (byte) (cbwtag & 0xff);
    cmd[5] = (byte) (cbwtag >> 8 & 0xff);
    cmd[6] = (byte) (cbwtag >> 16 & 0xff);
    cmd[7] = (byte) (cbwtag >> 24 & 0xff);
    cbwtag++;
    cmd[8] = (byte) (data.length & 0xff);
    cmd[9] = (byte) (data.length >> 8 & 0xff);
    cmd[21] = cmd[8];
    cmd[22] = cmd[9];

    if (mEndpointOut != null) {
      synchronized (mSync) {
        int result = mUsbConnection.bulkTransfer(mEndpointOut, cmd, cmd.length, 1000);

        if (result < 0) {
          Log.e(TAG,"send command error mEndpointOut is " + mEndpointOut);
        } else {
          result = mUsbConnection.bulkTransfer(mEndpointOut, data, data.length, 1000);

          if (result < 0) {
            Log.e(TAG,"send command error mEndpointOut is " + mEndpointOut);
          } else {
            byte[] csw = new byte[64];
            mUsbConnection.bulkTransfer(mEndpointIn, csw, csw.length, 1000);

            if (csw[4] == cmd[4] && csw[5] == cmd[5] && csw[6] == cmd[6] && csw[7] == cmd[7])
              return 0;
          }
        }
      }
    } else {
      Log.e(TAG,"mEndpointOut is null !");
    }

    return -1;
  }

  public String recv(RECV_TYPE recv_type) {
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

    cmd[4] = (byte) (cbwtag & 0xff);
    cmd[5] = (byte) (cbwtag >> 8 & 0xff);
    cmd[6] = (byte) (cbwtag >> 16 & 0xff);
    cmd[7] = (byte) (cbwtag >> 24 & 0xff);
    cbwtag++;
    cmd[8] = (byte) (MAX_RECV_DATA_LEN & 0xff);
    cmd[9] = (byte) (MAX_RECV_DATA_LEN >> 8 & 0xff);
    cmd[21] = cmd[8];
    cmd[22] = cmd[9];

    if (recv_type == RECV_TYPE.RESPONSE)
      cmd[25] = 1;

    if (mEndpointIn != null) {
      synchronized (mSync) {
        int result = mUsbConnection.bulkTransfer(mEndpointOut, cmd, cmd.length, 1000);

        if (result < 0) {
          Log.e(TAG,"recv command error mEndpointIn is " + mEndpointIn);
        } else {
          byte[] data = new byte[MAX_RECV_DATA_LEN];
//                    for (int i = 0; i < data.length; i++)
//                        data[i] = 0;

          result = mUsbConnection.bulkTransfer(mEndpointIn, data, data.length, 1000);

          String result_s = new String(data);
          if (result < 0) {
            Log.e(TAG,"recv command error mEndpointOut is " + mEndpointIn);
          } else {
            byte[] csw = new byte[64];
            mUsbConnection.bulkTransfer(mEndpointIn, csw, csw.length, 1000);
            if (csw[4] == cmd[4] && csw[5] == cmd[5] && csw[6] == cmd[6] && csw[7] == cmd[7])
              return result_s;
          }
        }
      }
    } else {
      Log.e(TAG,"mEndpointOut is null !");
    }
    return "";
  }

  public enum RECV_TYPE {RESPONSE, OTHER}
}
