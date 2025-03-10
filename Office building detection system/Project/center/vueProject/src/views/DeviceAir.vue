<template>
  <el-row>
    <el-card class="device-card" style="margin-right:1vw">
      <el-row justify="center">
        <el-tooltip        
          class="box-item"
          effect="light"
          :content="air1_allow ? '已允许自动切换' : '已强制关闭'"
          placement="right-start"
        >
        <img class="device-icon" v-if="airconductor_1?.value === 'true'" @click="toggleAirconductor(1)" src="/airconductorOn.svg"/>
        <img class="device-icon" v-else @click="toggleAirconductor(1)" src="/airconductorOff.svg"/>
      </el-tooltip>
      </el-row>
      <el-row justify="center">
        <span class="device-name">办公室①空调</span>
      </el-row>
    </el-card>
    <el-card class="device-card">
      <el-row justify="center">
        <el-tooltip        
          class="box-item"
          effect="light"
          :content="air2_allow ? '已允许自动切换' : '已强制关闭'"
          placement="left-end"
        >
          <img class="device-icon" v-if="airconductor_2?.value === 'true'" @click="toggleAirconductor(2)" src="/airconductorOn.svg"/>
          <img class="device-icon" v-else @click="toggleAirconductor(2)" src="/airconductorOff.svg"/>
        </el-tooltip>
        </el-row>
      <el-row justify="center">
        <span class="device-name">办公室②空调</span>
      </el-row>
    </el-card>
  </el-row>
</template>

<script lang="ts" setup>
import { airconductor_1, airconductor_2 } from '@/api/getDevProp';
import axios from 'axios';
import { ref } from 'vue';

const air1_allow = ref(true);   // 定义变量，用于控制是否允许自动切换灯光状态
const air2_allow = ref(true);

// 定义API URL和设备信息
const oneNetUrl = 'https://iot-api.heclouds.com/thingmodel/set-device-property';
const productId = 'OcFyEAzH58';
const deviceNames = ['office_1', 'office_2']; // 定义设备名称数组
const airconductorIdentifier = 'airconductor';

// 长期token
const authorizationTokens = [
  'version=2018-10-31&res=products%2FOcFyEAzH58%2Fdevices%2Foffice_1&et=1761720044&method=md5&sign=JFXruNyGm%2BFixVErw7Mmdg%3D%3D',
  'version=2018-10-31&res=products%2FOcFyEAzH58%2Fdevices%2Foffice_2&et=1762233160&method=md5&sign=mCWMJY%2BFSHLZzcBlstleCw%3D%3D'
];

// 切换空调状态并调用API
function toggleAirconductor(airconductorId: number) {
  let value, deviceName, token;
  if (airconductorId === 1) {
    air1_allow.value = !air1_allow.value;
    value = air1_allow.value;
    deviceName = deviceNames[0]; // 获取对应的deviceName
    token = authorizationTokens[0]; // 获取对应的token
  }
  else {
    air2_allow.value = !air2_allow.value;
    value = air2_allow.value;
    deviceName = deviceNames[1]; // 获取对应的deviceName
    token = authorizationTokens[1]; // 获取对应的token
  }

  // 构建API请求体，符合oneJson格式
  const data = {
    product_id: productId,
    device_name: deviceName,
    params: {
      [airconductorIdentifier]: value,
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