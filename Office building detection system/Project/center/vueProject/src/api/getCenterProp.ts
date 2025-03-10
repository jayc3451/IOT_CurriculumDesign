import axios from 'axios';
import { ref, computed } from 'vue';

interface DevicePropertiesResponse {
  code: number;
  data: DeviceProperties[];
}

interface DeviceProperties {
  identifier: string;
  time: number;
  value: any;
}

const center_PropertiesResponse = ref<DevicePropertiesResponse>({} as DevicePropertiesResponse);

const fetchDeviceProperties = async () => {
  let authorizationToken = 'version=2018-10-31&res=products%2FOcFyEAzH58%2Fdevices%2Fcenter&et=1762233160&method=md5&sign=Epu9EknM3DSEmUGfxXHQ8g%3D%3D';

  try {
    const response = await axios.get('https://iot-api.heclouds.com/thingmodel/query-device-property', {
      params: {
        product_id: 'OcFyEAzH58',
        device_name: 'center'
      },
      headers: {
        'authorization': authorizationToken
      }
    });
    // 获取设备属性成功
    center_PropertiesResponse.value = response.data;

    // console.log(center_PropertiesResponse.value);
  } catch (error) {
    console.error('获取设备属性失败:', error);
  }
}

const createComputedProperty = (index: number) => {
  return computed(() => {
    const data = center_PropertiesResponse.value?.data;
    return data && data.length > index ? data[index] as DeviceProperties : undefined;
  });
};

const accessInfo = createComputedProperty(0);

// 2ss 请求一次
setInterval(() => {
  fetchDeviceProperties();
  // console.log('accessInfo:', accessInfo.value);
}, 2000);


export { accessInfo }