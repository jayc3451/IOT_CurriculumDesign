<template>
  <e-charts class="TempChart" :option="option" ref="echartsRef"/>
</template>

<script lang="ts" setup>
import { ref, watch, onBeforeMount } from 'vue';
import * as echarts from 'echarts/core';
import ECharts from 'vue-echarts';
import { GridComponent,
  TitleComponent,
  TooltipComponent,
  LegendComponent,
 } from 'echarts/components';
import { UniversalTransition } from 'echarts/features';
import { CanvasRenderer } from 'echarts/renderers';
import { temp_2, humi_2 } from '@/api/getDevProp';

echarts.use([
  GridComponent,
  CanvasRenderer,
  UniversalTransition,
  TitleComponent,
  TooltipComponent,
  LegendComponent
]); 

interface DeviceProperties {
  time: number;
  value: any;
}
const tempHistory = ref<DeviceProperties[]>([]);
const humiHistory = ref<DeviceProperties[]>([]);
const dataLength = 600;
const lastUpdateTime = ref<number>(0); // 维护一个全局的上次更新时间戳

// 将温度和湿度数据添加到历史记录中
function addData(newTempData: DeviceProperties, newHumiData: DeviceProperties) {
  const currentTime = newTempData.time;
  if (currentTime !== lastUpdateTime.value) {
    // 更新温度和湿度历史数据
    tempHistory.value.push(newTempData);
    humiHistory.value.push(newHumiData);

    // 确保历史数据不超过指定长度
    if (tempHistory.value.length > dataLength) {
      tempHistory.value.shift();
      humiHistory.value.shift();
    }

    lastUpdateTime.value = currentTime; // 更新时间戳
  }
}

// 由于temp_2和humi_2是同步更新的，我们只需要监听其中一个即可
watch(temp_2, (newData) => {
  if (newData && humi_2.value) {
    addData({
      time: newData.time,
      value: newData.value
    }, {
      time: humi_2.value.time,
      value: humi_2.value.value
    });
  }
});

const echartsRef = ref<InstanceType<typeof ECharts> | null>(null);

// 初始化 tempHistory 和 humiHistory
onBeforeMount(() => {
  tempHistory.value = Array.from({ length: 60 }, () => ({
    time: 1730649600000, // 初始化时时间为 2024/11/1/ 0:00:00
    value: ''
  }));
  humiHistory.value = Array.from({ length: 60 }, () => ({
    time: 1730649600000,
    value: ''
  }));
});

setInterval(() => {
  updateChart();
}, 1000);

// 更新图表数据
function updateChart() {
  const chart = echartsRef.value;
  if (chart) {
    chart.setOption({
      xAxis: {
        data: tempHistory.value.map(item => {
          const date = new Date(item.time); // 将Unix时间戳转换为Date对象 onenet提供毫秒级时间戳
          const hours = String(date.getHours()).padStart(2, '0'); // 确保小时是两位数
          const minutes = String(date.getMinutes()).padStart(2, '0'); // 确保分钟是两位数
          const seconds = String(date.getSeconds()).padStart(2, '0'); // 确保秒是两位数
          return `${hours}:${minutes}:${seconds}`; // 格式化为时分秒
        })
      },
      series: [{
        name: '温度',
        data: tempHistory.value.map(item => item.value)
      }, {
        name: '湿度',
        data: humiHistory.value.map(item => item.value)
      }]
    });
  }
}

const option = ref({
  title:  {
    show: true,
    text: '办公室②温湿度实时数据',
    left: 'center',
    top: '5%',
    textStyle: {
      color: '#fff',
      fontWeight: 'normal',
      fontSize: 18,
    },
  },
  grid:{
    show: false,
    borderColor: '#fff',
    top: '25%',
    left: '3%',
    right: '3%',
    bottom: '10%',
    width: '94%',
    containLabel: true,
  },
  legend: [
    { 
      left: '5%',
      bottom: '1%',      
      textStyle: {
        color: '#fff',
        fontType: 'italic',
      },
      data: ['温度'],
    }, 
    { 
      right: '5%',
      bottom: '1%',      
      textStyle: {
        color: '#fff',
        fontType: 'italic',
      },
      data: ['湿度'],
    },       
  ],  
  tooltip: {
    show: true,
    trigger: 'axis',
    formatter: function (params: any) {
      var result = '';
      result += params[0].name + '<br/>温度 : ' + params[0].value + ' °C' +
        '<br/>湿度 : ' + params[1].value + ' %';
      return result;
    },
    axisPointer: {
      animation: false
    }
  },
  xAxis: {
    type: 'category',
    data: [],
    boundaryGap: false,
    axisTick: {
      show: true,
      inside: true,
      alignWithLabel: true, // 使刻度与标签对齐 
    },      
    axisLine: {
      onZero: false,
      lineStyle: {
        color: '#fff'
      }
    },
    axisLabel: {
      fontStyle: 'italic', 
    },
    splitLine: {  
      show: false,
    },
  },
  yAxis: [{
    type: 'value',
    show: true,
    name: '温度/℃',
    nameTextStyle: {
      color: '#fff',
      fontStyle: 'italic',
      align: 'left',
    },
    axisTick: {
      show: false,
      inside: true,
    },
    axisLine: {
      onZero: false,
      lineStyle: {
        show: true,
        color: '#fff'
      }
    },
    axisLabel: {
      color: '#fff',
      fontStyle: 'italic',
    },
    // boundaryGap: ['20%', '80%'],
    max: 40,
    min: 0,
    splitLine: {  
      show: false  
    },  
  },
  {
    type: 'value',
    position: 'right',
    name: '湿度/%',
    nameTextStyle: {
      color: '#fff',
      fontStyle: 'italic',
      align: 'right',
    },
    axisTick: {
      show: false,
      inside: true,
    },
    axisLine: {
      onZero: false,
      lineStyle: {
        show: true,
        color: '#fff'
      }
    },
    axisLabel: {
      color: '#fff',
      fontStyle: 'italic',
    },
    boundaryGap: [0, '20%'],
    splitLine: {  
      show: false  
    },  
  }],
  series: [
    {
      name: '温度',
      yAxisIndex: 0,
      type: 'line',
      smooth: true,
      align: 'right',
      showSymbol: false,
      lineStyle: {
        width: 1,
        color: '#60df54',
      },
      itemStyle: {
        color: '#60df54',
        opacity: 0.7,
      },
      data: [],
    },
    {
      name: '湿度',
      yAxisIndex: 1,
      type: 'line',
      smooth: true,
      align: 'right',
      showSymbol: false,
      lineStyle: {
        width: 1,
        color: '#5ac2ee',
      },
      itemStyle: {
        color: '#5ac2ee',
        opacity: 0.7,
      },
      data: [],
    }
  ]
});

</script>

<style scoped lang="scss">
  .TempChart {
    background-color: #0100016b;
    border-radius: 15px;
    padding-top: 1vh;
    padding-bottom: 1vh;
    padding-left: 1vw;
    padding-right: 1vw;
    height: 35vh;
    width: 37vw;    // 左右有1vw的padding，所以宽度为39-2=37vw
  }
</style>