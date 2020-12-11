//package com.lombo.app.broadcastreceiver;
//
//import android.content.BroadcastReceiver;
//import android.content.Context;
//import android.content.Intent;
//import android.hardware.usb.UsbDevice;
//import android.hardware.usb.UsbManager;
//
//public class UsbReceiver extends BroadcastReceiver {
//  @Override
//  public void onReceive(Context context, Intent intent) {
//    UsbDevice usbDevice = intent.getParcelableExtra(UsbManager.EXTRA_DEVICE);
//    LogUtil.i("usbDevice-->" + usbDevice);
//    if (!isTargetDevice(usbDevice) || mConnectCallback == null) {
//      return;
//    }
//
//    switch (intent.getAction()) {
//      case UsbManager.ACTION_USB_DEVICE_ATTACHED:
//        LogUtil.i("onAttached");
//        mConnectCallback.onAttached(usbDevice);
//        break;
//
//      case ACTION_USB_DEVICE_PERMISSION:
//        boolean granted = intent.getBooleanExtra(UsbManager.EXTRA_PERMISSION_GRANTED, false);
//        mConnectCallback.onGranted(usbDevice, granted);
//        LogUtil.i("onGranted-->" + granted);
//        break;
//
//      case UsbManager.ACTION_USB_DEVICE_DETACHED:
//        LogUtil.i("onDetached");
//        mConnectCallback.onDetached(usbDevice);
//        break;
//
//      default:
//        break;
//    }
//  }
//}
