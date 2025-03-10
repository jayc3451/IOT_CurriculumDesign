import { ref, computed } from 'vue';
import axios from 'axios';

interface DevicePropertiesResponse {
  code: number;
  data: DeviceProperties[];
}

interface DeviceProperties {
  "identifier": string;
  "time": number;
  "value": any;
}

const authorizationTokens = [
  'version=2018-10-31&res=products%2FOcFyEAzH58%2Fdevices%2Foffice_1&et=1761720044&method=md5&sign=JFXruNyGm%2BFixVErw7Mmdg%3D%3D',
  'version=2018-10-31&res=products%2FOcFyEAzH58%2Fdevices%2Foffice_2&et=1762233160&method=md5&sign=mCWMJY%2BFSHLZzcBlstleCw%3D%3D'
];

const Device_1_PropertiesResponse = ref<DevicePropertiesResponse>({} as DevicePropertiesResponse);
const Device_2_PropertiesResponse = ref<DevicePropertiesResponse>({} as DevicePropertiesResponse);

// 使用 createComputedProperty 函数创建每个计算属性
const createComputedProperty = ( deviceName: string , index: number) => {
  return computed(() => {
    if( deviceName === 'office_1' ) {
      const data = Device_1_PropertiesResponse.value?.data;
      return data && data.length > index ? data[index] as DeviceProperties : undefined;
    }
    else if( deviceName === 'office_2' ) {
      const data = Device_2_PropertiesResponse.value?.data;
      return data && data.length > index ? data[index] as DeviceProperties : undefined;
     }
  });
};


const airconductor_1 = createComputedProperty('office_1', 1);
const ch2o_1 = createComputedProperty('office_1', 2);
const co2_1 = createComputedProperty('office_1', 3);
const exist_1 = createComputedProperty('office_1', 4); 
const fan_1 = createComputedProperty('office_1', 5);
const fire_1 = createComputedProperty('office_1', 6);
const humi_1 = createComputedProperty('office_1', 7);
const light_1 = createComputedProperty('office_1', 8);
const temp_1 = createComputedProperty('office_1', 10);


const airconductor_2 = createComputedProperty('office_2', 1);
const ch2o_2 = createComputedProperty('office_2', 2);
const co2_2 = createComputedProperty('office_2', 3);
const exist_2 = createComputedProperty('office_2', 4);
const fan_2 = createComputedProperty('office_2', 5);
const fire_2 = createComputedProperty('office_2', 6);
const humi_2 = createComputedProperty('office_2', 7);
const light_2 = createComputedProperty('office_2', 8);
const temp_2 = createComputedProperty('office_2', 10);


const fetchDeviceProperties  = async ( deviceName: string ) => {
  let authorizationToken = '';
  if( deviceName === 'office_1' ) {
    authorizationToken = authorizationTokens[0];
  }
  else if( deviceName === 'office_2' ) {
    authorizationToken = authorizationTokens[1];
  }

  axios.get('https://iot-api.heclouds.com/thingmodel/query-device-property', {
    params: {
      product_id: 'OcFyEAzH58',
      device_name: deviceName
    },
    headers: {
      'authorization': authorizationToken
    }
  }).then(response => {         // 获取设备属性成功
    if( deviceName === 'office_1' ) Device_1_PropertiesResponse.value = response.data;
    else if( deviceName === 'office_2' ) Device_2_PropertiesResponse.value = response.data;

    console.log('office1',Device_1_PropertiesResponse.value);
    // console.log('office2',Device_2_PropertiesResponse.value);
  }).catch(error => {
    console.error('获取设备属性失败:', error);
  });
}

// 2秒更新一次数据
setInterval(()=>fetchDeviceProperties('office_1') , 1000);
setInterval(()=>fetchDeviceProperties('office_2') , 1000);

export {airconductor_1, ch2o_1, co2_1, exist_1, fan_1, fire_1, humi_1, light_1, temp_1}
export {airconductor_2, ch2o_2, co2_2, exist_2, fan_2, fire_2, humi_2, light_2, temp_2}
