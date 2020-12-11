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

public class UVCConfigInfo {

  /**
   * product_name : n7v1_voyage_uvc
   * version : v1.6
   * living_detect : 1
   * max_track_faces : 5
   * max_face_pixel : 512
   * min_face_pixel : 80
   * recg_threshold : 0.5
   * set_para_status: -1
   */

  private String product_name;
  private String version;
  private int living_detect;
  private int max_track_faces;
  private int max_face_pixel;
  private int min_face_pixel;
  private String recg_threshold;
  private int set_para_status;

  public int getLiving_detect() {
    return living_detect;
  }

  public void setLiving_detect(int living_detect) {
    this.living_detect = living_detect;
  }

  public int getMax_track_faces() {
    return max_track_faces;
  }

  public void setMax_track_faces(int max_track_faces) {
    this.max_track_faces = max_track_faces;
  }

  public int getMax_face_pixel() {
    return max_face_pixel;
  }

  public void setMax_face_pixel(int max_face_pixel) {
    this.max_face_pixel = max_face_pixel;
  }

  public int getMin_face_pixel() {
    return min_face_pixel;
  }

  public void setMin_face_pixel(int min_face_pixel) {
    this.min_face_pixel = min_face_pixel;
  }

  public String getRecg_threshold() {
    return recg_threshold;
  }

  public void setRecg_threshold(String recg_threshold) {
    this.recg_threshold = recg_threshold;
  }

  public String getProduct_name() {
    return product_name;
  }

  public void setProduct_name(String product_name) {
    this.product_name = product_name;
  }

  public String getVersion() {
    return version;
  }

  public void setVersion(String version) {
    this.version = version;
  }

  public int getSet_para_status() {
    return set_para_status;
  }

  public void setSet_para_status(int set_para_status) {
    this.set_para_status = set_para_status;
  }

  @Override
  public String toString() {
    return "UVCConfigInfo{" +
            "product_name='" + product_name + '\'' +
            ", version='" + version + '\'' +
            ", living_detect=" + living_detect +
            ", max_track_faces=" + max_track_faces +
            ", max_face_pixel=" + max_face_pixel +
            ", min_face_pixel=" + min_face_pixel +
            ", recg_threshold='" + recg_threshold + '\'' +
            ", set_para_status=" + set_para_status +
            '}';
  }
}
