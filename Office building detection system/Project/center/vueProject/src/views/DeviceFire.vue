<template>
  <el-row>
    <el-card class="device-card" style="margin-right:1vw">
      <el-row justify="center">
        <el-tooltip        
          class="box-item"
          effect="light"
          :content="fire_1?.value === 'true' ? '请及时处理消防异常' : '消防正常'"
          placement="right-start"
        >
          <img class="device-icon" v-if="fire_1?.value === 'true'" @click="toggleFire(1)" src="/fireOn.svg"/>
          <img class="device-icon" v-else @click="toggleFire(1)" src="/fireOff.svg"/>
        </el-tooltip>
      </el-row>
      <el-row justify="center">
        <span class="device-name">办公室①消防</span>
      </el-row>
    </el-card>
    <el-card class="device-card">
      <el-row justify="center">
        <el-tooltip        
          class="box-item"
          effect="light"
          :content="fire_1?.value === 'true' ? '请及时处理消防异常' : '消防正常'"
          placement="left-end"
        >
          <img class="device-icon" v-if="fire_2?.value === 'true'" @click="toggleFire(2)" src="/fireOn.svg"/>
          <img class="device-icon" v-else @click="toggleFire(2)" src="/fireOff.svg"/>
        </el-tooltip>
      </el-row>
      <el-row justify="center">
        <span class="device-name">办公室②消防</span>
      </el-row>
    </el-card>
  </el-row>
</template>

<script lang="ts" setup>
import { ref } from 'vue';
import axios from 'axios';
import { fire_1, fire_2 } from '@/api/getDevProp';

// 定义API URL和设备信息
const oneNetUrl = 'https://iot-api.heclouds.com/thingmodel/set-device-property';
const productId = 'OcFyEAzH58';
const deviceNames = ['office_1', 'office_2']; // 定义设备名称数组
const fireIdentifier = 'fire';

// 长期token
const token = 'version=2018-10-31&res=products%2FOcFyEAzH58%2Fdevices%2Foffice_1&et=1761720044&method=md5&sign=JFXruNyGm%2BFixVErw7Mmdg%3D%3D';

// 切换消防状态并调用API
function toggleFire(fireId: number) {
  const fire = fireId === 1 ? fire_1 : fire_2;
  const deviceName = deviceNames[fireId - 1]; // 根据fireId获取对应的deviceName
  let value = fire?.value?.value === 'true' ? false : true; // 切换状态

  // 构建API请求体，符合oneJson格式
  const data = {
    product_id: productId,
    device_name: deviceName,
    params: {
      [fireIdentifier]: value,
    },
  };

  console.log(data);

  // 发送POST请求到OneNet
  axios.post(oneNetUrl, data, {
    headers: {
      'Authorization': token, // 使用长期token进行鉴权
      'Content-Type': 'application/json', // 指定请求体类型为JSON
    },
  })
  .then(response => {
    console.log('API请求成功', response);
  })
  .catch(error => {
    console.error('API请求失败', error);
  });
}
</script>

<style lang="css" scoped>
  .device-card {
    border: none;
    background-color: #0100016b;
    border-radius: 15px;
    height: 22vh;
    width: 8vw;
  }
  .device-icon {
    height: 100px;
    width: 93.88px;
    margin-bottom: 10px;
    cursor: pointer; /* 添加鼠标手型，表示可点击 */
  }
  .device-name {
    font-size: 15px;
    font-weight: 400;
    color: #fff;
  }
</style>