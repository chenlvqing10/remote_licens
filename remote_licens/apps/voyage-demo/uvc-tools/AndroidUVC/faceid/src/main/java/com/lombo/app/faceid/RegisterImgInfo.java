package com.lombo.app.faceid;

public class RegisterImgInfo {
  /**
   * latest_reg_img_path: /data/face_register/
   * latest_reg_img_name : yd-small.jpg
   * latest_reg_img_size : 16000
   */

  private String latest_reg_img_path;
  private String latest_reg_img_name;
  private int latest_reg_img_size;

  public String getLatest_reg_img_path() {
    return latest_reg_img_path;
  }

  public void setLatest_reg_img_path(String latest_reg_img_path) {
    this.latest_reg_img_path = latest_reg_img_path;
  }

  public String getLatest_reg_img_name() {
    return latest_reg_img_name;
  }

  public void setLatest_reg_img_name(String latest_reg_img_name) {
    this.latest_reg_img_name = latest_reg_img_name;
  }

  public int getLatest_reg_img_size() {
    return latest_reg_img_size;
  }

  public void setLatest_reg_img_size(int latest_reg_img_size) {
    this.latest_reg_img_size = latest_reg_img_size;
  }

  @Override
  public String toString() {
    return "RegisterImgInfo{" +
            "latest_reg_img_path='" + latest_reg_img_path + '\'' +
            ", latest_reg_img_name='" + latest_reg_img_name + '\'' +
            ", latest_reg_img_size=" + latest_reg_img_size +
            '}';
  }
}
