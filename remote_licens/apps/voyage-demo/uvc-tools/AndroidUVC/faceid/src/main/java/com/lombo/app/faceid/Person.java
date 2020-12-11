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

import android.content.Context;

import java.util.Arrays;

/**
 * Created by Administrator on 2018/7/26/026.
 */

public class Person {
    private String name;
    private int[] rect;
    private int living;
    private int recg;
    private int male;
    private String[] head_pose = new String[3];
    private String[] kpts = new String[10];

    public Person() {
    }


    public void setName(String name) {
        this.name = name;
    }

    public String getName() {
        return this.name;
    }

    public int[] getRect() {
        return rect;
    }

    public void setRect(int[] rect) {
        this.rect = rect;
    }

    public int getLiving() {
        return living;
    }

    public void setLiving(int living) {
        this.living = living;
    }

    public int getRecg() {
        return recg;
    }

    public void setRecg(int recg) {
        this.recg = recg;
    }

    public int getMale() {
        return male;
    }

    public void setMale(int male) {
        this.male = male;
    }

    public String[] getHead_pose() {
        return head_pose;
    }

    public void setHead_pose(String[] head_pose) {
        this.head_pose = head_pose;
    }

    public String[] getKpts() {
        return kpts;
    }

    public void setKpts(String[] kpts) {
        this.kpts = kpts;
    }

    @Override
    public String toString() {
        return "Person{" +
                "name='" + name + '\'' +
                ", rect=" + Arrays.toString(rect) +
                ", living=" + living +
                ", recg=" + recg +
                ", male=" + male +
                ", head_pose=" + Arrays.toString(head_pose) +
                ", kpts=" + Arrays.toString(kpts) +
                '}';
    }
}
