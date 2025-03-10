<template>
    <e-charts class="CO2chart" :option="option" ref="echartsRef"/>
</template>

<script lang="ts" setup>
import { ref, watch, onBeforeMount } from 'vue';
import * as echarts from 'echarts/core';
import ECharts from 'vue-echarts';
import { GridComponent,
  TitleComponent,
  TooltipComponent,
  LegendComponent
 } from 'echarts/components';
import { LineChart } from 'echarts/charts';
import { UniversalTransition } from 'echarts/features';
import { CanvasRenderer } from 'echarts/renderers';
import { co2_1, ch2o_1 } from '@/api/getDevProp';

echarts.use([GridComponent, LineChart, CanvasRenderer, UniversalTransition, 
TitleComponent, TooltipComponent, LegendComponent]); // 引入组件

interface DeviceProperties {
  time: number;
  value: any;
}
const co2History = ref<DeviceProperties[]>([]);
const ch2oHistory = ref<DeviceProperties[]>([]);
const dataLength = 600;
const lastUpdateTime = ref<number>(0); // 维护一个全局的上次更新时间戳

// 将 CO2 和 CH2O 数据添加到历史记录中
function addData(newCo2Data: DeviceProperties, newCh2oData: DeviceProperties) {
  const currentTime = newCo2Data.time;
  if (currentTime !== lastUpdateTime.value) {
    // 更新 CO2 和 CH2O 历史数据
    co2History.value.push(newCo2Data);
    ch2oHistory.value.push(newCh2oData);

    // 确保历史数据不超过指定长度
    if (co2History.value.length > dataLength) {
      co2History.value.shift();
      ch2oHistory.value.shift();
    }

    lastUpdateTime.value = currentTime; // 更新时间戳
  }
}

// 由于 co2_1 和 ch2o_1 是同步更新的，我们只需要监听其中一个即可
watch(co2_1, (newData) => {
  if (newData && ch2o_1.value) {
    addData({
      time: newData.time,
      value: newData.value
    }, {
      time: ch2o_1.value.time,
      value: ch2o_1.value.value
    });
  }
});

const echartsRef = ref<InstanceType<typeof ECharts> | null>(null);

setInterval(() => {
  updateChart();
}, 1000);

// 初始化 co2History 和 ch2oHistory
onBeforeMount(() => {
  co2History.value = Array.from({ length: 60 }, () => ({
    time: 1730649600000, // 初始化时时间为 2024/11/1/ 0:00:00
    value: ''
  }));
  ch2oHistory.value = Array.from({ length: 60 }, () => ({
    time: 1730649600000,
    value: ''
  }));
});

// 更新图表数据
function updateChart() {
  const chart = echartsRef.value;
  if (chart) {
    chart.setOption({
      xAxis: {
        data: co2History.value.map(item => {
          const date = new Date(item.time); // 将Unix时间戳转换为Date对象 onenet提供毫秒级时间戳
          const hours = String(date.getHours()).padStart(2, '0'); // 确保小时是两位数
          const minutes = String(date.getMinutes()).padStart(2, '0'); // 确保分钟是两位数
          const seconds = String(date.getSeconds()).padStart(2, '0'); // 确保秒是两位数
          return `${hours}:${minutes}:${seconds}`; // 格式化为时分秒
        })
      },
      series: [{
        name: 'CO₂浓度',
        data: co2History.value.map(item => item.value)
      }, {
        name: 'CH₂O浓度',
        data: ch2oHistory.value.map(item => item.value)
      }]
    });
  }
}

const option = ref({
    title:  {
      show: true,
      text: '办公室①空气质量实时检测',
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
        data: ['CO₂浓度'],
      }, 
      { 
        right: '5%',
        bottom: '1%',      
        textStyle: {
          color: '#fff',
          fontType: 'italic',
        },
        data: ['CH₂O浓度'],
      },       
    ],  
    tooltip: {
      show: true,
      trigger: 'axis',
      formatter: function (params: any) {
        var result = '';
        result += params[0].name + '<br/>CO₂浓度 : ' + params[0].value + ' ppm' +
         '<br/>CH₂O浓度 : ' + params[1].value + ' mg/m³';
        return result;
      },
      axisPointer: {
        animation: false
      }
    },
    xAxis: {
      type: 'category',
      data: [],
      show: true,
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
      name: 'CO₂浓度/(ppm)',
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
      boundaryGap: [0, '80%'],
      splitLine: {  
        show: false  
      },  
    },
    {
      type: 'value',
      position: 'right',
      name: 'CH₂O浓度/(mg/m³)',
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
      boundaryGap: [0, '10%'],
      splitLine: {  
        show: false  
      },  
    }],
    series: [
      {
        name: 'CO₂浓度',
        yAxisIndex: 0,
        type: 'line',
        smooth: true,
        showSymbol: false,
        lineStyle: {
          width: 1,
          color: '#9ea9ae',
        },
        itemStyle: {
          color: '#9ea9ae',
          opacity: 0.7
        },
        data: [],
      },
      {
        name: 'CH₂O浓度',
        yAxisIndex: 1,
        type: 'line',
        smooth: true,
        showSymbol: false,
        lineStyle: {
          width: 1,
          color: '#beeb7b',
        },
        itemStyle: {
          color: '#beeb7b',
          opacity: 0.7
        },
        data: [],
    }
  ]
});
</script>

<style scoped lang="scss">
  .CO2chart {
    background-color: #0100016b;
    border-radius: 15px;
    height: 35vh;
    width: 29.24vw;
    padding-top: 1vh;
    padding-bottom: 1vh;
    padding-left: 1vw;
    padding-right: 1vw;
  }
</style>