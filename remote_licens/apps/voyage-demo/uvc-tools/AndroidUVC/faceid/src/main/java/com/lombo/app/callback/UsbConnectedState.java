package com.lombo.app.callback;

public interface UsbConnectedState {

  /**
   * Usb device is connected.
   */
  void onConnected();

  /**
   * Usb device is detached.
   */
  void onDetached();

  /**
   * Usb device update failed.
   */
  void onUpdateFailed();
}
